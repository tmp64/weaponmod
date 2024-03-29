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

#ifndef _PVDATA_H
#define _PVDATA_H

#include "sdk/amxxmodule.h"
#include "sdk/HLTypeConversion.h"
#include "wpnmod_vtable.h"

#define GET_PVDATA_OFFSET(x) GamePvDatasOffsets[x].fieldOffset

enum PrivateDataOffsets
{
	pvData_pfnThink,
	pvData_pfnTouch,
	
	pvData_ammo_9mm,
	pvData_ammo_357,
	pvData_ammo_bolts,
	pvData_ammo_buckshot,
	pvData_ammo_rockets,
	pvData_ammo_uranium,
	pvData_ammo_hornets,
	pvData_ammo_argrens,
	
	pvData_flStartThrow,
	pvData_flReleaseThrow,
	pvData_chargeReady,
	pvData_fInAttack,
	pvData_fireState,
	pvData_pPlayer,
	pvData_pNext,
	pvData_iId,
	pvData_iPlayEmptySound,
	pvData_fFireOnEmpty,
	pvData_flPumpTime,
	pvData_fInSpecialReload,
	pvData_flNextPrimaryAttack,
	pvData_flNextSecondaryAttack,
	pvData_flTimeWeaponIdle,
	pvData_iPrimaryAmmoType,
	pvData_iSecondaryAmmoType,
	pvData_iClip,
	pvData_fInReload,
	pvData_iDefaultAmmo,

	pvData_LastHitGroup,
	pvData_flNextAttack,
	pvData_iWeaponVolume,
	pvData_iWeaponFlash,
	pvData_iFOV,
	pvData_rgpPlayerItems,
	pvData_pActiveItem,
	pvData_pLastItem,
	pvData_rgAmmo,
	pvData_szAnimExtention,

	pvData_End
};

extern void pvData_Init(void);
extern TypeDescription GamePvDatasOffsets[pvData_End];

inline int GetPrivateInt(edict_t* pEntity, int iOffset)
{
	return get_pdata<int>(pEntity, GET_PVDATA_OFFSET(iOffset));
}

inline int GetPrivateInt(edict_t* pEntity, int iOffset, int iExtraRealOffset)
{
	return get_pdata<int>(pEntity, GET_PVDATA_OFFSET(iOffset), iExtraRealOffset);
}

inline float GetPrivateFloat(edict_t* pEntity, int iOffset)
{
	return get_pdata<float>(pEntity, GET_PVDATA_OFFSET(iOffset));
}

inline char* GetPrivateString(edict_t* pEntity, int iOffset)
{
	return get_pdata<char*>(pEntity, GET_PVDATA_OFFSET(iOffset));
}

inline void SetPrivateInt(edict_t* pEntity, int iOffset, int iValue)
{
	get_pdata<int>(pEntity, GET_PVDATA_OFFSET(iOffset)) = iValue;
}

inline void SetPrivateInt(edict_t* pEntity, int iOffset,int iValue, int iExtraRealOffset)
{
	get_pdata<int>(pEntity, GET_PVDATA_OFFSET(iOffset), iExtraRealOffset) = iValue;
}

inline void SetPrivateFloat(edict_t* pEntity, int iOffset, float flValue)
{
	get_pdata<float>(pEntity, GET_PVDATA_OFFSET(iOffset)) = flValue;
}

extern void		Dll_SetTouch		(edict_t* pEntity, void* funcAddress);
extern void		Dll_SetThink		(edict_t* pEntity, void* funcAddress);

extern void		SetPrivateCbase		(edict_t* pEntity, int iOffset, edict_t* pValue);
extern void		SetPrivateCbase		(edict_t* pEntity, int iOffset, edict_t* pValue, int iExtraRealOffset);
extern void		SetPrivateString	(edict_t* pEntity, int iOffset, const char* pValue);

extern edict_t*	PrivateToEdict		(const void* pvPrivateData);

extern edict_t*	GetPrivateCbase		(edict_t* pEntity, int iOffset);
extern edict_t*	GetPrivateCbase		(edict_t* pEntity, int iOffset, int iExtraRealOffset);

#endif  // _PVDATA_H
