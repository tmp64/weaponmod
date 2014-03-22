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




#include "wpnmod_hooks.h"
#include "wpnmod_parse.h"


CConfig g_Config;

CConfig::CConfig()
{
	m_bInited = false;
	m_bWorldSpawned = false;

	m_pCurrentSlots = NULL;
	m_iMaxWeaponSlots = 5;
	m_iMaxWeaponPositions = 5;

	m_bWpnBoxModels = false;
	m_iWpnBoxLifeTime = 120;
	m_iWpnBoxRenderColor = NULL;

	m_pEquipEnt = NULL;
};

void CConfig::InitGameMod(void)
{
	if (!m_bInited)
	{
		g_GameMod = g_Config.CheckSubMod(MF_GetModname());

		pvData_Init();
		Vtable_Init();

		if (g_GameMod == SUBMOD_GEARBOX)
		{
			// More slots in OP4.
			m_iMaxWeaponSlots = 7;
		}

		g_Ents = new EntData[gpGlobals->maxEntities];
		m_pCurrentSlots = new int* [m_iMaxWeaponSlots];

		for (int i = 0; i < m_iMaxWeaponSlots; ++i)
		{
			memset((m_pCurrentSlots[i] = new int [m_iMaxWeaponPositions]), 0, sizeof(int) * m_iMaxWeaponPositions);
		}

		m_bInited = true;
	}
}

void CConfig::WorldPrecache(void)
{
	if (m_bWorldSpawned)
	{
		return;
	}

	SetConfigFile();

	cvar_sv_cheats = CVAR_GET_POINTER("sv_cheats");
	cvar_mp_weaponstay = CVAR_GET_POINTER("mp_weaponstay");

	WPNMOD_LOG_ONLY("-------- Mapchange to %s --------\n", STRING(gpGlobals->mapname));

	if (ParseSection(GetConfigFile(), "[block]", (void*)OnParseBlockedItems, -1) && (int)g_BlockedItems.size())
	{
		WPNMOD_LOG("Blocked default items:\n");

		for (int i = 0; i < (int)g_BlockedItems.size(); i++)
		{
			WPNMOD_LOG(" \"%s\"\n", g_BlockedItems[i]->classname);
		}
	}

	m_bWorldSpawned = true;
}

void CConfig::ServerActivate(void)
{
	ParseBSP();
	ParseSpawnPoints();

	// Parse default equipments and ammo.
	ParseSection(GetConfigFile(), "[ammo]", (void*)OnParseStartAmmos, ':');
	ParseSection(GetConfigFile(), "[equipment]", (void*)OnParseStartEquipments	, ':');

	// Remove blocked items from map.
	for (int i = 0; i < (int)g_BlockedItems.size(); i++)
	{
		edict_t *pFind = FIND_ENTITY_BY_CLASSNAME(NULL, g_BlockedItems[i]->classname);

		while (!FNullEnt(pFind))
		{
			UTIL_RemoveEntity(pFind);
			pFind = FIND_ENTITY_BY_CLASSNAME(pFind, g_BlockedItems[i]->classname);
		}
	}

	g_Memory.EnableShieldHitboxTracing();

	if (ParseSection(GetConfigFile(), "[weaponbox]", (void*)OnParseWeaponbox, ':'))
	{
		g_Memory.EnableWeaponboxModels();
	}

	SetHookVirtual(&g_PlayerSpawn_Hook);
	SetHookVirtual(&g_PlayerPostThink_Hook);
}

void CConfig::ServerDeactivate(void)
{
	m_bWorldSpawned = false;

	for (int i = 0; i < m_iMaxWeaponSlots; ++i)
	{
		memset(m_pCurrentSlots[i], 0, sizeof(int) * m_iMaxWeaponPositions);
	}

	for (int i = 0; i < CrowbarHook_End; i++)
	{
		UnsetHookVirtual(&g_CrowbarHooks[i]);
	}

	if (g_fh_funcPackWeapon.done)
	{
		UnsetHook(&g_fh_funcPackWeapon);
	}

	m_pEquipEnt = NULL;
	g_fh_funcPackWeapon.address = NULL;

	UnsetHookVirtual(&g_RpgAddAmmo_Hook);
	UnsetHookVirtual(&g_PlayerSpawn_Hook);
	UnsetHookVirtual(&g_PlayerPostThink_Hook);

	m_iWpnBoxLifeTime = 120;
}

void CConfig::ServerShutDown(void)
{
	for (int i = 0; i < m_iMaxWeaponSlots; ++i)
	{
		delete [] m_pCurrentSlots[i];
	}
	
	delete [] g_Ents;
	delete [] m_pCurrentSlots;
}

void CConfig::SetConfigFile(void)
{
	build_pathname_r(m_cfgpath, sizeof(m_cfgpath) - 1, "%s/weaponmod/weaponmod-%s.ini", LOCALINFO((char*)"amxx_configsdir"), STRING(gpGlobals->mapname));
	
	if (!FileExists(m_cfgpath))
	{
		build_pathname_r(m_cfgpath, sizeof(m_cfgpath) - 1, "%s/weaponmod/weaponmod.ini", LOCALINFO((char*)"amxx_configsdir"));
	}
}

SUBMOD CConfig::CheckSubMod(const char* game)
{
	if (!stricmp(game, "ag"))
	{
		WPNMOD_LOG_ONLY("Adrenaline Gamer detected.\n");
		return SUBMOD_AG;
	}
	else if (!stricmp(game, "gearbox"))
	{
		WPNMOD_LOG_ONLY("Opposing Force detected.\n");
		return SUBMOD_GEARBOX;
	}
	else if (!stricmp(game, "valve"))
	{
		if (CVAR_GET_POINTER("sv_ag_version"))
		{
			WPNMOD_LOG_ONLY("Adrenaline Gamer Mini detected.\n");
			return SUBMOD_MINIAG;
		}
		else if (CVAR_GET_POINTER("aghl.ru"))
		{
			WPNMOD_LOG_ONLY("Bugfixed and improved HL release detected.\n");
			return SUBMOD_AGHLRU;
		}

		WPNMOD_LOG_ONLY("Half-Life assumed.\n");
		return SUBMOD_VALVE;
	}

	WPNMOD_LOG("Warning: game mod unrecognized (\"%s\").\n", MF_GetModname());
	return SUBMOD_UNKNOWN;
}

void CConfig::AutoSlotDetection(int iWeaponID, int iSlot, int iPosition)
{
	if (iSlot >= m_iMaxWeaponSlots || iSlot < 0)
	{
		iSlot = m_iMaxWeaponSlots - 1;
	}

	if (iPosition >= m_iMaxWeaponPositions || iPosition < 0)
	{
		iPosition = m_iMaxWeaponPositions - 1;
	}

	if (!m_pCurrentSlots[iSlot][iPosition])
	{
		m_pCurrentSlots[iSlot][iPosition] = iWeaponID;

		WeaponInfoArray[iWeaponID].ItemData.iSlot = iSlot;
		WeaponInfoArray[iWeaponID].ItemData.iPosition = iPosition;
	}
	else
	{
		bool bFound = false;

		for (int k, i = 0; i < m_iMaxWeaponSlots && !bFound; i++)
		{
			for (k = 0; k < m_iMaxWeaponPositions; k++)
			{
				if (!m_pCurrentSlots[i][k])
				{
					m_pCurrentSlots[i][k] = iWeaponID;

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

void CConfig::ManageEquipment(void)
{
	const char* equip_classname = "game_player_equip";
	edict_t* pFind = FIND_ENTITY_BY_CLASSNAME(NULL, equip_classname);

	while (!FNullEnt(pFind))
	{
		UTIL_RemoveEntity(pFind);
		pFind = FIND_ENTITY_BY_CLASSNAME(pFind, equip_classname);
	}

	pFind = CREATE_NAMED_ENTITY(MAKE_STRING(equip_classname));

	if (IsValidPev(pFind))
	{
		MDLL_Spawn(pFind);

		pFind = CREATE_NAMED_ENTITY(MAKE_STRING(equip_classname));

		if (IsValidPev(pFind))
		{
			m_pEquipEnt = pFind;
			m_pEquipEnt->v.classname = MAKE_STRING("weaponmod_equipment");
				
			MDLL_Spawn(m_pEquipEnt);
		}
	}
}








cvar_t* cvar_sv_cheats		= NULL;
cvar_t* cvar_mp_weaponstay	= NULL;

CVector <DecalList*>		g_Decals;
CVector <StartAmmo*>		g_StartAmmo;
CVector <VirtualHookData*>	g_BlockedItems;

int g_iWeaponsCount			= 0;
int g_iWeaponInitID			= 0;
int g_iAmmoBoxIndex			= 0;

bool g_bCrowbarHooked	= false;
bool g_bAmmoBoxHooked	= false;

SUBMOD g_GameMod;

WeaponData	WeaponInfoArray	[MAX_WEAPONS];
AmmoBoxData AmmoBoxInfoArray[MAX_WEAPONS];






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

