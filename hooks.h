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
#include "wpnmod_utils.h"


#define _CBHOOK(call) \
	{ \
		"weapon_crowbar", NULL, NULL, NULL, (void*)Weapon_##call, \
	} 

extern VirtualHookData g_RpgAddAmmo_Hook;
extern VirtualHookData g_PlayerSpawn_Hook;
extern VirtualHookData g_WorldPrecache_Hook;

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

extern function g_dllFuncs[Func_End];




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

inline void HOLSTER(edict_t* pItem)
{
	reinterpret_cast<void (__fastcall *)(void *, int, int)>((*((void***)((char*)pItem->pvPrivateData)))[g_vtblOffsets[VO_Holster]])(pItem->pvPrivateData, 0, 0);
}

inline void DEPLOY(edict_t* pItem)
{
	reinterpret_cast<void (__fastcall *)(void *, int)>((*((void***)((char*)pItem->pvPrivateData)))[g_vtblOffsets[VO_Deploy]])(pItem->pvPrivateData, 0);
}

inline BOOL CAN_DEPLOY(edict_t* pEntity)
{
	return reinterpret_cast<BOOL (__fastcall *)(void *, int)>((*((void***)((char*)pEntity->pvPrivateData)))[g_vtblOffsets[VO_CanDeploy]])(pEntity->pvPrivateData, 0);
}

inline BOOL CAN_HOLSTER(edict_t* pEntity)
{
	return reinterpret_cast<BOOL (__fastcall *)(void *, int)>((*((void***)((char*)pEntity->pvPrivateData)))[g_vtblOffsets[VO_CanHolster]])(pEntity->pvPrivateData, 0);
}

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

inline void TAKE_DAMAGE(edict_t* pEntity, edict_t* pInflictor, edict_t* pAttacker, float flDamage, int bitsDamageType)
{
	reinterpret_cast<int (__fastcall *)(void *, int, entvars_t *, entvars_t *, float, int)>((*((void***)((char*)pEntity->pvPrivateData)))[g_vtblOffsets[VO_TakeDamage]])(pEntity->pvPrivateData, 0, &(pInflictor->v), &(pAttacker->v), flDamage, bitsDamageType);
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

inline void HOLSTER(edict_t* pItem)
{
	reinterpret_cast<int (*)(void *)>((*((void***)(((char*)pItem->pvPrivateData) + g_Base)))[g_vtblOffsets[VO_Holster]])(pItem->pvPrivateData);
}

inline void DEPLOY(edict_t* pItem)
{
	reinterpret_cast<int (*)(void *)>((*((void***)(((char*)pItem->pvPrivateData) + g_Base)))[g_vtblOffsets[VO_Deploy]])(pItem->pvPrivateData);
}

inline BOOL CAN_DEPLOY(edict_t* pEntity)
{
	return reinterpret_cast<BOOL (*)(void *)>((*((void***)(((char*)pEntity->pvPrivateData) + g_Base)))[g_vtblOffsets[VO_CanDeploy]])(pEntity->pvPrivateData);
}

inline BOOL CAN_HOLSTER(edict_t* pEntity)
{
	return reinterpret_cast<BOOL (*)(void *)>((*((void***)(((char*)pEntity->pvPrivateData) + g_Base)))[g_vtblOffsets[VO_CanHolster]])(pEntity->pvPrivateData);
}

inline int GET_DAMAGE_DECAL(edict_t* pEntity)
{
	return reinterpret_cast<int (*)(void *, int)>((*((void***)(((char*)pEntity->pvPrivateData) + g_Base)))[g_vtblOffsets[VO_DamageDecal]])(pEntity->pvPrivateData, 0);
}

inline void GET_ITEM_INFO(edict_t* pItem, ItemInfo *p)
{
	reinterpret_cast<int (*)(void *, ItemInfo *)>((*((void***)(((char*)pItem->pvPrivateData) + g_Base)))[g_vtblOffsets[VO_GetItemInfo]])(pItem->pvPrivateData, p);
}

inline void CLEAR_MULTI_DAMAGE()
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

inline void TAKE_DAMAGE(edict_t* pEntity, edict_t* pInflictor, edict_t* pAttacker, float flDamage, int bitsDamageType)
{
	reinterpret_cast<int (*)(void *, entvars_t *, entvars_t *, float, int)>((*((void***)(((char*)pEntity->pvPrivateData) + 0x60)))[g_vtblOffsets[VO_TakeDamage]])(pEntity->pvPrivateData, &(pInflictor->v), &(pAttacker->v), flDamage, bitsDamageType);
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

#endif // _HOOKS_H
