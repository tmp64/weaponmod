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

#include "sdk/amxx_gameconfigs.h"
#include "wpnmod_pvdata.h"
#include "wpnmod_memory.h"
#include "wpnmod_config.h"
#include "wpnmod_utils.h"

//! List of offsets.
TypeDescription GamePvDatasOffsets[pvData_End];

namespace
{

//! Class and field for which an offset must be loaded.
struct OffsetInitializer
{
	const char* szClassName;
	const char* szFieldName;
};

//! List class and field name for each offset.
OffsetInitializer g_OffsetInitializers[pvData_End];

} // namespace

void pvData_Init(void)
{
	// CBaseEntity
	g_OffsetInitializers[pvData_pfnThink] = { "CBaseEntity", "m_pfnThink" };
	g_OffsetInitializers[pvData_pfnTouch] = { "CBaseEntity", "m_pfnTouch" };
	g_OffsetInitializers[pvData_ammo_9mm] = { "CBaseEntity", "ammo_9mm" };
	g_OffsetInitializers[pvData_ammo_357] = { "CBaseEntity", "ammo_357" };
	g_OffsetInitializers[pvData_ammo_bolts] = { "CBaseEntity", "ammo_bolts" };
	g_OffsetInitializers[pvData_ammo_buckshot] = { "CBaseEntity", "ammo_buckshot" };
	g_OffsetInitializers[pvData_ammo_rockets] = { "CBaseEntity", "ammo_rockets" };
	g_OffsetInitializers[pvData_ammo_uranium] = { "CBaseEntity", "ammo_uranium" };
	g_OffsetInitializers[pvData_ammo_hornets] = { "CBaseEntity", "ammo_hornets" };
	g_OffsetInitializers[pvData_ammo_argrens] = { "CBaseEntity", "ammo_argrens" };
	g_OffsetInitializers[pvData_flStartThrow] = { "CBaseEntity", "m_flStartThrow" };
	g_OffsetInitializers[pvData_flReleaseThrow] = { "CBaseEntity", "m_flReleaseThrow" };
	g_OffsetInitializers[pvData_chargeReady] = { "CBaseEntity", "m_chargeReady" };
	g_OffsetInitializers[pvData_fInAttack] = { "CBaseEntity", "m_fInAttack" };
	g_OffsetInitializers[pvData_fireState] = { "CBaseEntity", "m_fireState" };

	// CBasePlayerItem
	g_OffsetInitializers[pvData_pPlayer] = { "CBasePlayerItem", "m_pPlayer" };
	g_OffsetInitializers[pvData_pNext] = { "CBasePlayerItem", "m_pNext" };
	g_OffsetInitializers[pvData_iId] = { "CBasePlayerItem", "m_iId" };

	// CBasePlayerWeapon
	g_OffsetInitializers[pvData_iPlayEmptySound] = { "CBasePlayerWeapon", "m_iPlayEmptySound" };
	g_OffsetInitializers[pvData_fFireOnEmpty] = { "CBasePlayerWeapon", "m_fFireOnEmpty" };
	g_OffsetInitializers[pvData_flPumpTime] = { "CBasePlayerWeapon", "m_flPumpTime" };
	g_OffsetInitializers[pvData_fInSpecialReload] = { "CBasePlayerWeapon", "m_fInSpecialReload" };
	g_OffsetInitializers[pvData_flNextPrimaryAttack] = { "CBasePlayerWeapon", "m_flNextPrimaryAttack" };
	g_OffsetInitializers[pvData_flNextSecondaryAttack] = { "CBasePlayerWeapon", "m_flNextSecondaryAttack" };
	g_OffsetInitializers[pvData_flTimeWeaponIdle] = { "CBasePlayerWeapon", "m_flTimeWeaponIdle" };
	g_OffsetInitializers[pvData_iPrimaryAmmoType] = { "CBasePlayerWeapon", "m_iPrimaryAmmoType" };
	g_OffsetInitializers[pvData_iSecondaryAmmoType] = { "CBasePlayerWeapon", "m_iSecondaryAmmoType" };
	g_OffsetInitializers[pvData_iClip] = { "CBasePlayerWeapon", "m_iClip" };
	g_OffsetInitializers[pvData_fInReload] = { "CBasePlayerWeapon", "m_fInReload" };
	g_OffsetInitializers[pvData_iDefaultAmmo] = { "CBasePlayerWeapon", "m_iDefaultAmmo" };
	
	// CBaseMonster
	g_OffsetInitializers[pvData_LastHitGroup] = { "CBaseMonster", "m_LastHitGroup" };
	g_OffsetInitializers[pvData_flNextAttack] = { "CBaseMonster", "m_flNextAttack" };

	// CBasePlayer
	g_OffsetInitializers[pvData_iWeaponVolume] = { "CBasePlayer", "m_iWeaponVolume" };
	g_OffsetInitializers[pvData_iWeaponFlash] = { "CBasePlayer", "m_iWeaponFlash" };
	g_OffsetInitializers[pvData_iFOV] = { "CBasePlayer", "m_iFOV" };
	g_OffsetInitializers[pvData_rgpPlayerItems] = { "CBasePlayer", "m_rgpPlayerItems" };
	g_OffsetInitializers[pvData_pActiveItem] = { "CBasePlayer", "m_pActiveItem" };
	g_OffsetInitializers[pvData_pLastItem] = { "CBasePlayer", "m_pLastItem" };
	g_OffsetInitializers[pvData_rgAmmo] = { "CBasePlayer", "m_rgAmmo" };
	g_OffsetInitializers[pvData_szAnimExtention] = { "CBasePlayer", "m_szAnimExtention" };

	// Load offsets
	IGameConfigPtr pCfg = WpnMod_LoadGameConfigFile("common.games");
	bool anyNotFound = false;

	for (int i = 0; i < std::size(g_OffsetInitializers); i++)
	{
		const OffsetInitializer& init = g_OffsetInitializers[i];
		
		if (!pCfg->GetOffsetByClass(init.szClassName, init.szFieldName, &GamePvDatasOffsets[i]))
		{
			WPNMOD_LOG("Offset not found: %s::%s\n", init.szClassName, init.szFieldName);
			anyNotFound = true;
		}
	}

	if (anyNotFound)
	{
		WPNMOD_LOG("Failed to find some offsets. The server will now crash. Goodbye.\n");
		std::abort();
	}
}

edict_t* PrivateToEdict(const void* pvPrivateData)
{
	if (!pvPrivateData || (int)pvPrivateData == -1)
	{
		return NULL;
	}

	char* ptr = (char*)pvPrivateData + g_EntityVTableOffsetPev;

	if (!ptr)
	{
		return NULL;
	}

	entvars_t* pev = *(entvars_t**)ptr;

	if (!pev)
	{
		return NULL;
	}

	return pev->pContainingEntity;
}

edict_t *GetPrivateCbase(edict_t *pEntity, int iOffset)
{
	void* pPrivate = get_pdata<void*>(pEntity, GET_PVDATA_OFFSET(iOffset));

	if (!pPrivate)
	{
		return NULL;
	}

	return PrivateToEdict(pPrivate);
}

edict_t *GetPrivateCbase(edict_t *pEntity, int iOffset, int iExtraRealOffset)
{
	void* pPrivate = get_pdata<void*>(pEntity, GET_PVDATA_OFFSET(iOffset), iExtraRealOffset);

	if (!pPrivate)
	{
		return NULL;
	}

	return PrivateToEdict(pPrivate);
}

void SetPrivateCbase(edict_t *pEntity, int iOffset, edict_t* pValue)
{
	get_pdata<void*>(pEntity, GET_PVDATA_OFFSET(iOffset)) = (INDEXENT2(0) + ENTINDEX(pValue))->pvPrivateData;
}

void SetPrivateCbase(edict_t *pEntity, int iOffset, edict_t* pValue, int iExtraRealOffset)
{
	get_pdata<void*>(pEntity, GET_PVDATA_OFFSET(iOffset), iExtraRealOffset) = (INDEXENT2(0) + ENTINDEX(pValue))->pvPrivateData;
}

void SetPrivateString(edict_t* pEntity, int iOffset, const char* pValue)
{
	char* data = get_pdata<char*>(pEntity, GET_PVDATA_OFFSET(iOffset));

	#if defined WIN32
		if (!IsBadWritePtr(data, 1))
		{
			strcpy(data, pValue);
		}
	#else
		strcpy(data, pValue);
	#endif
}

void Dll_SetTouch(edict_t* pEntity, void* funcAddress)
{
	get_pdata<void*>(pEntity, GET_PVDATA_OFFSET(pvData_pfnTouch)) = funcAddress;
}

void Dll_SetThink(edict_t* pEntity, void* funcAddress)
{
	get_pdata<void*>(pEntity, GET_PVDATA_OFFSET(pvData_pfnThink)) = funcAddress;
}

