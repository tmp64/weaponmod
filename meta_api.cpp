/*
 * Half-Life Weapon Mod
 * Copyright (c) 2012 AGHL.RU Dev Team
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

#include "weaponmod.h"
#include "hooks.h"
#include "utils.h"



EntData *g_Ents = NULL;

cvar_t *cvar_aghlru = NULL;
cvar_t *cvar_sv_cheats = NULL;
cvar_t *cvar_mp_weaponstay = NULL;


void OnAmxxAttach()
{
	BOOL bAddNatives = TRUE; 

	cvar_aghlru = CVAR_GET_POINTER("aghl.ru");
	cvar_sv_cheats = CVAR_GET_POINTER("sv_cheats");
	cvar_mp_weaponstay = CVAR_GET_POINTER("mp_weaponstay");
	
	if (!FindModuleByAddr((void*)MDLL_FUNC->pfnGetGameDescription(), &hl_dll))
	{
		print_srvconsole("[WEAPONMOD] Failed to locate %s\n", GET_GAME_INFO(PLID, GINFO_DLL_FILENAME));
		bAddNatives = FALSE;
	}
	else
	{
		for (int i = 0; i < Func_End; i++)
		{
			if (cvar_aghlru)
			{
				g_dllFuncs[i].sig = g_dllFuncs[i].sigCustom;
			}

			if (CreateFunctionHook(&g_dllFuncs[i]))
			{
				SetHook(&g_dllFuncs[i]);
			}
		}
	}

	for (int i = 0; i < Func_End; i++)
	{
		if (!g_dllFuncs[i].address)
		{
			print_srvconsole("[WEAPONMOD] Failed to find \"%s\" function.\n", g_dllFuncs[i].name);
			bAddNatives = FALSE;
		}
	}
	
	if (!bAddNatives)
	{
		print_srvconsole("[WEAPONMOD] Cannot register natives.\n");
	}
	else
	{
		MF_AddNatives(Natives);
		print_srvconsole("[WEAPONMOD] Found %s at %p\n", GET_GAME_INFO(PLID, GINFO_DLL_FILENAME), hl_dll.base);

		print_srvconsole("\n   Half-Life Weapon Mod version %s Copyright (c) 2012 AGHL.RU Dev Team. \n"
						"   Weapon Mod comes with ABSOLUTELY NO WARRANTY; for details type `wpnmod gpl'.\n", Plugin_info.version);
		print_srvconsole("   This is free software and you are welcome to redistribute it under \n"
						"   certain conditions; type 'wpnmod gpl' for details.\n  \n");
	}

	g_Ents = new EntData[gpGlobals->maxEntities];

	cvar_t version = {"hl_wpnmod_version", Plugin_info.version, FCVAR_SERVER};
	
	REG_SVR_COMMAND("wpnmod", WpnModCommand);
	CVAR_REGISTER (&version);
}



void OnAmxxDetach()
{
	for (int i = 0; i < Func_End; i++)
	{
		if (g_dllFuncs[i].done)
		{
			UnsetHook(&g_dllFuncs[i]);
		}
	}

	delete [] g_Ents;
}



void ServerDeactivate()
{
	g_iWeaponsCount = 0;
	g_iWeaponInitID = 0;
	g_iAmmoBoxIndex = 0;
		
	memset(g_iCurrentSlots, 0, sizeof(g_iCurrentSlots));
	memset(WeaponInfoArray, 0, sizeof(WeaponInfoArray));
	memset(AmmoBoxInfoArray, 0, sizeof(AmmoBoxInfoArray));

	RETURN_META(MRES_IGNORED);
}



int AmxxCheckGame(const char *game)
{
	if (!strcasecmp(game, "valve"))
	{
		return AMXX_GAME_OK;
	}
	
	return AMXX_GAME_BAD;
}



void ClientCommand(edict_t *pEntity)
{
	const char* cmd = CMD_ARGV(0);

	if (cmd && _stricmp(cmd, "wpnmod") == 0)
	{
		int i = 0;
		int ammo = 0;
		int items = 0;
		int weapons = 0;
		
		static char buf[1024];
		size_t len = 0;
			
		sprintf(buf, "\n%s %s\n", Plugin_info.name, Plugin_info.version);
		CLIENT_PRINT(pEntity, print_console, buf);
		len = sprintf(buf, "Author: \n         KORD_12.7 (AGHL.RU Dev Team)\n");
		len += sprintf(&buf[len], "Credits: \n         AMXX Dev team, 6a6kin, GordonFreeman, Lev, noo00oob\n");
		len += sprintf(&buf[len], "Compiled: %s\nURL: http://www.aghl.ru/ - Russian Half-Life and Adrenaline Gamer Community.\n\n", __DATE__ ", " __TIME__);
		CLIENT_PRINT(pEntity, print_console, buf);

		CLIENT_PRINT(pEntity, print_console, "Currently loaded weapons:\n");

		for (i = 1; i <= g_iWeaponsCount; i++)
		{
			if (WeaponInfoArray[i].iType == Wpn_Custom)
			{
				items++;
				sprintf(buf, " [%2d] %-23.22s\n", ++weapons, GetWeapon_pszName(i));
				CLIENT_PRINT(pEntity, print_console, buf);
			}
		}

		CLIENT_PRINT(pEntity, print_console, "\nCurrently loaded ammo:\n");

		for (i = 0; i < g_iAmmoBoxIndex; i++)
		{
			items++;
			sprintf(buf, " [%2d] %-23.22s\n", ++ammo, AmmoBoxInfoArray[i].classname.c_str());
			CLIENT_PRINT(pEntity, print_console, buf);
		}

		CLIENT_PRINT(pEntity, print_console, "\nTotal:\n");
		sprintf(buf, "%4d items (%d weapons, %d ammo).\n\n", items, weapons, ammo);
		CLIENT_PRINT(pEntity, print_console, buf);

		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}



static const char* get_localinfo(const char* name , const char* def = 0)
{
	const char* b = LOCALINFO((char*)name);
	if (((b==0)||(*b==0)) && def)
		SET_LOCALINFO((char*)name,(char*)(b = def));
	return b;
}



void ServerActivate_Post(edict_t *pEdictList, int edictCount, int clientMax)
{
	char filepath[1024];

	MF_BuildPathnameR(filepath, sizeof(filepath) - 1, "maps/%s.bsp", STRING(gpGlobals->mapname));
	ParseBSPEntData(filepath);

	MF_BuildPathnameR(filepath, sizeof(filepath) - 1, "%s/weaponmod/%s.ini", get_localinfo("amxx_configsdir", "addons/amxmodx/configs"), STRING(gpGlobals->mapname));
	FILE *stream = fopen(filepath, "r");

	if (stream)
	{
		char data[2048];
		int wpns = 0, ammoboxes = 0;

		while (!feof(stream))
		{
			fgets(data, sizeof(data) - 1, stream);
			
			char *b = &data[0];

			if (*b != ';')
			{
				int i;
				int state;
				
				char* arg;
				char szData[3][32];

				for (i = 0; i < 3; i++)
				{
					arg = parse_arg(&b, state);
					strcpy(szData[i], arg);
				}
				
				for (i = 1; i <= g_iWeaponsCount; i++)
				{
					if (WeaponInfoArray[i].iType == Wpn_Custom && !_stricmp(GetWeapon_pszName(i), szData[0]))
					{
						Weapon_Spawn(i, strlen(szData[1]) ? ParseVec(szData[1]) : Vector(0, 0, 0), strlen(szData[2])  ? ParseVec(szData[2]) : Vector(0, 0, 0));
						wpns++;
					}
				}

				

				for (i = 0; i < g_iAmmoBoxIndex; i++)
				{
					if (!_stricmp(AmmoBoxInfoArray[i].classname.c_str(), szData[0]))
					{
						Ammo_Spawn(i, strlen(szData[1]) ? ParseVec(szData[1]) : Vector(0, 0, 0), strlen(szData[2]) ? ParseVec(szData[2]) : Vector(0, 0, 0));
						ammoboxes++;
					}
				}
			}
		}

		print_srvconsole("[WEAPONMOD] \"%s.ini\": spawn %d weapons and %d ammoboxes.\n", STRING(gpGlobals->mapname), wpns, ammoboxes);
		fclose(stream);
	}

	RETURN_META(MRES_IGNORED);
}

