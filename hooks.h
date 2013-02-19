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



#include "wpnmod_hooker.h"
#include "wpnmod_utils.h"


typedef struct
{
	int iThink;
	int iTouch;
	int iExplode;
} EntData;

extern EntData*	g_Ents;

#ifdef WIN32

	void __fastcall Global_Think	(void* pvEntity);
	void __fastcall Global_Touch	(void* pvEntity, int DUMMY, void* pvOther);

#else

	void Global_Think	(void* pvEntity);
	void Global_Touch	(void* pvEntity, void* pvOther);

#endif


//
// FUNCTIONS
//


	#define HOOK(call)														\
	{																		\
		"", &g_GameDllModule, {"", "", 0}, NULL, (void*)call, {}, {}, 0,	\
	}																		\

	enum LibFunctions
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

	extern module	g_GameDllModule;
	extern function g_dllFuncs[Func_End];

#ifdef WIN32

	typedef void	(__cdecl *FuncPrecacheOtherWeapon)	(const char*);
	typedef void	(__fastcall	*FuncGiveNamedItem)		(void *, DUMMY, const char *);

	void	__cdecl		PrecacheOtherWeapon_HookHandler		(const char *szClassname);
	void	__fastcall	GiveNamedItem_HookHandler			(void *pvPlayer, int DUMMY, const char *szName);

	// void CBasePlayer::GiveNamedItem(const char *pszName)
	//
		inline void GIVE_NAMED_ITEM(void* pvPlayer, const char *szClassname)
		{
			reinterpret_cast<FuncGiveNamedItem>(g_dllFuncs[Func_GiveNamedItem].address)(pvPlayer, DUMMY_VAL, szClassname);
		}

#else

	typedef void	(*FuncPrecacheOtherWeapon)	(const char*);
	typedef void	(*FuncGiveNamedItem)		(void *, const char *);

	void	PrecacheOtherWeapon_HookHandler		(const char *szClassname);
	void	GiveNamedItem_HookHandler			(void *pvPlayer, const char *szName);

	// void CBasePlayer::GiveNamedItem(const char *pszName)
	//
		inline void GIVE_NAMED_ITEM(void* pvPlayer, const char *szClassname)
		{
			reinterpret_cast<FuncGiveNamedItem>(g_dllFuncs[Func_GiveNamedItem].address)(pvPlayer, szClassname);
		}

#endif

	// void UTIL_PrecacheOtherWeapon(const char *szClassname)
	//
		inline void PRECACHE_OTHER_WEAPON(const char *szClassname)
		{
			reinterpret_cast<FuncPrecacheOtherWeapon>(g_dllFuncs[Func_PrecacheOtherWeapon].address)(szClassname);
		}


//
// VIRTUAL FUNCTIONS
//


	#define VHOOK(call)													\
	{																	\
		"weapon_crowbar", VO_##call, (void*)Weapon_##call, NULL, NULL,	\
	}																	\

	enum VirtualCrowbarHooks
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

	extern VirtualHookData g_RpgAddAmmo_Hook;
	extern VirtualHookData g_PlayerSpawn_Hook;
	extern VirtualHookData g_WorldPrecache_Hook;
	extern VirtualHookData g_CrowbarHooks[CrowbarHook_End];

#ifdef WIN32
	
	typedef int		(__fastcall *FuncGetItemInfo)	(void*, DUMMY, ItemInfo*);
	typedef BOOL	(__fastcall *FuncCanDeploy)		(void*, DUMMY);
	typedef BOOL	(__fastcall *FuncDeploy)		(void*, DUMMY);
	typedef BOOL	(__fastcall *FuncCanHolster)	(void*, DUMMY);
	typedef void	(__fastcall *FuncHolster)		(void*, DUMMY, int);
	typedef void	(__fastcall *FuncItemPostFrame)	(void*, DUMMY);
	typedef BOOL	(__fastcall *FuncIsUseable)		(void*, DUMMY);
	typedef int		(__fastcall *FuncAddToPlayer)	(void*, DUMMY, void*);
	typedef int		(__fastcall *FuncItemSlot)		(void*, DUMMY);
	typedef void*	(__fastcall *FuncRespawn)		(void*, DUMMY);
	typedef BOOL	(__fastcall *FuncAddAmmo)		(void*, DUMMY, void*);
	typedef void	(__fastcall *FuncSpawn)			(void*, DUMMY);
	typedef void	(__fastcall *FuncPrecache)		(void*, DUMMY);

	int		__fastcall Weapon_GetItemInfo	(void* pvItem, DUMMY, ItemInfo* p);
	BOOL	__fastcall Weapon_CanDeploy		(void* pvItem);
	BOOL	__fastcall Weapon_Deploy		(void* pvItem);
	BOOL	__fastcall Weapon_CanHolster	(void* pvItem);
	void	__fastcall Weapon_Holster		(void* pvItem, DUMMY, int skiplocal);
	void	__fastcall Weapon_ItemPostFrame	(void* pvItem);
	BOOL	__fastcall Weapon_IsUseable		(void* pvItem);
	int		__fastcall Weapon_AddToPlayer	(void* pvItem, DUMMY, void* pvPlayer);
	int		__fastcall Weapon_ItemSlot		(void* pvItem);
	void*	__fastcall Weapon_Respawn		(void* pvItem);
	BOOL	__fastcall AmmoBox_AddAmmo		(void* pvAmmo, DUMMY, void* pvOther);
	int		__fastcall Item_Block			(void* pvItem, int DUMMY, void* pvOther);
	void	__fastcall Player_Spawn			(void* pvPlayer);
	void	__fastcall World_Precache		(void* pvEntity);

	// virtual int CBasePlayerItem::GetItemInfo(ItemInfo* p);
	//
		inline int GET_ITEM_INFO(void* pvItem, ItemInfo* p)
		{
			return reinterpret_cast<FuncGetItemInfo>(g_CrowbarHooks[CrowbarHook_GetItemInfo].address)(pvItem, DUMMY_VAL, p);
		}

		inline int GET_ITEM_INFO(edict_t* pentItem, ItemInfo* p)
		{
			return reinterpret_cast<FuncGetItemInfo >(GET_VTABLE(pentItem)[g_vtblOffsets[VO_GetItemInfo]])(pentItem->pvPrivateData, DUMMY_VAL, p);
		}

	// virtual BOOL CanDeploy(void)
	//
		inline BOOL CAN_DEPLOY(void* pvItem)
		{
			return reinterpret_cast<FuncCanDeploy>(g_CrowbarHooks[CrowbarHook_CanDeploy].address)(pvItem, DUMMY_VAL);
		}

		inline BOOL CAN_DEPLOY(edict_t* pentItem)
		{
			return reinterpret_cast<FuncCanDeploy>(GET_VTABLE(pentItem)[g_vtblOffsets[VO_CanDeploy]])(pentItem->pvPrivateData, DUMMY_VAL);
		}

	// virtual BOOL Deploy(void);
	//
		inline BOOL DEPLOY(void* pvItem)
		{
			return reinterpret_cast<FuncDeploy>(g_CrowbarHooks[CrowbarHook_Deploy].address)(pvItem, DUMMY_VAL);
		}

		inline BOOL DEPLOY(edict_t* pentItem)
		{
				return reinterpret_cast<FuncDeploy>(GET_VTABLE(pentItem)[g_vtblOffsets[VO_Deploy]])(pentItem->pvPrivateData, DUMMY_VAL);
		}

	// virtual BOOL CanHolster(void);
	//
		inline BOOL CAN_HOLSTER( void* pvItem )
		{
			return reinterpret_cast<FuncCanHolster>(g_CrowbarHooks[CrowbarHook_CanHolster].address)(pvItem, DUMMY_VAL);
		}

		inline BOOL CAN_HOLSTER( edict_t* pentItem )
		{
			return reinterpret_cast<FuncCanHolster>(GET_VTABLE(pentItem)[g_vtblOffsets[VO_CanHolster]])(pentItem->pvPrivateData, DUMMY_VAL);
		}

	// virtual void Holster(int skiplocal = 0);
	//
		inline void HOLSTER(void* pvItem)
		{
			reinterpret_cast<FuncHolster>(g_CrowbarHooks[CrowbarHook_Holster].address)(pvItem, DUMMY_VAL, 0);
		}

		inline void HOLSTER(edict_t* pentItem)
		{
			reinterpret_cast<FuncHolster>(GET_VTABLE(pentItem)[g_vtblOffsets[VO_Holster]])(pentItem->pvPrivateData, DUMMY_VAL, 0);
		}

	// virtual void ItemPostFrame(void);
	//
		inline void ITEM_POST_FRAME(void* pvItem)
		{
			reinterpret_cast<FuncItemPostFrame>(g_CrowbarHooks[CrowbarHook_ItemPostFrame].address)(pvItem, DUMMY_VAL);
		}

		inline void ITEM_POST_FRAME(edict_t* pentItem)
		{
			reinterpret_cast<FuncItemPostFrame>(GET_VTABLE(pentItem)[g_vtblOffsets[VO_ItemPostFrame]])(pentItem->pvPrivateData, DUMMY_VAL);
		}

	// virtual BOOL IsUseable(void);
	//
		inline BOOL IS_USEABLE(void* pvItem)
		{
			return reinterpret_cast<FuncIsUseable>(g_CrowbarHooks[CrowbarHook_IsUseable].address)(pvItem, DUMMY_VAL);
		}

		inline BOOL IS_USEABLE(edict_t* pentItem)
		{
			return reinterpret_cast<FuncIsUseable>(GET_VTABLE(pentItem)[g_vtblOffsets[VO_IsUseable]])(pentItem->pvPrivateData, DUMMY_VAL);
		}

	// virtual int AddToPlayer(CBasePlayer* pPlayer);
	//
		inline int ADD_TO_PLAYER(void* pvItem, void* pvPlayer)
		{
			return reinterpret_cast<FuncAddToPlayer>(g_CrowbarHooks[CrowbarHook_AddToPlayer].address)(pvItem, DUMMY_VAL, pvPlayer);
		}

		inline int ADD_TO_PLAYER(edict_t* pentItem, edict_t* pentPlayer)
		{
			return reinterpret_cast<FuncAddToPlayer>(GET_VTABLE(pentItem)[g_vtblOffsets[VO_AddToPlayer]])(pentItem->pvPrivateData, DUMMY_VAL, pentPlayer->pvPrivateData);
		}

	// virtual int ItemSlot(void);
	//
		inline int ITEM_SLOT(void* pvItem)
		{
			return reinterpret_cast<FuncItemSlot>(g_CrowbarHooks[CrowbarHook_ItemSlot].address)(pvItem, DUMMY_VAL);
		}

		inline int ITEM_SLOT(edict_t* pentItem)
		{
			return reinterpret_cast<FuncItemSlot>(GET_VTABLE(pentItem)[g_vtblOffsets[VO_ItemSlot]])(pentItem->pvPrivateData, DUMMY_VAL);
		}

	// virtual CBaseEntity* Respawn(void);
	// 
		inline void* RESPAWN(void* pvItem)
		{
			return reinterpret_cast<FuncRespawn>(g_CrowbarHooks[CrowbarHook_Respawn].address)(pvItem, DUMMY_VAL);
		}

		inline void* RESPAWN(edict_t* pentItem)
		{
			return reinterpret_cast<FuncRespawn>(GET_VTABLE(pentItem)[g_vtblOffsets[VO_Respawn]])(pentItem->pvPrivateData, DUMMY_VAL);
		}

	// virtual BOOL AddAmmo(CBaseEntity* pOther);
	//
		inline BOOL ADD_AMMO(void* pvAmmo, void* pvOther)
		{
			return reinterpret_cast<FuncAddAmmo>(g_RpgAddAmmo_Hook.address)(pvAmmo, DUMMY_VAL, pvOther);
		}

		inline BOOL ADD_AMMO(edict_t* pentAmmo, edict_t* pentOther)
		{
			return reinterpret_cast<FuncAddAmmo>(GET_VTABLE(pentAmmo)[g_vtblOffsets[VO_AddAmmo]])(pentAmmo->pvPrivateData, DUMMY_VAL, pentOther->pvPrivateData);
		}

	// void Precache(void);
	// 
		inline void WORLD_PRECACHE(void* pvEntity)
		{
			reinterpret_cast<FuncPrecache>(g_WorldPrecache_Hook.address)(pvEntity, DUMMY_VAL);
		}

	// void Spawn(void);
	// 
		inline void PLAYER_SPAWN(void* pvPlayer)
		{
			reinterpret_cast<FuncSpawn>(g_PlayerSpawn_Hook.address)(pvPlayer, DUMMY_VAL);
		}

#else

	typedef int		(*FuncGetItemInfo)		(void*, ItemInfo*);
	typedef BOOL	(*FuncCanDeploy)		(void*);
	typedef BOOL	(*FuncDeploy)			(void*);
	typedef BOOL	(*FuncCanHolster)		(void*);
	typedef void	(*FuncHolster)			(void*, int);
	typedef void	(*FuncItemPostFrame)	(void*);
	typedef BOOL	(*FuncIsUseable)		(void*);
	typedef int		(*FuncAddToPlayer)		(void*, void*);
	typedef int		(*FuncItemSlot)			(void*);
	typedef void*	(*FuncRespawn)			(void*);
	typedef BOOL	(*FuncAddAmmo)			(void*, void*);
	typedef void	(*FuncSpawn)			(void*);
	typedef void	(*FuncPrecache)		(void*);

	int		Weapon_GetItemInfo		(void* pvItem, ItemInfo* p);
	BOOL	Weapon_CanDeploy		(void* pvItem);
	BOOL	Weapon_Deploy			(void* pvItem);
	BOOL	Weapon_CanHolster		(void* pvItem);
	void	Weapon_Holster			(void* pvItem, int skiplocal);
	void	Weapon_ItemPostFrame	(void* pvItem);
	BOOL	Weapon_IsUseable		(void* pvItem);
	int		Weapon_AddToPlayer		(void* pvItem, void* pvPlayer);
	int		Weapon_ItemSlot			(void* pvItem);
	void*	Weapon_Respawn			(void* pvItem);
	BOOL	AmmoBox_AddAmmo			(void* pvAmmo, void* pvOther);
	int		Item_Block				(void* pvItem, void* pvOther);
	void	Player_Spawn			(void* pvPlayer);
	void	World_Precache			(void* pvEntity);

	// virtual int CBasePlayerItem::GetItemInfo(ItemInfo* p);
	//
		inline int GET_ITEM_INFO( void* pvItem, ItemInfo* p )
		{
			return reinterpret_cast<FuncGetItemInfo>(g_CrowbarHooks[CrowbarHook_GetItemInfo].address)(pvItem, p);
		}

		inline int GET_ITEM_INFO(edict_t* pentItem, ItemInfo* p)
		{
			return reinterpret_cast<FuncGetItemInfo>(GET_VTABLE(pentItem)[g_vtblOffsets[VO_GetItemInfo]])(pentItem->pvPrivateData, p);
		}

	// virtual BOOL CanDeploy(void)
	//
		inline BOOL CAN_DEPLOY(void* pvItem)
		{
			return reinterpret_cast<FuncCanDeploy>(g_CrowbarHooks[CrowbarHook_CanDeploy].address)(pvItem);
		}

		inline BOOL CAN_DEPLOY(edict_t* pentItem)
		{
			return reinterpret_cast<FuncCanDeploy>(GET_VTABLE(pentItem)[g_vtblOffsets[VO_CanDeploy]])(pentItem->pvPrivateData);
		}

	// virtual BOOL Deploy(void);
	//
		inline BOOL DEPLOY(void* pvItem)
		{
			return reinterpret_cast<FuncDeploy>(g_CrowbarHooks[CrowbarHook_Deploy].address)(pvItem);
		}

		inline BOOL DEPLOY(edict_t* pentItem)
		{
			return reinterpret_cast<FuncDeploy>(GET_VTABLE(pentItem)[g_vtblOffsets[VO_Deploy]])(pentItem->pvPrivateData);
		}

	// virtual BOOL CanHolster(void);
	//
		inline BOOL CAN_HOLSTER(void* pvItem)
		{
			return reinterpret_cast<FuncCanHolster>(g_CrowbarHooks[CrowbarHook_CanHolster].address)(pvItem);
		}

		inline BOOL CAN_HOLSTER(edict_t* pentItem)
		{
			return reinterpret_cast<FuncCanHolster>(GET_VTABLE(pentItem)[g_vtblOffsets[VO_CanHolster]])(pentItem->pvPrivateData);
		}

	// virtual void Holster(int skiplocal = 0);
	//
		inline void HOLSTER(void* pvItem)
		{
			reinterpret_cast<FuncHolster>(g_CrowbarHooks[CrowbarHook_Holster].address)(pvItem, 0);
		}

		inline void HOLSTER(edict_t* pentItem)
		{
			reinterpret_cast<FuncHolster>(GET_VTABLE(pentItem)[g_vtblOffsets[VO_Holster]])(pentItem->pvPrivateData, 0);
		}

	// virtual void ItemPostFrame(void);
	//
		inline void ITEM_POST_FRAME(void* pvItem)
		{
			reinterpret_cast<FuncItemPostFrame>(g_CrowbarHooks[CrowbarHook_ItemPostFrame].address)(pvItem);
		}

		inline void ITEM_POST_FRAME(edict_t* pentItem)
		{
			reinterpret_cast<FuncItemPostFrame>(GET_VTABLE(pentItem)[g_vtblOffsets[VO_ItemPostFrame]])(pentItem->pvPrivateData);
		}

	// virtual BOOL IsUseable(void);
	//
		inline BOOL IS_USEABLE(void* pvItem)
		{
			return reinterpret_cast<FuncIsUseable>(g_CrowbarHooks[CrowbarHook_IsUseable].address)(pvItem);
		}

		inline BOOL IS_USEABLE(edict_t* pentItem)
		{
			return reinterpret_cast<FuncIsUseable>(GET_VTABLE(pentItem)[g_vtblOffsets[VO_IsUseable]])(pentItem->pvPrivateData);
		}

	// virtual int AddToPlayer(CBasePlayer* pPlayer);
	//
		inline int ADD_TO_PLAYER(void* pvItem, void* pvPlayer)
		{
			return reinterpret_cast<FuncAddToPlayer>(g_CrowbarHooks[CrowbarHook_AddToPlayer].address)(pvItem, pvPlayer);
		}

		inline int ADD_TO_PLAYER(edict_t* pentItem, edict_t* pentPlayer)
		{
			return reinterpret_cast<FuncAddToPlayer>(GET_VTABLE(pentItem)[g_vtblOffsets[VO_AddToPlayer]])(pentItem->pvPrivateData, pentPlayer->pvPrivateData);
		}

	// virtual int ItemSlot(void);
	//
		inline int ITEM_SLOT(void* pvItem)
		{
			return reinterpret_cast<FuncItemSlot>(g_CrowbarHooks[CrowbarHook_ItemSlot].address)(pvItem);
		}

		inline int ITEM_SLOT(edict_t* pentItem)
		{
			return reinterpret_cast<FuncItemSlot>(GET_VTABLE(pentItem)[g_vtblOffsets[VO_ItemSlot]])(pentItem->pvPrivateData);
		}

	// virtual CBaseEntity* Respawn(void);
	// 
		inline void* RESPAWN(void* pvItem)
		{
			return reinterpret_cast<FuncRespawn>(g_CrowbarHooks[CrowbarHook_Respawn].address)(pvItem);
		}

		inline void* RESPAWN(edict_t* pentItem)
		{
			return reinterpret_cast<FuncRespawn>(GET_VTABLE(pentItem)[g_vtblOffsets[VO_Respawn]])(pentItem->pvPrivateData);
		}

	// virtual BOOL AddAmmo(CBaseEntity* pOther);
	//
		inline BOOL ADD_AMMO(void* pvAmmo, void* pvOther)
		{
			return reinterpret_cast<FuncAddAmmo>(g_RpgAddAmmo_Hook.address)(pvAmmo, pvOther);
		}

		inline BOOL ADD_AMMO(edict_t* pentAmmo, edict_t* pentOther)
		{
			return reinterpret_cast<FuncAddAmmo>(GET_VTABLE(pentAmmo)[g_vtblOffsets[VO_AddAmmo]])(pentAmmo->pvPrivateData, pentOther->pvPrivateData);
		}

	// void Precache(void);
	// 
		inline void WORLD_PRECACHE(void* pvEntity)
		{
			reinterpret_cast<FuncPrecache>(g_WorldPrecache_Hook.address)(pvEntity);
		}

	// void Spawn(void);
	// 
		inline void PLAYER_SPAWN(void* pvPlayer)
		{
			reinterpret_cast<FuncSpawn>(g_PlayerSpawn_Hook.address)(pvPlayer);
		}
	
#endif
	



























extern edict_t* Ammo_Spawn(const char* szName, Vector vecOrigin, Vector vecAngles);
extern edict_t* Weapon_Spawn(const char* szName, Vector vecOrigin, Vector vecAngles);


#ifdef _WIN32
extern void __fastcall CheatImpulseCommands_HookHandler(void *pPrivate, int i, int iImpulse);


inline int GET_DAMAGE_DECAL(edict_t* pEntity)
{
	return reinterpret_cast<int (__fastcall *)(void *, int, int)>((*((void***)((char*)pEntity->pvPrivateData)))[g_vtblOffsets[VO_DamageDecal]])(pEntity->pvPrivateData, 0, 0);
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
extern void CheatImpulseCommands_HookHandler(void *pPrivate, int iImpulse);

inline int GET_DAMAGE_DECAL(edict_t* pEntity)
{
	return reinterpret_cast<int (*)(void *, int)>((*((void***)(((char*)pEntity->pvPrivateData) + g_Base)))[g_vtblOffsets[VO_DamageDecal]])(pEntity->pvPrivateData, 0);
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
#endif

#endif // _HOOKS_H
