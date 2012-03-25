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

int OnMetaAttach()
{
	REG_SVR_COMMAND("wpnmod", WpnModCommand);

	/*print_srvconsole("\n   Half-Life Weapon Mod version %s Copyright (c) 2012 AGHL.RU Dev Team. \n"
					 "   Weapon Mod comes with ABSOLUTELY NO WARRANTY; for details type `wpnmod gpl'.\n", Plugin_info.version);
	print_srvconsole("   This is free software and you are welcome to redistribute it under \n"
					 "   certain conditions; type 'wpnmod gpl' for details.\n  \n");
	*/
	if (FindDllBase((void*)MDLL_FUNC->pfnGetGameDescription()))
	{
		if (CVAR_GET_POINTER("aghl.ru"))
		{
#ifdef _WIN32
		// TO DO: Find patterns.
#elif __linux__
		// TO DO: Find linux patterns.
#endif
		}
		else
		{
#ifdef _WIN32
			pRadiusDamage = FindFunction(	"\xD9\x44\x24\x1C\xD8\x00\x00\x00\x00\x00\x83\xEC\x64",
											"xxxxx?????xxx", 13);

			pPlayerSetAnimation = FindFunction(		"\x83\xEC\x44\x53\x55\x8B\xE9\x33\xDB\x56\x57",
													"xxxxxxxxxxx", 11);

			pPrecacheOtherWeapon = FindFunction(	"\x8B\x00\x00\x00\x00\x00\x8B\x00\x00\x00\x83"
													"\x00\x00\x53\x56\x2B\x00\x00\x00\x00\x00\x50",
													"x?????x???x??xxx?????x", 22);
		}
#elif __linux__
		// TO DO: Find linux patterns. :D
#endif
	}

	return 1;
}



void OnAmxxAttach()
{
	if (!g_IsBaseSet || !pRadiusDamage || !pPlayerSetAnimation || ! pPrecacheOtherWeapon)
	{
		print_srvconsole("[WEAPONMOD] Failed to locate hl.dll, cannot register natives.\n");
	}
	else
	{
		MF_AddNatives(Natives_Ammo);
		MF_AddNatives(Natives_Weapon);

		print_srvconsole("[WEAPONMOD] Found hl.dll at %p\n", hldll_base);
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
	g_sModelIndexBloodSpray = PRECACHE_MODEL ("sprites/bloodspray.spr"); // initial blood
	g_sModelIndexBloodDrop = PRECACHE_MODEL ("sprites/blood.spr"); // splattered blood 
}



void ServerDeactivate()
{
	if (g_Initialized)
	{
		g_Initialized = FALSE;
	}

	RETURN_META(MRES_IGNORED);
}



int FN_DispatchSpawn(edict_t *pEntity)
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



void FN_ClientCommand(edict_t *pEntity)
{
	const char* cmd = CMD_ARGV(0);

	if (cmd && stricmp(cmd, "wpnmod") == 0)
	{
		// Print version
		static char buf[1024];
		size_t len = 0;
			
		sprintf(buf, "\n%s %s\n", Plugin_info.name, Plugin_info.version);
		CLIENT_PRINT(pEntity, print_console, buf);
		len = sprintf(buf, "Author: \n         KORD_12.7\n");
		len += sprintf(&buf[len], "Credits: \n         Lev\n");
		len += sprintf(&buf[len], "Compiled: %s\nURL: http://www.aghl.ru/ - Russian Half-Life and Adrenaline Gamer Community.\n\n", __DATE__ ", " __TIME__);
		CLIENT_PRINT(pEntity, print_console, buf);

		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}