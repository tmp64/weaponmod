/*
 * Half-Life Weapon Mod
 * Copyright (c) 2012 - 2013 AGHL.RU Dev Team
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

#ifndef _UTILS_H
#define _UTILS_H

#include "amxxmodule.h"
#include "wpnmod_vhooker.h"

#if defined WIN32

	#define DUMMY_VAL 0
	typedef int DUMMY;

#endif

enum VTableOffsets 
{
	VO_Spawn,
	VO_Precache,
	VO_Classify,
	VO_TraceAttack,
	VO_TakeDamage,
	VO_DamageDecal,
	VO_Respawn,
	VO_AddAmmo,
	VO_AddToPlayer,
	VO_GetItemInfo,
	VO_CanDeploy,
	VO_Deploy,
	VO_CanHolster,
	VO_Holster,
	VO_ItemPostFrame,
	VO_ItemSlot,
	VO_IsUseable,

	VO_End
};

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

extern int g_vtblOffsets[VO_End];
extern int g_pvDataOffsets[pvData_End];

extern edict_t*	INDEXENT2		(int iEdictNum);
extern edict_t*	GetPrivateCbase	(edict_t *pEntity, int iOffset);
extern edict_t*	GetPrivateCbase	(edict_t *pEntity, int iOffset, int iExtraRealOffset);
extern void		SetPrivateCbase	(edict_t *pEntity, int iOffset, edict_t* pValue);

extern int PrimaryAmmoIndex		(edict_t *pEntity);
extern int SecondaryAmmoIndex	(edict_t *pEntity);

extern int GetAmmoInventory(edict_t* pPlayer, int iAmmoIndex);
extern int SetAmmoInventory(edict_t* pPlayer, int iAmmoIndex, int iAmount);

inline edict_t* PrivateToEdict(const void* pdata)
{
	if (!pdata || (int)pdata == -1)
	{
		return NULL;
	}

	char* ptr = (char*)pdata + g_EntityVTableOffsetPev;

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
};

inline int GetPrivateInt(edict_t* pEntity, int iOffset)
{
	return *((int*)pEntity->pvPrivateData + g_pvDataOffsets[iOffset]);
}

inline int GetPrivateInt(edict_t* pEntity, int iOffset, int iExtraRealOffse)
{
	return *((int*)pEntity->pvPrivateData + g_pvDataOffsets[iOffset] + iExtraRealOffse);
}

inline float GetPrivateFloat(edict_t* pEntity, int iOffset)
{
	return *((float*)pEntity->pvPrivateData + g_pvDataOffsets[iOffset]);
}

inline char* GetPrivateString(edict_t* pEntity, int iOffset)
{
	return (char*)pEntity->pvPrivateData + (g_pvDataOffsets[iOffset] * 4);
}

inline void SetPrivateInt(edict_t* pEntity, int iOffset, int iValue)
{
	*((int*)pEntity->pvPrivateData + g_pvDataOffsets[iOffset]) = iValue;
}

inline void SetPrivateInt(edict_t* pEntity, int iOffset,int iValue, int iExtraRealOffse)
{
	*((int*)pEntity->pvPrivateData + g_pvDataOffsets[iOffset] + iExtraRealOffse) = iValue;
}

inline void SetPrivateFloat(edict_t* pEntity, int iOffset, float flValue)
{
	*((float*)pEntity->pvPrivateData + g_pvDataOffsets[iOffset]) = flValue;
}

inline void SetPrivateString(edict_t* pEntity, int iOffset, const char* pValue)
{
	char* data = (char*)pEntity->pvPrivateData + (g_pvDataOffsets[iOffset] * 4);

	#if defined WIN32
		if (!IsBadWritePtr(data, 1))
		{
			strcpy(data, pValue);
		}
	#else
		strcpy(data, newValue);
	#endif
}

inline void SetTouch_(edict_t* e, void* funcAddress) 
{     
#ifdef __linux__         
	*((long*)e->pvPrivateData + g_pvDataOffsets[pvData_pfnTouch]) = funcAddress == NULL ? NULL : 0xFFFF0000;         
	*((long*)e->pvPrivateData + g_pvDataOffsets[pvData_pfnTouch] + 1) = (long)(funcAddress);     
#else         
	*((long*)e->pvPrivateData + g_pvDataOffsets[pvData_pfnTouch]) = (long)(funcAddress);     
#endif 
}

inline void SetThink_(edict_t* e, void* funcAddress) 
{     
#ifdef __linux__         
	*((long*)e->pvPrivateData + g_pvDataOffsets[pvData_pfnThink] - 1) = funcAddress == NULL ? NULL : 0xFFFF0000;         
	*((long*)e->pvPrivateData + g_pvDataOffsets[pvData_pfnThink]) = (long)(funcAddress);     
#else         
	*((long*)e->pvPrivateData + g_pvDataOffsets[pvData_pfnThink]) = (long)(funcAddress);     
#endif 
}

#define IsValidPev(entity)	((int)entity != -1 && !FNullEnt(entity) && entity->pvPrivateData)

#define CBTEXTURENAMEMAX		13

#define CHAR_TEX_CONCRETE		'C'
#define CHAR_TEX_METAL			'M'
#define CHAR_TEX_DIRT			'D'
#define CHAR_TEX_VENT			'V'
#define CHAR_TEX_GRATE			'G'
#define CHAR_TEX_TILE			'T'
#define CHAR_TEX_SLOSH			'S'
#define CHAR_TEX_WOOD			'W'
#define CHAR_TEX_COMPUTER		'P'
#define CHAR_TEX_GLASS			'Y'
#define CHAR_TEX_FLESH			'F'

extern	BOOL	SwitchWeapon		(edict_t* pPlayer, edict_t* pWeapon);
extern	BOOL	Entity_IsInWorld	(edict_t *pEntity);
extern	BOOL	GetNextBestWeapon	(edict_t* pPlayer, edict_t* pCurrentWeapon);

extern	void	SelectItem			(edict_t *pPlayer, const char *pstr);
extern	void	RadiusDamage2		(Vector vecSrc, edict_t* pInflictor, edict_t* pAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType);
extern	void	GiveNamedItem		(edict_t *pPlayer, const char *szName);
extern	void	SendWeaponAnim		(edict_t* pPlayer, edict_t* pWeapon, int iAnim);
extern	void	SelectLastItem		(edict_t *pPlayer);
extern	void	UTIL_EjectBrass		(const Vector &vecOrigin, const Vector &vecVelocity, float rotation, int model, int soundtype);
extern	void	UTIL_DecalGunshot	(TraceResult *pTrace);
extern	void	FireBulletsPlayer	(edict_t* pPlayer, edict_t* pAttacker, int iShotsCount, Vector vecSpread, float flDistance, float flDamage, int bitsDamageType, BOOL bTracers);


#endif // _UTILS_H
