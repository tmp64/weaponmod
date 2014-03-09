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

#include "wpnmod_pvdata.h"
#include "wpnmod_utils.h"

//
// Default pvdata offsets for Bugfixed and improved HL release.
//
GameOffset GamePvDatasOffsets[pvData_End] =
{
	{4,		0},		// m_pfnThink
	{5,		0},		// m_pfnTouch
	{8,		3},		// ammo_9mm
	{9,		3},		// ammo_357
	{10,	3},		// ammo_bolts
	{11,	3},		// ammo_buckshot
	{12,	3},		// ammo_rockets
	{13,	3},		// ammo_uranium
	{14,	3},		// ammo_hornets
	{15,	3},		// ammo_argrens
	{16,	3},		// m_flStartThrow
	{17,	3},		// m_flReleaseThrow
	{18,	3},		// m_chargeReady
	{19,	3},		// m_fInAttack
	{20,	3},		// m_fireState
	{28,	4},		// m_pPlayer
	{29,	4},		// m_pNext
	{30,	4},		// m_iId
	{31,	4},		// m_iPlayEmptySound
	{32,	4},		// m_fFireOnEmpty
	{33,	4},		// m_flPumpTime
	{34,	4},		// m_fInSpecialReload
	{35,	4},		// m_flNextPrimaryAttack
	{36,	4},		// m_flNextSecondaryAttack
	{37,	4},		// m_flTimeWeaponIdle
	{38,	4},		// m_iPrimaryAmmoType
	{39,	4},		// m_iSecondaryAmmoType
	{40,	4},		// m_iClip
	{43,	4},		// m_fInReload
	{44,	4},		// m_iDefaultAmmo
	{90,	5},		// m_LastHitGroup
	{148,	5},		// m_flNextAttack
	{173,	5},		// m_iWeaponVolume
	{175,	5},		// m_iWeaponFlash
	{298,	5},		// m_iFOV
	{300,	5},		// m_rgpPlayerItems
	{306,	5},		// m_pActiveItem
	{308,	5},		// m_pLastItem
	{310,	5},		// m_rgAmmo
	{387,	5}		// m_szAnimExtention
};

void pvData_Init(void)
{
}

edict_t *GetPrivateCbase(edict_t *pEntity, int iOffset)
{
	void *pPrivate = *((void **)((int *)(edict_t *)(INDEXENT2(0) + ENTINDEX(pEntity))->pvPrivateData + GET_PVDATA_OFFSET(iOffset)));

	if (!pPrivate)
	{
		return NULL;
	}

	return PrivateToEdict(pPrivate);
}

edict_t *GetPrivateCbase(edict_t *pEntity, int iOffset, int iExtraRealOffset)
{
	void *pPrivate = *((void **)((int *)(edict_t *)(INDEXENT2(0) + ENTINDEX(pEntity))->pvPrivateData + GET_PVDATA_OFFSET(iOffset) + iExtraRealOffset));

	if (!pPrivate)
	{
		return NULL;
	}

	return PrivateToEdict(pPrivate);
}

void SetPrivateCbase(edict_t *pEntity, int iOffset, edict_t* pValue)
{
	*((void**)((int*)(edict_t*)(INDEXENT2(0) + ENTINDEX(pEntity))->pvPrivateData + GET_PVDATA_OFFSET(iOffset))) = (INDEXENT2(0) + ENTINDEX(pValue))->pvPrivateData;
}

void SetPrivateCbase(edict_t *pEntity, int iOffset, edict_t* pValue, int iExtraRealOffset)
{
	*((void**)((int*)(edict_t*)(INDEXENT2(0) + ENTINDEX(pEntity))->pvPrivateData + GET_PVDATA_OFFSET(iOffset) + iExtraRealOffset)) = (INDEXENT2(0) + ENTINDEX(pValue))->pvPrivateData;
}

void SetPrivateString(edict_t* pEntity, int iOffset, const char* pValue)
{
	char* data = (char*)pEntity->pvPrivateData + (GET_PVDATA_OFFSET(iOffset) * 4);

	#if defined WIN32
		if (!IsBadWritePtr(data, 1))
		{
			strcpy(data, pValue);
		}
	#else
		strcpy(data, pValue);
	#endif
}

void SetTouch_(edict_t* pEntity, void* funcAddress)
{
#ifdef __linux__

	if (!g_ExtraTouch)
	{
		*((long*)pEntity->pvPrivateData + GET_PVDATA_OFFSET(pvData_pfnTouch)) = funcAddress == NULL ? NULL : 0xFFFF0000;
	}

	*((long*)pEntity->pvPrivateData + GET_PVDATA_OFFSET(pvData_pfnTouch) + 1) = (long)(funcAddress);

#else

	*((long*)pEntity->pvPrivateData + GET_PVDATA_OFFSET(pvData_pfnTouch)) = (long)(funcAddress);

#endif
}

void SetThink_(edict_t* pEntity, void* funcAddress)
{
#ifdef __linux__

	if (!g_ExtraThink)
	{
		*((long*)pEntity->pvPrivateData + GET_PVDATA_OFFSET(pvData_pfnThink) - 1) = funcAddress == NULL ? NULL : 0xFFFF0000;
	}

	*((long*)pEntity->pvPrivateData + GET_PVDATA_OFFSET(pvData_pfnThink)) = (long)(funcAddress);

#else

	*((long*)pEntity->pvPrivateData + GET_PVDATA_OFFSET(pvData_pfnThink)) = (long)(funcAddress);

#endif
}
