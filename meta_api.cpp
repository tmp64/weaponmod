/*
 * Half-Life Weapon Mod
 * Copyright (c) 2012 AGHL.RU Dev Team
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





int OnMetaAttach()
{
	pDbase = GetModuleHandleA("hl.dll");

	if (pDbase == NULL) 
	{
		print_srvconsole("[WEAPONMOD] Failed to locate hl.dll\n");
        return 0;
	}

	REG_SVR_COMMAND("wpnmod", WpnModCommand);

	print_srvconsole("[WEAPONMOD] Found hl.dll at %p\n", pDbase);
	return 1;
}

void OnAmxxAttach()
{
	MF_AddNatives(wpnmod_Natives);
}

void OnAmxxDetach()
{
	g_EntData.clear();
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
		VirtualFunction[VirtFunc_AddToPlayer] =			58 + extraoffset;
		VirtualFunction[VirtFunc_GetItemInfo] =			60 + extraoffset;
		VirtualFunction[VirtFunc_CanDeploy] =			61 + extraoffset;
		VirtualFunction[VirtFunc_Deploy] =				62 + extraoffset;
		VirtualFunction[VirtFunc_CanHolster] =			63 + extraoffset;
		VirtualFunction[VirtFunc_Holster] =				64 + extraoffset;
		VirtualFunction[VirtFunc_ItemPostFrame] =		67 + extraoffset;
		VirtualFunction[VirtFunc_Drop] =				68 + extraoffset;
		VirtualFunction[VirtFunc_ItemSlot] =			75 + extraoffset;

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
	if (g_initialized)
	{
		g_initialized = FALSE;
	}

	RETURN_META(MRES_IGNORED);
}

int FN_DispatchSpawn(edict_t *pent)
{
	if (!g_initialized)
	{
		g_initialized = true;
		g_iWeaponIndex = LIMITER_WEAPON;

		memset(WeaponInfoArray, 0, sizeof(WeaponInfoArray));
	}

	RETURN_META_VALUE(MRES_IGNORED, 0);
}