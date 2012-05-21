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


BOOL g_Initialized;

Vector ParseVec(char *pString);
char* parse_arg(char** line, int& state);



int OnMetaAttach()
{
	print_srvconsole("\n   Half-Life Weapon Mod version %s Copyright (c) 2012 AGHL.RU Dev Team. \n"
					 "   Weapon Mod comes with ABSOLUTELY NO WARRANTY; for details type `wpnmod gpl'.\n", Plugin_info.version);
	print_srvconsole("   This is free software and you are welcome to redistribute it under \n"
					 "   certain conditions; type 'wpnmod gpl' for details.\n  \n");
	
	if (FindDllBase((void*)MDLL_FUNC->pfnGetGameDescription()))
	{
#ifdef __linux__
		void* handle = NULL;
		char* DllFileName = NULL;

		DllFileName = "dlls/hl_i386.so";
		handle = dlopen(DllFileName, RTLD_NOW);

		if (handle != NULL) 
		{
			pRadiusDamage = dlsym(handle, "RadiusDamage__FG6VectorP9entvars_sT1ffii");
			pGetAmmoIndex = dlsym(handle, "GetAmmoIndex__11CBasePlayerPCc");
			pPlayerSetAnimation = dlsym(handle, "SetAnimation__11CBasePlayer11PLAYER_ANIM");
			pPrecacheOtherWeapon = dlsym(handle, "UTIL_PrecacheOtherWeapon__FPCc");
		}

		dlclose(handle);
#elif _WIN32
		if (CVAR_GET_POINTER("aghl.ru"))
		{
			pRadiusDamage = FindFunction(	"\x83\xEC\x7C\xD9\xEE\xD9\x54\x24\x58",
											"xxxxxxxxx", 9);

			pGetAmmoIndex = FindFunction(	"\x57\x8B\x7C\x24\x08\x85\xFF\x75\x05",
											"xxxxxxxxx", 9);

			pPlayerSetAnimation = FindFunction(		"\x83\xEC\x48\xA1\x00\x00\x00\x00"
													"\x00\x00\x89\x00\x00\x00\x53\x56",
													"xxxx??????x???xx", 16);

			pPrecacheOtherWeapon = FindFunction(	"\x8B\x00\x00\x00\x00\x00\x8B\x00\x00\x00\x2B"
													"\x81\x98\x00\x00\x00\x83\xEC\x2C\x53\x50",
													"x?????x???xxx???xxxxx", 21);
		}
		else
		{
			pRadiusDamage = FindFunction(	"\xD9\x44\x24\x1C\xD8\x00\x00\x00\x00\x00\x83\xEC\x64",
											"xxxxx?????xxx", 13);

			pGetAmmoIndex = FindFunction(	"\x56\x57\x8B\x7C\x24\x0C\x85\xFF",
											"xxxxxxxx", 8);

			pPlayerSetAnimation = FindFunction(		"\x83\xEC\x44\x53\x55\x8B\xE9\x33\xDB\x56\x57",
													"xxxxxxxxxxx", 11);

			pPrecacheOtherWeapon = FindFunction(	"\x8B\x00\x00\x00\x00\x00\x8B\x00\x00\x00\x83"
													"\x00\x00\x53\x56\x2B\x00\x00\x00\x00\x00\x50",
													"x?????x???x??xxx?????x", 22);
		}
#endif 
	}

	cvar_t	version = {"hl_wpnmod_version", Plugin_info.version, FCVAR_SERVER};
	
	REG_SVR_COMMAND("wpnmod", WpnModCommand);
	CVAR_REGISTER (&version);

	return 1;
}



void OnAmxxAttach()
{
	if (!g_IsBaseSet)
	{
#ifdef __linux__
		print_srvconsole("[WEAPONMOD] Failed to locate hl_i386.so, cannot register natives.\n");
#elif _WIN32
		print_srvconsole("[WEAPONMOD] Failed to locate hl.dll, cannot register natives.\n");
#endif
	}
	else if (!pRadiusDamage)
	{
		print_srvconsole("[WEAPONMOD] Failed to find \"RadiusDamage\" function, cannot register natives.\n");
	}
	else if (!pGetAmmoIndex)
	{
		print_srvconsole("[WEAPONMOD] Failed to find \"GetAmmoIndex\" function, cannot register natives.\n");
	}
	else if (!pPlayerSetAnimation)
	{
		print_srvconsole("[WEAPONMOD] Failed to find \"PlayerSetAnimation\" function, cannot register natives.\n");
	}
	else if (!pPrecacheOtherWeapon)
	{
		print_srvconsole("[WEAPONMOD] Failed to find \"PrecacheOtherWeapon\" function, cannot register natives.\n");
	}
	else
	{
		MF_AddNatives(Natives_Ammo);
		MF_AddNatives(Natives_Weapon);
#ifdef __linux__
		print_srvconsole("[WEAPONMOD] Found hl_i386.so at %p\n", hldll_base);
#elif _WIN32
		print_srvconsole("[WEAPONMOD] Found hl.dll at %p\n", hldll_base);
#endif
	}
}



void OnAmxxDetach()
{
	g_EntData.clear();
	g_VirtHook_Crowbar.clear();
	g_VirtHook_InfoTarget.clear();
}



int AmxxCheckGame(const char *game)
{
	if (!strcasecmp(game, "valve"))
	{
#ifdef _WIN32
		int extraoffset = 0;
#elif __linux__
		int extraoffset = 2;
#endif
		VirtualFunction[VirtFunc_Classify] =			8 + extraoffset;
		VirtualFunction[VirtFunc_TakeDamage] =			11 + extraoffset;
		VirtualFunction[VirtFunc_BloodColor] =			14 + extraoffset;
		VirtualFunction[VirtFunc_TraceBleed] =			15 + extraoffset;
		VirtualFunction[VirtFunc_Think] =				43 + extraoffset;
		VirtualFunction[VirtFunc_Touch] =				44 + extraoffset;
		VirtualFunction[VirtFunc_Respawn] =				47 + extraoffset;
		VirtualFunction[VirtFunc_AddToPlayer] =			58 + extraoffset;
		VirtualFunction[VirtFunc_GetItemInfo] =			60 + extraoffset;
		VirtualFunction[VirtFunc_CanDeploy] =			61 + extraoffset;
		VirtualFunction[VirtFunc_Deploy] =				62 + extraoffset;
		VirtualFunction[VirtFunc_CanHolster] =			63 + extraoffset;
		VirtualFunction[VirtFunc_Holster] =				64 + extraoffset;
		VirtualFunction[VirtFunc_ItemPostFrame] =		67 + extraoffset;
		VirtualFunction[VirtFunc_Drop] =				68 + extraoffset;
		VirtualFunction[VirtFunc_ItemSlot] =			75 + extraoffset;
		VirtualFunction[VirtFunc_IsUseable] =			82 + extraoffset;

		return AMXX_GAME_OK;
	}
	
	return AMXX_GAME_BAD;
}



void OnPluginsLoaded()
{
	g_sModelIndexBloodSpray = PRECACHE_MODEL("sprites/bloodspray.spr"); // initial blood
	g_sModelIndexBloodDrop = PRECACHE_MODEL("sprites/blood.spr"); // splattered blood
}



int DispatchSpawn(edict_t *pEntity)
{
	if (!g_Initialized)
	{
		g_Initialized = TRUE;
		g_iWeaponIndex = LIMITER_WEAPON;
		g_iAmmoBoxIndex = 0;

		memset(WeaponInfoArray, 0, sizeof(WeaponInfoArray));
		memset(AmmoBoxInfoArray, 0, sizeof(AmmoBoxInfoArray));
	}

	RETURN_META_VALUE(MRES_IGNORED, 0);
}



void ClientCommand(edict_t *pEntity)
{
	const char* cmd = CMD_ARGV(0);

	if (cmd && stricmp(cmd, "wpnmod") == 0)
	{
		// Print version
		static char buf[1024];
		size_t len = 0;
			
		sprintf(buf, "\n%s %s\n", Plugin_info.name, Plugin_info.version);
		CLIENT_PRINT(pEntity, print_console, buf);
		len = sprintf(buf, "Author: \n         KORD_12.7 (AGHL.RU Dev Team)\n");
		len += sprintf(&buf[len], "Credits: \n         6a6kin, GordonFreeman, Lev, noo00oob\n");
		len += sprintf(&buf[len], "Compiled: %s\nURL: http://www.aghl.ru/ - Russian Half-Life and Adrenaline Gamer Community.\n\n", __DATE__ ", " __TIME__);
		CLIENT_PRINT(pEntity, print_console, buf);

		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}



void CmdStart(const edict_t *pPlayer, const struct usercmd_s *cmd, unsigned int random_seed)
{
	if (cmd->impulse == 101 && CVAR_GET_FLOAT("sv_cheats"))
	{
		edict_t *pItem = NULL;

		// Give weapons;
		for (int i = LIMITER_WEAPON + 1; i <= g_iWeaponIndex; i++)
		{
			pItem = Weapon_Spawn(i, pPlayer->v.origin, Vector (0, 0, 0));

			if (IsValidPev(pItem))
			{
				pItem->v.spawnflags |= SF_NORESPAWN;
				MDLL_Touch(pItem, (edict_t *)pPlayer);

				if (pItem->v.modelindex)
				{
					REMOVE_ENTITY(pItem);
				}
			}
		}

		// Give ammo;
		for (int i = 0; i < g_iAmmoBoxIndex; i++)
		{
			pItem = Ammo_Spawn(i, pPlayer->v.origin, Vector (0, 0, 0));

			if (IsValidPev(pItem))
			{
				pItem->v.spawnflags |= SF_NORESPAWN;
				MDLL_Touch(pItem, (edict_t *)pPlayer);

				if (pItem->v.modelindex)
				{
					REMOVE_ENTITY(pItem);
				}
			}
		}
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

	MF_BuildPathnameR(filepath, sizeof(filepath) - 1, "%s/weaponmod/%s.ini", get_localinfo("amxx_configsdir","addons/amxmodx/configs"), STRING(gpGlobals->mapname));
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
				
				for (i = LIMITER_WEAPON + 1; i <= g_iWeaponIndex; i++)
				{
					if (!_stricmp(pszName(i), szData[0]))
					{
						Weapon_Spawn(i, strlen(szData[1]) ? ParseVec(szData[1]) : Vector(0, 0, 0), strlen(szData[2])  ? ParseVec(szData[2]) : Vector(0, 0, 0));
						wpns++;
					}
				}

				for (i = 0; i < g_iAmmoBoxIndex; i++)
				{
					if (!_stricmp(AmmoBoxInfoArray[i].pszName, szData[0]))
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



void ServerDeactivate()
{
	if (g_Initialized)
	{
		g_Initialized = FALSE;
	}

	RETURN_META(MRES_IGNORED);
}
