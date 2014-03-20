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
#include "wpnmod_utils.h"

char g_ConfigFilepath[1024];

edict_t* g_EquipEnt			= NULL;

cvar_t *cvar_sv_cheats		= NULL;
cvar_t *cvar_mp_weaponstay	= NULL;

CVector <DecalList*>		g_Decals;
CVector <StartAmmo*>		g_StartAmmo;
CVector <VirtualHookData*>	g_BlockedItems;

int g_iWeaponsCount			= 0;
int g_iWeaponInitID			= 0;
int g_iAmmoBoxIndex			= 0;

int g_iMaxWeaponSlots		= 5;
int g_iMaxWeaponPositions	= 5;

bool g_bCrowbarHooked	= false;
bool g_bAmmoBoxHooked	= false;

SUBMOD g_GameMod;

WeaponData	WeaponInfoArray	[MAX_WEAPONS];
AmmoBoxData AmmoBoxInfoArray[MAX_WEAPONS];

int** g_pCurrentSlots = NULL;

SUBMOD CheckSubMod(const char* game)
{
	if (!stricmp(game, "ag"))
	{
		WPNMOD_LOG("Adrenaline Gamer detected.\n");
		return SUBMOD_AG;
	}
	else if (!stricmp(game, "gearbox"))
	{
		WPNMOD_LOG("Opposing Force detected.\n");
		return SUBMOD_GEARBOX;
	}
	else if (!stricmp(game, "valve"))
	{
		if (CVAR_GET_POINTER("sv_ag_version"))
		{
			WPNMOD_LOG("Adrenaline Gamer Mini detected.\n");
			return SUBMOD_MINIAG;
		}
		else if (CVAR_GET_POINTER("aghl.ru"))
		{
			WPNMOD_LOG("Bugfixed and improved HL release detected.\n");
			return SUBMOD_AGHLRU;
		}

		WPNMOD_LOG("Half-Life assumed.\n");
		return SUBMOD_VALVE;
	}

	WPNMOD_LOG("Unknown mod detected (\"%s\").\n", MF_GetModname());
	return SUBMOD_UNKNOWN;
}

void SetConfigFile()
{
	build_pathname_r(g_ConfigFilepath, sizeof(g_ConfigFilepath) - 1, "%s/weaponmod/weaponmod-%s.ini", LOCALINFO((char*)"amxx_configsdir"), STRING(gpGlobals->mapname));
	
	if (!FileExists(g_ConfigFilepath))
	{
		build_pathname_r(g_ConfigFilepath, sizeof(g_ConfigFilepath) - 1, "%s/weaponmod/weaponmod.ini", LOCALINFO((char*)"amxx_configsdir"));
	}
}

void AutoSlotDetection(int iWeaponID, int iSlot, int iPosition)
{
	if (iSlot >= g_iMaxWeaponSlots || iSlot < 0)
	{
		iSlot = g_iMaxWeaponSlots - 1;
	}

	if (iPosition >= g_iMaxWeaponPositions || iPosition < 0)
	{
		iPosition = g_iMaxWeaponPositions - 1;
	}

	if (!g_pCurrentSlots[iSlot][iPosition])
	{
		g_pCurrentSlots[iSlot][iPosition] = iWeaponID;

		WeaponInfoArray[iWeaponID].ItemData.iSlot = iSlot;
		WeaponInfoArray[iWeaponID].ItemData.iPosition = iPosition;
	}
	else
	{
		bool bFound = false;

		for (int k, i = 0; i < g_iMaxWeaponSlots && !bFound; i++)
		{
			for (k = 0; k < g_iMaxWeaponPositions; k++)
			{
				if (!g_pCurrentSlots[i][k])
				{
					g_pCurrentSlots[i][k] = iWeaponID;

					WeaponInfoArray[iWeaponID].ItemData.iSlot = i;
					WeaponInfoArray[iWeaponID].ItemData.iPosition = k;

					WPNMOD_LOG("Warning: \"%s\" is moved to slot %d-%d.\n", GetWeapon_pszName(iWeaponID), i + 1, k + 1);

					bFound = true;
					break;
				}
			}
		}

		if (!bFound)
		{
			WeaponInfoArray[iWeaponID].ItemData.iPosition = MAX_WEAPONS;
			WPNMOD_LOG("Warning: No free slot for \"%s\" in HUD!\n", GetWeapon_pszName(iWeaponID));
		}
	}
}

edict_t* Weapon_Spawn(const char* szName, Vector vecOrigin, Vector vecAngles)
{
	if (!szName)
	{
		return NULL;
	}

	int iId = 0;
	
	for (int i = 1; i <= g_iWeaponsCount; i++)
	{
		if (WeaponInfoArray[i].iType == Wpn_Custom && !_stricmp(GetWeapon_pszName(i), szName))
		{
			iId = i;
			break;
		}
	}

	if (!iId)
	{
		return NULL;
	}

	edict_t* pItem = CREATE_NAMED_ENTITY(MAKE_STRING("weapon_crowbar"));

	if (IsValidPev(pItem))
	{
		MDLL_Spawn(pItem);
		SET_ORIGIN(pItem, vecOrigin);

		pItem->v.classname = MAKE_STRING(GetWeapon_pszName(iId));
		pItem->v.angles = vecAngles;

		SetPrivateInt(pItem, pvData_iId, iId);

		if (GetWeapon_MaxClip(iId) != -1)
		{
			SetPrivateInt(pItem, pvData_iClip, 0);
		}

		if (WeaponInfoArray[iId].iForward[Fwd_Wpn_Spawn])
		{
			MF_ExecuteForward
			(
				WeaponInfoArray[iId].iForward[Fwd_Wpn_Spawn],

				static_cast<cell>(ENTINDEX(pItem)), 
				static_cast<cell>(0), 
				static_cast<cell>(0), 
				static_cast<cell>(0),
				static_cast<cell>(0)
			);
		}	

		return pItem;
	}

	return NULL;
}

edict_t* Ammo_Spawn(const char* szName, Vector vecOrigin, Vector vecAngles)
{
	if (!szName)
	{
		return NULL;
	}

	int iId = 0;
	
	for (int i = 1; i <= g_iAmmoBoxIndex; i++)
	{
		if (!_stricmp(AmmoBoxInfoArray[i].classname.c_str(), szName))
		{
			iId = i;
			break;
		}
	}

	if (!iId)
	{
		return NULL;
	}

	edict_t* pAmmoBox = CREATE_NAMED_ENTITY(MAKE_STRING("ammo_rpgclip"));

	if (IsValidPev(pAmmoBox))
	{
		MDLL_Spawn(pAmmoBox);
		SET_ORIGIN(pAmmoBox, vecOrigin);

		pAmmoBox->v.classname = MAKE_STRING(AmmoBoxInfoArray[iId].classname.c_str());
		pAmmoBox->v.angles = vecAngles;

		if (AmmoBoxInfoArray[iId].iForward[Fwd_Ammo_Spawn])
		{
			MF_ExecuteForward
			(
				AmmoBoxInfoArray[iId].iForward[Fwd_Ammo_Spawn],
				static_cast<cell>(ENTINDEX(pAmmoBox)),
				static_cast<cell>(0)
			);
		}

		SET_SIZE(pAmmoBox, Vector(-16, -16, 0), Vector(16, 16, 16));
		return pAmmoBox;
	}

	return NULL;
}

