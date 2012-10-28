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

#ifndef _HOOKS_H
#define _HOOKS_H

#include "weaponmod.h"
#include "libFunc.h"


#define _CBHOOK(call) \
	{ \
		"weapon_crowbar", NULL, NULL, NULL, (void*)Weapon_##call, \
	} 

enum e_VtblOffsets
{
	VO_Spawn,
	VO_Precache,
	VO_Classify,
	VO_TraceAttack,
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

enum e_PvDataOffsets
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
	pvData_pActiveItem,
	pvData_rgAmmo,
	pvData_szAnimExtention,

	pvData_End
};

enum e_DllFuncs
{
	Func_RadiusDamage,
	Func_GetAmmoIndex,
	Func_ClearMultiDamage,
	Func_ApplyMultiDamage,
	Func_PlayerSetAnimation,
	Func_PrecacheOtherWeapon,
	Func_GiveNamedItem,
	Func_CheatImpulseCommands,

	Func_End
};

enum e_CrowbarHooks
{
	CrowbarHook_Respawn,
	CrowbarHook_AddToPlayer,
	CrowbarHook_GetItemInfo,
	CrowbarHook_CanDeploy,
	CrowbarHook_Deploy,
	CrowbarHook_CanHolster,
	CrowbarHook_Holster,
	CrowbarHook_ItemPostFrame,
	CrowbarHook_ItemSlot,
	CrowbarHook_IsUseable,

	CrowbarHook_End
};

extern module hl_dll;

extern int g_vtblOffsets[VO_End];
extern int g_pvDataOffsets[pvData_End];

extern function g_dllFuncs[Func_End];

extern VirtHookData g_RpgAddAmmo_Hook;
extern VirtHookData g_PlayerSpawn_Hook;
extern VirtHookData g_WorldPrecache_Hook;
extern VirtHookData g_CrowbarHooks[CrowbarHook_End];

extern edict_t* Ammo_Spawn(const char* szName, Vector vecOrigin, Vector vecAngles);
extern edict_t* Weapon_Spawn(const char* szName, Vector vecOrigin, Vector vecAngles);

extern void SetVDataOffsets();

#ifdef _WIN32
extern void __cdecl PrecacheOtherWeapon_HookHandler(const char *szClassname);
extern int __fastcall Item_Block(void *pPrivate, int i, void *pPrivate2);
extern int __fastcall Weapon_GetItemInfo(void *pPrivate, int i, ItemInfo *p);
extern int __fastcall Weapon_AddToPlayer(void *pPrivate, int i, void *pPrivate2);
extern int __fastcall Weapon_ItemSlot(void *pPrivate);
extern void __fastcall Player_Spawn(void *pPrivate);
extern void __fastcall Global_Touch(void *pPrivate, int i, void *pPrivate2);
extern void __fastcall Global_Think(void *pPrivate);
extern void __fastcall CheatImpulseCommands_HookHandler(void *pPrivate, int i, int iImpulse);
extern void __fastcall GiveNamedItem_HookHandler(void *pPrivate, int i, const char *szName);
extern void __fastcall Weapon_Holster(void *pPrivate, int i, int skiplocal);
extern void __fastcall Weapon_ItemPostFrame(void *pPrivate);
extern void __fastcall World_Precache(void *pPrivate);
extern void* __fastcall Weapon_Respawn(void *pPrivate);
extern BOOL __fastcall Weapon_CanHolster(void *pPrivate);
extern BOOL __fastcall Weapon_IsUseable(void *pPrivate);
extern BOOL __fastcall Weapon_Deploy(void *pPrivate);
extern BOOL __fastcall Weapon_CanDeploy(void *pPrivate);
extern BOOL __fastcall AmmoBox_AddAmmo(void *pPrivate, int i, void *pPrivateOther);

inline int GET_DAMAGE_DECAL(edict_t* pEntity)
{
	return reinterpret_cast<int (__fastcall *)(void *, int, int)>((*((void***)((char*)pEntity->pvPrivateData)))[g_vtblOffsets[VO_DamageDecal]])(pEntity->pvPrivateData, 0, 0);
}

inline void GET_ITEM_INFO(edict_t* pItem, ItemInfo *p)
{
	reinterpret_cast<int (__fastcall *)(void *, int, ItemInfo *)>((*((void***)((char*)pItem->pvPrivateData)))[g_vtblOffsets[VO_GetItemInfo]])(pItem->pvPrivateData, 0, p);
}

inline void CLEAR_MULTI_DAMAGE()
{
	reinterpret_cast<int (__cdecl *)()>(g_dllFuncs[Func_ClearMultiDamage].address)();
}

inline void APPLY_MULTI_DAMAGE(edict_t* pInlictor, edict_t* pAttacker)
{
	reinterpret_cast<int (__cdecl *)(entvars_t*, entvars_t*)>(g_dllFuncs[Func_ApplyMultiDamage].address)(&(pInlictor->v), &(pAttacker->v));
}

inline int CLASSIFY(edict_t* pEntity)
{
	return reinterpret_cast<int (__fastcall *)(void *, int)>((*((void***)((char*)pEntity->pvPrivateData)))[g_vtblOffsets[VO_Classify]])(pEntity->pvPrivateData, 0);
}

inline void TRACE_ATTACK(edict_t* pEntity, edict_t* pAttacker, float flDamage, Vector vecDir, TraceResult tr, int bitsDamageType)
{
	reinterpret_cast<int (__fastcall *)(void *, int, entvars_t *, float, Vector, TraceResult *, int)>((*((void***)((char*)pEntity->pvPrivateData)))[g_vtblOffsets[VO_TraceAttack]])(pEntity->pvPrivateData, 0, &(pAttacker->v), flDamage, vecDir,  &tr, bitsDamageType);
}

inline int GET_AMMO_INDEX(const char *ammoname)
{
	return reinterpret_cast<int (__cdecl *)(const char *)>(g_dllFuncs[Func_GetAmmoIndex].address)(ammoname);
}
#else
extern int Item_Block(void *pPrivate, void *pPrivate2);
extern int Weapon_GetItemInfo(void *pPrivate, ItemInfo *p);
extern int Weapon_AddToPlayer(void *pPrivate, void *pPrivate2);
extern int Weapon_ItemSlot(void *pPrivate);
extern void Player_Spawn(void *pPrivate);
extern void Global_Touch(void *pPrivate, void *pPrivate2);
extern void Global_Think(void *pPrivate);
extern void GiveNamedItem_HookHandler(void *pPrivate, const char *szName);
extern void PrecacheOtherWeapon_HookHandler(const char *szClassname);
extern void CheatImpulseCommands_HookHandler(void *pPrivate, int iImpulse);
extern void Weapon_Holster(void *pPrivate, int skiplocal);
extern void Weapon_ItemPostFrame(void *pPrivate);
extern void World_Precache(void *pPrivate);
extern void* Weapon_Respawn(void *pPrivate);
extern BOOL Weapon_CanHolster(void *pPrivate);
extern BOOL Weapon_IsUseable(void *pPrivate);
extern BOOL Weapon_Deploy(void *pPrivate);
extern BOOL Weapon_CanDeploy(void *pPrivate);
extern BOOL AmmoBox_AddAmmo(void *pPrivate, void *pPrivateOther);

inline int GET_DAMAGE_DECAL(edict_t* pEntity)
{
	return reinterpret_cast<int (*)(void *, int)>((*((void***)(((char*)pEntity->pvPrivateData) + g_Base)))[g_vtblOffsets[VO_DamageDecal]])(pEntity->pvPrivateData, 0);
}

inline void GET_ITEM_INFO(edict_t* pItem, ItemInfo *p)
{
	reinterpret_cast<int (*)(void *, ItemInfo *)>((*((void***)(((char*)pItem->pvPrivateData) + g_Base)))[g_vtblOffsets[VO_GetItemInfo]])(pItem->pvPrivateData, p);
}

inline vod CLEAR_MULTI_DAMAGE()
{
	reinterpret_cast<int (*)()>(g_dllFuncs[Func_ClearMultiDamage].address)();
}

inline void APPLY_MULTI_DAMAGE(edict_t* pInlictor, edict_t* pAttacker)
{
	reinterpret_cast<int (*)(entvars_t*, entvars_t*)>(g_dllFuncs[Func_ApplyMultiDamage].address)(&(pInlictor->v), &(pAttacker->v));
}

inline int CLASSIFY(edict_t* pEntity)
{
	return reinterpret_cast<int (*)(void *)>((*((void***)(((char*)pEntity->pvPrivateData) + g_Base)))[g_vtblOffsets[VO_Classify]])(pEntity->pvPrivateData);
}

inline void TRACE_ATTACK(edict_t* pEntity, edict_t* pAttacker, float flDamage, Vector vecDir, TraceResult tr, int bitsDamageType)
{
	reinterpret_cast<int (*)(void *, entvars_t *, float, Vector, TraceResult *, int)>((*((void***)(((char*)pEntity->pvPrivateData) + g_Base)))[g_vtblOffsets[VO_TraceAttack]])(pEntity->pvPrivateData, &(pAttacker->v), flDamage, vecDir,  &tr, bitsDamageType);
}

inline int GET_AMMO_INDEX(const char *ammoname)
{
	return reinterpret_cast<int (*)(const char *)>(g_dllFuncs[Func_GetAmmoIndex].address)(ammoname);
}

//implement these with setjmp later.
inline bool IsBadReadPtr(void *l, size_t size)
{
	return false;
}

inline bool IsBadWritePtr(void *l, size_t size)
{
	return false;
}
#endif

inline void SetTouch_(edict_t* e, void* funcAddress) 
{     
#ifdef __linux__         
	*((long*)e->pvPrivateData + g_pvDataOffsets[pvData_pfnTouch]) = funcAddress == NULL ? NULL : 0xFFFF0000;         
	*((long*)e->pvPrivateData +g_pvDataOffsets[pvData_pfnTouch] + 1) = (long)(funcAddress);     
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

#endif // _HOOKS_H
