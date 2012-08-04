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
#include "libFunc.h"
#include "CVirtHook.h"


BOOL g_Initialized;

extern CVirtHook g_VirtHook_Crowbar;



dllFunc g_dllFuncs[Func_End] =
{
	{
		NULL,
		"RadiusDamage",
		"RadiusDamage__FG6VectorP9entvars_sT1ffii",
		{
			"\x83\x00\x00\xD9\x00\xD9\x00\x00\x00\xD9\x00\x00\x00\xD9\x00"
			"\x00\x00\xD9\x00\x00\x00\xD9\x00\x00\x00\xD9\x00\x00\x00",
			"x??x?x???x???x???x???x???x???", 29,
		},
		{
			"\xD9\x44\x24\x1C\xD8\x00\x00\x00\x00\x00\x83\xEC\x64", 
			"xxxxx?????xxx", 13
		}
	},
	{
		NULL,
		"CBasePlayer::GetAmmoIndex",
		"GetAmmoIndex__11CBasePlayerPCc",
		{
			"\x57\x8B\x7C\x24\x08\x85\xFF\x75\x05", 
			"xxxxxxxxx", 9,
		},
		{
			"\x56\x57\x8B\x7C\x24\x0C\x85\xFF", 
			"xxxxxxxx", 8
		}
	},
	{
		NULL,
		"ClearMultiDamage",
		"ClearMultiDamage__Fv",
		{
			"\xD9\xEE\x33\xC0\xD9\x00\x00\x00\x00\x00\xA3\x00\x00\x00\x00", 
			"xxxxx?????x????", 
			15,
		},
		{
			"\x8B\x00\x00\x00\x00\x00\x8B\x00\x00\x00\x2B"
			"\x00\x00\x00\x00\x00\x83\x00\x00\x53\x50",
			"x?????x???x?????x??xx", 21
		}
	},
	{
		NULL,
		"ApplyMultiDamage",
		"ApplyMultiDamage__FP9entvars_sT0",
		{
			"\x8B\x0D\x00\x00\x00\x00\x85\xC9\x74\x22", 
			"xx????xxxx", 10,
		},
		{
			"\x57\x8B\x7C\x24\x08\x85\xFF\x75\x05",
			"xxxxxxxxx", 9
		}
	},
	{
		NULL,
		"CBasePlayer::SetAnimation",
		"SetAnimation__11CBasePlayer11PLAYER_ANIM",
		{
			"\x83\xEC\x00\xA1\x00\x00\x00\x00\x33"
			"\xC4\x89\x00\x00\x00\x53\x56\x8B\xD9",
			"xx?x????xxx???xxxx", 18,
		},
		{
			"\x83\xEC\x44\x53\x55\x8B\xE9\x33\xDB\x56\x57", 
			"xxxxxxxxxxx", 11
		}
	},
	{
		NULL,
		"UTIL_PrecacheOtherWeapon",
		"UTIL_PrecacheOtherWeapon__FPCc",
		{
			"\x8B\x00\x00\x00\x00\x00\x8B\x00\x00\x00\x2B"
			"\x00\x00\x00\x00\x00\x83\x00\x00\x53\x50",
			"x?????x???x?????x??xx", 21,
		},
		{
			"\x8B\x00\x00\x00\x00\x00\x8B\x00\x00\x00\x83"
			"\x00\x00\x53\x56\x2B\x00\x00\x00\x00\x00\x50",
			"x?????x???x??xxx?????x", 22
		}
	},
	{
		NULL,
		"GiveNamedItem",
		"",
		{
			"\x8B\x44\x00\x00\x56\x57\x8B\xF9\x8B\x0D\x00\x00\x00\x00",
			"xx??xxxxxx????", 14,
		},
		{
			""
			"",
			"", 0
		}
	}
};


function dll_GiveNamedItem;


#ifdef _WIN32
void __fastcall GiveNamedItem_HookHandler(void *pPrivate, int i, const char *pszName)
#elif __linux__
void GiveNamedItem_HookHandler(void *pPrivate, const char *pszName)
#endif
{
	UnsetHook(&dll_GiveNamedItem);
#ifdef _WIN32
	reinterpret_cast<int (__fastcall *)(void *, int, const char *)>( dll_GiveNamedItem.address)(pPrivate, i, pszName);
#elif __linux__
	reinterpret_cast<int (*)(void *, const char *)>( dll_GiveNamedItem.address)(pPrivate, pszName);
#endif
	SetHook(&dll_GiveNamedItem);
}


int OnMetaAttach()
{
	g_Ents = new EntData[gpGlobals->maxEntities];

	if (FindModuleByAddr((void*)MDLL_FUNC->pfnGetGameDescription(), &hl_dll))
	{
#ifdef __linux__
		void* handle = dlopen(GET_GAME_INFO(PLID, GINFO_DLL_FULLPATH), RTLD_NOW);

		if (handle != NULL) 
		{
			for (int i = 0; i < Func_End; i++)
			{
				g_dllFuncs[i].pAddress = dlsym(handle, g_dllFuncs[i].linuxName);
			}

			dlclose(handle);
		}
#elif _WIN32
		if (CVAR_GET_POINTER("aghl.ru"))
		{
			for (int i = 0; i < Func_End; i++)
			{
				g_dllFuncs[i].pAddress = FindFunction(&hl_dll, g_dllFuncs[i].sigAGHLru);
			}
		}
		else
		{
			for (int i = 0; i < Func_End; i++)
			{
				g_dllFuncs[i].pAddress = FindFunction(&hl_dll, g_dllFuncs[i].sigStandart);
			}
		}
#endif
		if (CreateFunctionHook(&dll_GiveNamedItem, g_dllFuncs[Func_GiveNamedItem].pAddress, (void*)GiveNamedItem_HookHandler))
		{
			SetHook(&dll_GiveNamedItem);
		}
	}

	cvar_t	version = {"hl_wpnmod_version", Plugin_info.version, FCVAR_SERVER};
	
	REG_SVR_COMMAND("wpnmod", WpnModCommand);
	CVAR_REGISTER (&version);

	return 1;
}



void OnAmxxAttach()
{
	BOOL bAddNatives = TRUE; 

	if (!hl_dll.base)
	{
		print_srvconsole("[WEAPONMOD] Failed to locate %s\n", GET_GAME_INFO(PLID, GINFO_DLL_FILENAME));
		bAddNatives = FALSE;
	}

	for (int i = 0; i < Func_End; i++)
	{
		if (!g_dllFuncs[i].pAddress)
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
}



void OnAmxxDetach()
{
	if (dll_GiveNamedItem.done)
	{
		UnsetHook(&dll_GiveNamedItem);
	}

	delete [] g_Ents;
	g_VirtHook_Crowbar.clear();
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
		VirtualFunction[VirtFunc_TraceAttack] =			10 + extraoffset;
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

	if (cmd && _stricmp(cmd, "wpnmod") == 0)
	{
		int i = 0;
		int items = 0;
		int weapons = 0;
		int ammo = 0;

		static char buf[1024];
		size_t len = 0;
			
		sprintf(buf, "\n%s %s\n", Plugin_info.name, Plugin_info.version);
		CLIENT_PRINT(pEntity, print_console, buf);
		len = sprintf(buf, "Author: \n         KORD_12.7 (AGHL.RU Dev Team)\n");
		len += sprintf(&buf[len], "Credits: \n         6a6kin, GordonFreeman, Lev, noo00oob\n");
		len += sprintf(&buf[len], "Compiled: %s\nURL: http://www.aghl.ru/ - Russian Half-Life and Adrenaline Gamer Community.\n\n", __DATE__ ", " __TIME__);
		CLIENT_PRINT(pEntity, print_console, buf);

		CLIENT_PRINT(pEntity, print_console, "Currently loaded weapons:\n");

		for (i = LIMITER_WEAPON + 1; i <= g_iWeaponIndex; i++)
		{
			items++;
			sprintf(buf, " [%2d] %-23.22s\n", ++weapons, pszName(i));
			CLIENT_PRINT(pEntity, print_console, buf);
		}

		CLIENT_PRINT(pEntity, print_console, "\nCurrently loaded ammo:\n");

		for (i = 0; i < g_iAmmoBoxIndex; i++)
		{
			items++;
			sprintf(buf, " [%2d] %-23.22s\n", ++ammo, AmmoBoxInfoArray[i].pszName);
			CLIENT_PRINT(pEntity, print_console, buf);
		}

		CLIENT_PRINT(pEntity, print_console, "\nTotal:\n");
		sprintf(buf, "%4d items (%d weapons, %d ammo).\n\n", items, weapons, ammo);
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
