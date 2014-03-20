/*
 * Half-Life Weapon Mod
 * Copyright (c) 2012 - 2014 AGHL.RU Dev Team
 * 
 * http://aghl.ru/forum/ - Russian Half-Life and Adrenaline Gamer Community
 *
 *
 *    This program is free software; you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation; either version 2 of the License, or (at
 *    your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software Foundation,
 *    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    In addition, as a special exception, the author gives permission to
 *    link the code of this program with the Half-Life Game Engine ("HL
 *    Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *    L.L.C ("Valve").  You must obey the GNU General Public License in all
 *    respects for all of the code used other than the HL Engine and MODs
 *    from Valve.  If you modify this file, you may extend this exception
 *    to your version of the file, but you are not obligated to do so.  If
 *    you do not wish to do so, delete this exception statement from your
 *    version.
 *
 */

#include "wpnmod_config.h"
#include "wpnmod_parse.h"
#include "wpnmod_memory.h"


bool g_bWorldSpawned = false;

int AmxxCheckGame(const char* game)
{
	return !stricmp(game, "cstrike") || !stricmp(game, "czero") ? AMXX_GAME_BAD : AMXX_GAME_OK;
}

// Called by Meta_Attach.
int WpnMod_Init(void)
{
	g_log.Init();
	WPNMOD_LOG("Start.\n");
#ifdef __linux__
	WPNMOD_LOG(" Version %s Linux\n", Plugin_info.version);
#else
	WPNMOD_LOG(" Version %s Windows\n", Plugin_info.version);
#endif
	if (!FindModuleByAddr((void*)MDLL_FUNC->pfnGetGameDescription(), &g_GameDllModule))
	{
		WPNMOD_LOG("  Failed to locate %s\n", GET_GAME_INFO(PLID, GINFO_DLL_FILENAME));
		WPNMOD_LOG("Errors occurred. Please visit http://aghl.ru/forum/ for support.\n");
		return 0;
	}

	WPNMOD_LOG("  Found %s at %p\n", GET_GAME_INFO(PLID, GINFO_DLL_FILENAME), g_GameDllModule.base);

	size_t start = (size_t)g_GameDllModule.base;
	size_t end = (size_t)g_GameDllModule.base + (size_t)g_GameDllModule.size;

	if (!FindFuncsInDll(start, end))
	{
		WPNMOD_LOG("Errors occurred. Please visit http://aghl.ru/forum/ for support.\n");
		return 0;
	}

	WPNMOD_LOG("Done.\n");
	return 1;
}

void Game_Init(void)
{
	static bool bInitOnce = false;

	if (!bInitOnce)
	{
		g_GameMod = CheckSubMod(MF_GetModname());

		pvData_Init();
		Vtable_Init();

		g_Ents = new EntData[gpGlobals->maxEntities];
		g_pCurrentSlots = new int* [g_iMaxWeaponSlots];

		for (int i = 0; i < g_iMaxWeaponSlots; ++i)
		{
			memset((g_pCurrentSlots[i] = new int [g_iMaxWeaponPositions]), 0, sizeof(int) * g_iMaxWeaponPositions);
		}

		bInitOnce = true;
	}
}

void World_Precache(void)
{
	SetConfigFile();

	cvar_sv_cheats = CVAR_GET_POINTER("sv_cheats");
	cvar_mp_weaponstay = CVAR_GET_POINTER("mp_weaponstay");

	WPNMOD_LOG_ONLY("-------- Mapchange to %s --------\n", STRING(gpGlobals->mapname));

	if (ParseSection(g_ConfigFilepath, "[block]", (void*)OnParseBlockedItems, -1) && (int)g_BlockedItems.size())
	{
		WPNMOD_LOG("Blocked default items:\n");

		for (int i = 0; i < (int)g_BlockedItems.size(); i++)
		{
			WPNMOD_LOG(" \"%s\"\n", g_BlockedItems[i]->classname);
		}
	}
}

void OnAmxxAttach(void)
{
	cvar_t version = 
	{
		"hl_wpnmod_version",
		Plugin_info.version,
		FCVAR_SERVER
	};

	MF_AddNatives(Natives);
	REG_SVR_COMMAND("wpnmod", WpnModCommand);
	CVAR_REGISTER(&version);
}

void OnAmxxDetach(void)
{
	for (int i = 0; i < Func_End; i++)
	{
		if (g_dllFuncs[i].done)
		{
			UnsetHook(&g_dllFuncs[i]);
		}
	}

	for (int i = 0; i < g_iMaxWeaponSlots; ++i)
	{
		delete [] g_pCurrentSlots[i];
	}

	delete [] g_Ents;
	delete [] g_pCurrentSlots;
}

void ServerActivate_Post(edict_t *pEdictList, int edictCount, int clientMax)
{
	ParseBSP();
	ParseSpawnPoints();

	// Parse default equipments and ammo.
	ParseSection(g_ConfigFilepath, "[ammo]", (void*)OnParseStartAmmos, ':');
	ParseSection(g_ConfigFilepath, "[equipment]", (void*)OnParseStartEquipments	, ':');

	// Remove blocked items from map.
	for (int i = 0; i < (int)g_BlockedItems.size(); i++)
	{
		edict_t *pFind = FIND_ENTITY_BY_CLASSNAME(NULL, g_BlockedItems[i]->classname);

		while (!FNullEnt(pFind))
		{
			pFind->v.flags |= FL_KILLME;
			pFind = FIND_ENTITY_BY_CLASSNAME(pFind, g_BlockedItems[i]->classname);
		}
	}

	EnableShieldHitboxTracing();

	if (ParseSection(g_ConfigFilepath, "[weaponbox]", (void*)OnParseWeaponbox, ':'))
	{
		EnableWeaponboxModels();
	}

	SetHookVirtual(&g_PlayerSpawn_Hook);
	SetHookVirtual(&g_PlayerPostThink_Hook);

	RETURN_META(MRES_IGNORED);
}

void ServerDeactivate()
{
	g_EquipEnt = 0;

	g_iWeaponsCount = 0;
	g_iWeaponInitID = 0;
	g_iAmmoBoxIndex = 0;

	memset(WeaponInfoArray, 0, sizeof(WeaponInfoArray));
	memset(AmmoBoxInfoArray, 0, sizeof(AmmoBoxInfoArray));

	for (int i = 0; i < g_iMaxWeaponSlots; ++i)
	{
		memset(g_pCurrentSlots[i], 0, sizeof(int) * g_iMaxWeaponPositions);
	}

	for (int i = 0; i < (int)g_Decals.size(); i++)
	{
		delete g_Decals[i];
	}

	for (int i = 0; i < (int)g_StartAmmo.size(); i++)
	{
		delete g_StartAmmo[i];
	}

	for (int i = 0; i < (int)g_BlockedItems.size(); i++)
	{
		UnsetHookVirtual(g_BlockedItems[i]);
		delete g_BlockedItems[i];
	}

	g_Decals.clear();
	g_StartAmmo.clear();
	g_BlockedItems.clear();

	g_bWorldSpawned = false;
	g_bCrowbarHooked = false;
	g_bAmmoBoxHooked = false;

	for (int i = 0; i < CrowbarHook_End; i++)
	{
		UnsetHookVirtual(&g_CrowbarHooks[i]);
	}

	if (g_funcPackWeapon.done)
	{
		UnsetHook(&g_funcPackWeapon);
	}

	g_funcPackWeapon.address = NULL;

	UnsetHookVirtual(&g_RpgAddAmmo_Hook);
	UnsetHookVirtual(&g_PlayerSpawn_Hook);
	UnsetHookVirtual(&g_PlayerPostThink_Hook);

	g_iWpnBoxLifeTime = 120;

	RETURN_META(MRES_IGNORED);
}

void FN_OnFreeEntPrivateData(edict_t *pEnt)
{
	if (g_Ents != NULL && IsValidPev(pEnt))
	{
		int iEntity = ENTINDEX(pEnt);

		g_Ents[iEntity].iThink = NULL;
		g_Ents[iEntity].iTouch = NULL;
		g_Ents[iEntity].iExplode = NULL;
	}

	RETURN_META(MRES_IGNORED);
}

int FN_DecalIndex_Post(const char *name)
{
	DecalList *p = new DecalList;

	p->name = STRING(ALLOC_STRING(name));
	p->index = META_RESULT_ORIG_RET(int);

	g_Decals.push_back(p);

	RETURN_META_VALUE(MRES_IGNORED, 0);
}

int DispatchSpawn(edict_t *pent)
{
	if (!g_bWorldSpawned)
	{
		g_bWorldSpawned = true;

		Game_Init();
		World_Precache();
	}

	RETURN_META_VALUE(MRES_IGNORED, 0);
}

void ClientCommand(edict_t *pEntity)
{
	static const char* cmd = NULL;

	cmd = CMD_ARGV(0);

	if (!cmd || !MF_IsPlayerIngame(ENTINDEX(pEntity)))
	{
		RETURN_META(MRES_IGNORED);
	}

	if (!strcmp(cmd, "lastinv"))
	{
		SelectLastItem(pEntity);
		RETURN_META(MRES_SUPERCEDE);
	}
	else if (strstr(cmd, "weapon_"))
	{
		SelectItem(pEntity, cmd);
		RETURN_META(MRES_SUPERCEDE);
	}
	else if (!_stricmp(cmd, "give") && cvar_sv_cheats->value)
	{
		const char* item = CMD_ARGV(1);

		if (item)
		{
			GiveNamedItem(pEntity, item);
		}
	}
	else if (!_stricmp(cmd, "wpnmod"))
	{
		int i = 0;
		int ammo = 0;
		int weapons = 0;

		static char buf[1024];
		size_t len = 0;

		sprintf(buf, "\n%s %s\n", Plugin_info.name, Plugin_info.version);
		CLIENT_PRINT(pEntity, print_console, buf);
		len = sprintf(buf, "Author: \n         KORD_12.7 (AGHL.RU Dev Team)\n");
		len += sprintf(&buf[len], "Credits: \n         AMXX Dev team, Arkshine, 6a6kin, GordonFreeman, Koshak, Lev, noo00oob\n");
		len += sprintf(&buf[len], "Compiled: %s\nURL: http://www.aghl.ru/ - Russian Half-Life and Adrenaline Gamer Community.\n\n", __DATE__ ", " __TIME__);
		CLIENT_PRINT(pEntity, print_console, buf);

		CLIENT_PRINT(pEntity, print_console, "Currently loaded weapons:\n");

		for (i = 1; i <= g_iWeaponsCount; i++)
		{
			if (WeaponInfoArray[i].iType == Wpn_Custom)
			{
				sprintf(buf, " [%2d] %-23.22s\n", ++weapons, GetWeapon_pszName(i));
				CLIENT_PRINT(pEntity, print_console, buf);
			}
		}

		CLIENT_PRINT(pEntity, print_console, "\nCurrently loaded ammo:\n");

		for (i = 1; i <= g_iAmmoBoxIndex; i++)
		{
			sprintf(buf, " [%2d] %-23.22s\n", ++ammo, AmmoBoxInfoArray[i].classname.c_str());
			CLIENT_PRINT(pEntity, print_console, buf);
		}

		CLIENT_PRINT(pEntity, print_console, "\nTotal:\n");
		sprintf(buf, "%4d items (%d weapons, %d ammo).\n\n", weapons + ammo, weapons, ammo);
		CLIENT_PRINT(pEntity, print_console, buf);

		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}

void WpnModCommand(void)
{
	const char *cmd = CMD_ARGV(1);

	if (!strcmp(cmd, "credits"))
	{
		printf2("Credits:\n\tAMXX Dev team, 6a6kin, GordonFreeman, Koshak, Lev, noo00oob.\n");
	}
	else  if (!strcmp(cmd, "version")) 
	{
		printf2("%s %s (%s)\n", Plugin_info.name, Plugin_info.version, Plugin_info.url);
		printf2("Author:\n\tKORD_12.7 (AGHL.RU Dev Team)\n");
		printf2("Compiled: %s\n", __DATE__ ", " __TIME__);

	}
	else if (!strcmp(cmd, "edicts"))
	{
		for (int i = 0; i < gpGlobals->maxEntities; i++)
		{
			if (IsValidPev(INDEXENT2(i)))
			{
				printf2("\t*Edict %d (%s)\n", i, STRING(INDEXENT2(i)->v.classname));
			}
		}
	}
	else if (!strcmp(cmd, "items"))
	{
		int i = 0;
		int items = 0;
		int weapons = 0;
		int ammo = 0;

		printf2("\nCurrently loaded weapons:\n");

		for (i = 1; i <= g_iWeaponsCount; i++)
		{
			if (WeaponInfoArray[i].iType == Wpn_Custom)
			{
				items++;
				printf2(" [%2d] %-23.22s\n", ++weapons, GetWeapon_pszName(i));
			}
		}

		printf2("\nCurrently loaded ammo:\n");

		for (i = 1; i <= g_iAmmoBoxIndex; i++)
		{
			items++;
			printf2(" [%2d] %-23.22s\n", ++ammo, AmmoBoxInfoArray[i].classname.c_str());
		}

		printf2("\nTotal:\n");
		printf2("%4d items (%d weapons, %d ammo).\n\n", items, weapons, ammo);
	}
	else if (!strcmp(cmd, "gpl"))
	{
		printf2("Half-Life Weapon Mod\n");
		printf2("\n");
		printf2(" by the AGHL.RU Dev Team\n");
		printf2("\n");
		printf2("\n");
		printf2("  This program is free software; you can redistribute it and/or modify it\n");
		printf2("  under the terms of the GNU General Public License as published by the\n");
		printf2("  Free Software Foundation; either version 2 of the License, or (at\n");
		printf2("  your option) any later version.\n");
		printf2("\n");
		printf2("  This program is distributed in the hope that it will be useful, but\n");
		printf2("  WITHOUT ANY WARRANTY; without even the implied warranty of\n");
		printf2("  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU\n");
		printf2("  General Public License for more details.\n");
		printf2("\n");
		printf2("  You should have received a copy of the GNU General Public License\n");
		printf2("  along with this program; if not, write to the Free Software Foundation,\n");
		printf2("  Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA\n");
		printf2("\n");
		printf2("  In addition, as a special exception, the author gives permission to\n");
		printf2("  link the code of this program with the Half-Life Game Engine (\"HL\n");
		printf2("  Engine\") and Modified Game Libraries (\"MODs\") developed by Valve,\n");
		printf2("  L.L.C (\"Valve\"). You must obey the GNU General Public License in all\n");
		printf2("  respects for all of the code used other than the HL Engine and MODs\n");
		printf2("  from Valve. If you modify this file, you may extend this exception\n");
		printf2("  to your version of the file, but you are not obligated to do so. If\n");
		printf2("  you do not wish to do so, delete this exception statement from your\n");
		printf2("  version.\n");
		printf2("\n");
	}
	else
	{
		// Unknown command
		printf2("\nUsage: wpnmod < command > [ argument ]\n");
		printf2("Commands:\n");
		printf2("   %-10s - %s\n", "version", "displays version information.");
		printf2("   %-10s - %s\n", "credits", "displays credits information.");
		printf2("   %-10s - %s\n", "items", "displays information about registered weapons and ammo.");
		printf2("   %-10s - %s\n", "gpl", "print the license.");
		printf2("\n");
	}
}
