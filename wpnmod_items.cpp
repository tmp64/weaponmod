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

#include "wpnmod_items.h"
#include "wpnmod_hooks.h"
#include "wpnmod_utils.h"
#include "wpnmod_pvdata.h"


CItems g_Items;

const char* gWeaponReference = "weapon_crowbar";
const char* gAmmoBoxReference = "ammo_rpgclip";

CItems::CItems()
{
	m_bWeaponRefHooked = false;
	m_bAmmoBoxRefHooked = false;

	m_pCurrentSlots = NULL;
	m_iMaxWeaponSlots = 5;
	m_iMaxWeaponPositions = 5;

	m_pItemInfoArray = NULL;
	m_pAmmoInfoArray = NULL;

	memset(m_WeaponsInfo, 0, sizeof(m_WeaponsInfo));
}

void CItems::AllocWeaponSlots(int slots, int positions)
{
	m_iMaxWeaponSlots = slots;
	m_iMaxWeaponPositions = positions;

	m_pCurrentSlots = new int*[m_iMaxWeaponSlots];

	for (int i = 0; i < m_iMaxWeaponSlots; ++i)
	{
		memset((m_pCurrentSlots[i] = new int[m_iMaxWeaponPositions]), 0, sizeof(int) * m_iMaxWeaponPositions);
	}
}

void CItems::FreeWeaponSlots(void)
{
	if (m_pCurrentSlots)
	{
		for (int i = 0; i < m_iMaxWeaponSlots; ++i)
		{
			delete[] m_pCurrentSlots[i];
		}

		delete[] m_pCurrentSlots;
	}
}

bool CItems::CheckSlots(int iWeaponID)
{
	int iSlot = Weapon_GetSlot(iWeaponID);
	int iPosition = Weapon_GetSlotPosition(iWeaponID);

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

		m_pItemInfoArray[iWeaponID].iSlot = iSlot;
		m_pItemInfoArray[iWeaponID].iPosition = iPosition;

		return true;
	}

	for (int k, i = 0; i < m_iMaxWeaponSlots; i++)
	{
		for (k = 0; k < m_iMaxWeaponPositions; k++)
		{
			if (!m_pCurrentSlots[i][k])
			{
				m_pCurrentSlots[i][k] = iWeaponID;

				m_pItemInfoArray[iWeaponID].iSlot = i;
				m_pItemInfoArray[iWeaponID].iPosition = k;

				WPNMOD_LOG("Warning: \"%s\" is moved to slot %d-%d.\n", WEAPON_GET_NAME(iWeaponID), i + 1, k + 1);
				return true;
			}
		}
	}

	m_pItemInfoArray[iWeaponID].iPosition = MAX_WEAPONS;
	WPNMOD_LOG("Warning: No free slot for \"%s\" in HUD!\n", WEAPON_GET_NAME(iWeaponID));
	return false;
}

void CItems::ServerDeactivate(void)
{
	memset(m_WeaponsInfo, 0, sizeof(m_WeaponsInfo));

	for (int i = 0; i < (int)m_AmmoBoxesInfo.size(); i++)
	{
		delete m_AmmoBoxesInfo[i];
	}

	m_AmmoBoxesInfo.clear();

	for (int i = 0; i < m_iMaxWeaponSlots; ++i)
	{
		memset(m_pCurrentSlots[i], 0, sizeof(int) * m_iMaxWeaponPositions);
	}
}

int CItems::Ammobox_Register(const char *name)
{
	CAmmoBoxInfo *p = new CAmmoBoxInfo;

	p->m_strClassname.assign(name);
	m_AmmoBoxesInfo.push_back(p);

	if (!m_bAmmoBoxRefHooked)
	{
		m_bAmmoBoxRefHooked = true;
		for (int k = 0; k < AmmoBoxRefHook_End; k++)
		{
			SetHookVirtual(&g_AmmoBoxRefHooks[k]);
		}
	}

	return m_AmmoBoxesInfo.size();
}

int CItems::Ammobox_RegisterForward(int iId, e_AmmoFwds fwdType, AMX *amx, const char *pFuncName)
{
	if (iId <= 0 || iId > Ammobox_GetCount())
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "Invalid ammobox id provided (%d).", iId);
		return 0;
	}

	if (fwdType < 0 || fwdType >= Fwd_Ammo_End)
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "Function out of bounds. Got: %d, Max: %d.", iId, fwdType - 1);
		return 0;
	}

	int iRegResult = MF_RegisterSPForwardByName(amx, pFuncName, FP_CELL, FP_CELL, FP_DONE);

	if (iRegResult == -1)
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "Function not found (%d, \"%s\").", fwdType, pFuncName);
		return 0;
	}

	m_AmmoBoxesInfo[--iId]->m_AmxxForwards[fwdType] = iRegResult;
	return iRegResult;
}

int CItems::Ammobox_ExecuteForward(int iId, e_AmmoFwds fwdType, edict_t* pAmmobox, edict_t* pPlayer)
{
	if (!m_AmmoBoxesInfo[--iId]->m_AmxxForwards[fwdType])
	{
		return 0;
	}

	int iPlayer = 0;

	if (IsValidPev(pPlayer))
	{
		iPlayer = ENTINDEX(pPlayer);
	}

	return MF_ExecuteForward
	(
		m_AmmoBoxesInfo[iId]->m_AmxxForwards[fwdType],

		static_cast<cell>(ENTINDEX(pAmmobox)),
		static_cast<cell>(iPlayer)
	);
}

int CItems::Ammobox_GetCount()
{
	return m_AmmoBoxesInfo.size();
}

const char*	CItems::Ammobox_GetName(int iId)
{
	if (iId <= 0 || iId > Ammobox_GetCount())
	{
		return NULL;
	}

	return m_AmmoBoxesInfo[--iId]->m_strClassname.c_str();
}

int CItems::Ammobox_GetId(const char *name)
{
	for (int i = 0; i < (int)m_AmmoBoxesInfo.size(); i++)
	{
		if (_stricmp(m_AmmoBoxesInfo[i]->m_strClassname.c_str(), name) == 0)
		{
			return i + 1;
		}
	}

	return 0;
}

bool CItems::AddAmmoNameToAmmoRegistry(const char *szAmmoname)
{
	int iAmmoIndex;

	for (iAmmoIndex = 1; iAmmoIndex < MAX_AMMO_SLOTS; iAmmoIndex++)
	{
		if (!m_pAmmoInfoArray[iAmmoIndex].pszName)
		{
			break;
		}

		if (_stricmp(m_pAmmoInfoArray[iAmmoIndex].pszName, szAmmoname) == 0)
		{
			return true;
		}
	}

	if (iAmmoIndex >= MAX_AMMO_SLOTS)
	{
		return false;
	}

	m_pAmmoInfoArray[iAmmoIndex].pszName = STRING(ALLOC_STRING(szAmmoname));
	return true;
}

int CItems::GetAmmoIndex(const char *psz)
{
	if (!psz)
	{
		return -1;
	}

	for (int iAmmoIndex = 1; iAmmoIndex < MAX_AMMO_SLOTS; iAmmoIndex++)
	{
		if (!m_pAmmoInfoArray[iAmmoIndex].pszName)
		{
			continue;
		}

		if (_stricmp(psz, m_pAmmoInfoArray[iAmmoIndex].pszName) == 0)
		{
			return iAmmoIndex;
		}
	}

	return -1;
}

int CItems::Weapon_RegisterWeapon(AMX *amx, cell *params)
{
	const char *szWeaponName = STRING(ALLOC_STRING(MF_GetAmxString(amx, params[1], 0, NULL)));
	int iId = Weapon_Exists(szWeaponName);

	if (iId)
	{
		WPNMOD_LOG("Error: \"%s\" already registered!\n", szWeaponName);
		return 0;
	}

	for (int i = 1; i < MAX_WEAPONS; i++)
	{
		if (!Weapon_GetpszName(i))
		{
			iId = i;
			break;
		}
	}

	if (!iId)
	{
		WPNMOD_LOG("Error: maximum weapon limit reached, \"%s\" was not registered!\n", szWeaponName);
		return 0;
	}

	m_pItemInfoArray[iId].iId = iId;
	m_pItemInfoArray[iId].pszName = szWeaponName;
	m_pItemInfoArray[iId].iSlot = params[2] - 1;
	m_pItemInfoArray[iId].iPosition = params[3] - 1;

	const char *szAmmo1 = MF_GetAmxString(amx, params[4], 0, NULL);
	if (*szAmmo1)
	{
		if (!AddAmmoNameToAmmoRegistry(szAmmo1))
		{
			// Error
		}

		m_pItemInfoArray[iId].pszAmmo1 = STRING(ALLOC_STRING(szAmmo1));
	}

	const char *szAmmo2 = MF_GetAmxString(amx, params[6], 0, NULL);
	if (*szAmmo2)
	{
		if (!AddAmmoNameToAmmoRegistry(szAmmo2))
		{
			// Error
		}

		m_pItemInfoArray[iId].pszAmmo2 = STRING(ALLOC_STRING(szAmmo2));
	}

	m_pItemInfoArray[iId].iMaxAmmo1 = params[5];
	m_pItemInfoArray[iId].iMaxAmmo2 = params[7];
	m_pItemInfoArray[iId].iMaxClip = params[8];
	m_pItemInfoArray[iId].iFlags = params[9];
	m_pItemInfoArray[iId].iWeight = params[10];

	if (!CheckSlots(iId))
	{
		// Do Something
	}

	m_WeaponsInfo[iId].m_WpnType = Wpn_Custom;

	//WeaponInfoArray[iId].title.assign(plugin->title.c_str());
	//WeaponInfoArray[iId].author.assign(plugin->author.c_str());
	//WeaponInfoArray[iId].version.assign(plugin->version.c_str());

	if (!m_bWeaponRefHooked)
	{
		m_bWeaponRefHooked = true;

		for (int k = 0; k < WeaponRefHook_End; k++)
		{
			SetHookVirtual(&g_CrowbarHooks[k]);
		}
	}

	return iId;
}

int CItems::Weapon_RegisterForward(int iId, e_WpnFwds fwdType, AMX *amx, const char * pFuncName)
{
	if (iId <= 0 || iId >= MAX_WEAPONS || !Weapon_IsCustom(iId))
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "Invalid weapon id provided (%d).", iId);
		return 0;
	}

	if (fwdType < 0 || fwdType >= Fwd_Wpn_End)
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "Function out of bounds. Got: %d, Max: %d.", iId, Fwd_Wpn_End - 1);
		return 0;
	}

	int iRegResult = MF_RegisterSPForwardByName(amx, pFuncName, FP_CELL, FP_CELL, FP_CELL, FP_CELL, FP_CELL, FP_DONE);

	if (iRegResult == -1)
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "Function not found (%d, \"%s\").", fwdType, pFuncName);
		return 0;
	}

	m_WeaponsInfo[iId].m_AmxxForwards[fwdType] = iRegResult;
	return iRegResult;
}

int CItems::Weapon_ExecuteForward(int iId, e_WpnFwds fwdType, edict_t* pWeapon, edict_t* pPlayer)
{
	if (!m_WeaponsInfo[iId].m_AmxxForwards[fwdType])
	{
		return 0;
	}

	int iAmmo1 = 0;
	int iAmmo2 = 0;
	int iPlayer = 0;

	if (IsValidPev(pPlayer))
	{
		iPlayer = ENTINDEX(pPlayer);
		iAmmo1 = GetAmmoInventory(pPlayer, PrimaryAmmoIndex(pWeapon));
		iAmmo2 = GetAmmoInventory(pPlayer, SecondaryAmmoIndex(pWeapon));
	}

	return MF_ExecuteForward
	(
		m_WeaponsInfo[iId].m_AmxxForwards[fwdType],

		static_cast<cell>(ENTINDEX(pWeapon)),
		static_cast<cell>(iPlayer),
		static_cast<cell>(GetPrivateInt(pWeapon, pvData_iClip)),
		static_cast<cell>(iAmmo1),
		static_cast<cell>(iAmmo2)
	);
}

int CItems::Weapon_GetForward(int iId, e_WpnFwds fwdType)
{
	return m_WeaponsInfo[iId].m_AmxxForwards[fwdType];
}

void CItems::Weapon_MarkAsDefault(int iId)
{
	m_WeaponsInfo[iId].m_WpnType = Wpn_Default;
}

bool CItems::Weapon_IsCustom(int iId)
{
	return m_WeaponsInfo[iId].m_WpnType == Wpn_Custom;
}

bool CItems::Weapon_IsDefault(int iId)
{
	return m_WeaponsInfo[iId].m_WpnType == Wpn_Default;
}

int CItems::Weapon_Exists(const char* szName)
{
	for (int i = 1; i < MAX_WEAPONS; i++)
	{
		if (Weapon_GetpszName(i) && !_stricmp(Weapon_GetpszName(i), szName))
		{
			return i;
		}
	}

	return 0;
}

void CItems::Weapon_ResetInfo(int iId)
{
	memset(&m_pItemInfoArray[iId], 0, sizeof(ItemInfo));
	memset(&WeaponInfoArray[iId], 0, sizeof(WeaponData));
}

