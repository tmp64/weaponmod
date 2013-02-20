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

#ifndef _HOOKS_H
#define _HOOKS_H

#include "wpnmod_config.h"
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

	typedef int		(*FuncGetAmmoIndex)			(const char*);
	typedef void	(*FuncRadiusDamage)			(Vector, entvars_s*, entvars_s*, float, float, int, int);
	typedef void	(*FuncClearMultiDamage)		(void);
	typedef void	(*FuncApplyMultiDamage)		(entvars_t*, entvars_t*);
	typedef void	(*FuncPrecacheOtherWeapon)	(const char*);

	void PrecacheOtherWeapon_HookHandler(const char *szClassname);

	// int CBasePlayer::GetAmmoIndex(const char *psz)
	//
		inline int GET_AMMO_INDEX(const char* ammoName)
		{
			return reinterpret_cast<FuncGetAmmoIndex>(g_dllFuncs[Func_GetAmmoIndex].address)(ammoName);
		}

	// void RadiusDamage( Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType )
	//
		void inline RADIUS_DAMAGE(Vector vecSrc, entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType)
		{
			return reinterpret_cast<FuncRadiusDamage>(g_dllFuncs[Func_RadiusDamage].address)(vecSrc, pevInflictor, pevAttacker, flDamage, flRadius, iClassIgnore, bitsDamageType);
		}
		
	// void ClearMultiDamage(void)
	//
		inline void CLEAR_MULTI_DAMAGE( void )
		{
			reinterpret_cast<FuncClearMultiDamage>(g_dllFuncs[Func_ClearMultiDamage].address)();
		}

	// void ApplyMultiDamage(entvars_t *pevInflictor, entvars_t *pevAttacker)
	//
		inline void APPLY_MULTI_DAMAGE(edict_t* pentInflictor, edict_t* pentAttacker)
		{
			reinterpret_cast<FuncApplyMultiDamage>(g_dllFuncs[Func_ApplyMultiDamage].address)(VARS(pentInflictor), VARS(pentAttacker));
		}

	// void UTIL_PrecacheOtherWeapon(const char *szClassname)
	//
		inline void PRECACHE_OTHER_WEAPON(const char *szClassname)
		{
			reinterpret_cast<FuncPrecacheOtherWeapon>(g_dllFuncs[Func_PrecacheOtherWeapon].address)(szClassname);
		}

#ifdef WIN32

	typedef void	(__fastcall	*FuncCheatImpulseCommands)	(void*, DUMMY, int);
	typedef void	(__fastcall	*FuncGiveNamedItem)			(void*, DUMMY, const char *);
	typedef void	(__fastcall *FuncSetAnimation)			(void*, DUMMY, int);

	void	__fastcall	CheatImpulseCommands_HookHandler	(void* pvPlayer, int DUMMY, int iImpulse);
	void	__fastcall	GiveNamedItem_HookHandler			(void* pvPlayer, int DUMMY, const char *szName);
	
	// void CBasePlayer::GiveNamedItem(const char *pszName)
	//
		inline void GIVE_NAMED_ITEM(void* pvPlayer, const char *szClassname)
		{
			reinterpret_cast<FuncGiveNamedItem>(g_dllFuncs[Func_GiveNamedItem].address)(pvPlayer, DUMMY_VAL, szClassname);
		}

	// void CBasePlayer::CheatImpulseCommands(int iImpulse)
	//
		inline void CHEAT_IMPULSE_COMMANDS(void* pvPlayer, int iImpulse)
		{
			reinterpret_cast<FuncCheatImpulseCommands>(g_dllFuncs[Func_CheatImpulseCommands].address)(pvPlayer, DUMMY_VAL, iImpulse);
		}

	// void SetAnimation(PLAYER_ANIM playerAnim);
	// 
		inline void SET_ANIMATION(edict_t* pentPlayer, int animation)
		{
			reinterpret_cast<FuncSetAnimation>(g_dllFuncs[Func_PlayerSetAnimation].address)(pentPlayer->pvPrivateData, DUMMY_VAL, animation);
		}

#else

	typedef void	(*FuncCheatImpulseCommands)	(void*, int);
	typedef void	(*FuncGiveNamedItem)		(void*, const char *);
	typedef void	(*FuncSetAnimation)			(void*, int);

	void	CheatImpulseCommands_HookHandler	(void* pvPlayer, int iImpulse);
	void	GiveNamedItem_HookHandler			(void* pvPlayer, const char *szName);
	
	// void CBasePlayer::GiveNamedItem(const char *pszName)
	//
		inline void GIVE_NAMED_ITEM(void* pvPlayer, const char *szClassname)
		{
			reinterpret_cast<FuncGiveNamedItem>(g_dllFuncs[Func_GiveNamedItem].address)(pvPlayer, szClassname);
		}

	// void CBasePlayer::CheatImpulseCommands(int iImpulse)
	//
		inline void CHEAT_IMPULSE_COMMANDS(void* pvPlayer, int iImpulse)
		{
			reinterpret_cast<FuncCheatImpulseCommands>(g_dllFuncs[Func_CheatImpulseCommands].address)(pvPlayer, iImpulse);
		}

	// void SetAnimation(PLAYER_ANIM playerAnim);
	// 
		inline void SET_ANIMATION(edict_t* pentPlayer, int animation)
		{
			reinterpret_cast<FuncSetAnimation>(g_dllFuncs[Func_PlayerSetAnimation].address)(pentPlayer->pvPrivateData, animation);
		}

#endif

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
	typedef int		(__fastcall *FuncDamageDecal)	(void*, DUMMY, int);
	typedef int		(__fastcall *FuncClassify)		(void*, DUMMY);
	typedef int		(__fastcall *FuncTraceAttack)	(void*, DUMMY, entvars_t *, float, Vector, TraceResult*, int);
	typedef int		(__fastcall *FuncTakeDamage)	(void*, DUMMY, entvars_t *, entvars_t *, float, int);

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

	// virtual int DamageDecal(CBaseEntity* pEntity, int bitsDamageType);
	//
		inline int GET_DAMAGE_DECAL(edict_t* pentEntity)
		{
			return reinterpret_cast<FuncDamageDecal>(GET_VTABLE(pentEntity)[g_vtblOffsets[VO_DamageDecal]])(pentEntity->pvPrivateData, DUMMY_VAL, 0);
		}

	// int Classify( void );
	// 
		inline int CLASSIFY(edict_t* pentEntity)
		{
			return reinterpret_cast<FuncClassify>(GET_VTABLE(pentEntity)[g_vtblOffsets[VO_Classify]])(pentEntity->pvPrivateData, DUMMY_VAL);
		}

	// virtual void TraceAttack(entvars_t* pevAttacker, float flDamage, Vector vecDir, TraceResult* ptr, int bitsDamageType);
	// 
		inline void TRACE_ATTACK(edict_t* pentVictim, edict_t* pentAttacker, float damage, Vector vecDir, TraceResult tr, int bitsDamageType)
		{
			reinterpret_cast<FuncTraceAttack>(GET_VTABLE(pentVictim)[g_vtblOffsets[VO_TraceAttack]])(pentVictim->pvPrivateData, DUMMY_VAL, VARS(pentAttacker), damage, vecDir, &tr, bitsDamageType);
		}

	// virtual int TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	//
		inline int TAKE_DAMAGE(edict_t* pentVictim, edict_t* pentInflictor, edict_t* pentAttacker, float damage, int bitsDamageType)
		{
			return reinterpret_cast<FuncTakeDamage>(GET_VTABLE(pentVictim)[g_vtblOffsets[VO_TakeDamage]])(pentVictim->pvPrivateData, DUMMY_VAL, VARS(pentInflictor), VARS(pentAttacker), damage, bitsDamageType);
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
	typedef void	(*FuncPrecache)			(void*);
	typedef int		(*FuncDamageDecal)		(void*, int);
	typedef int		(*FuncClassify)			(void*);
	typedef int		(*FuncTraceAttack)		(void*, entvars_t *, float, Vector, TraceResult*, int);
	typedef int		(*FuncTakeDamage)		(void*, entvars_t *, entvars_t *, float, int);

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

	// virtual int DamageDecal(CBaseEntity* pEntity, int bitsDamageType);
	//
		inline int GET_DAMAGE_DECAL(edict_t* pentEntity)
		{
			return reinterpret_cast<FuncDamageDecal>(GET_VTABLE(pentEntity)[g_vtblOffsets[VO_DamageDecal]])(pentEntity->pvPrivateData, 0);
		}
	
	// int Classify( void );
	// 
		inline int CLASSIFY(edict_t* pentEntity)
		{
			return reinterpret_cast<FuncClassify>(GET_VTABLE(pentEntity)[g_vtblOffsets[VO_Classify]])(pentEntity->pvPrivateData);
		}

	// virtual void TraceAttack(entvars_t* pevAttacker, float flDamage, Vector vecDir, TraceResult* ptr, int bitsDamageType);
	// 
		inline void TRACE_ATTACK(edict_t* pentVictim, edict_t* pentAttacker, float damage, Vector vecDir, TraceResult tr, int bitsDamageType)
		{
			reinterpret_cast<FuncTraceAttack>(GET_VTABLE(pentVictim)[g_vtblOffsets[VO_TraceAttack]])(pentVictim->pvPrivateData, VARS(pentAttacker), damage, vecDir, &tr, bitsDamageType);
		}

	// virtual int TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	//
		inline int TAKE_DAMAGE(edict_t* pentVictim, edict_t* pentInflictor, edict_t* pentAttacker, float damage, int bitsDamageType)
		{
			return reinterpret_cast<FuncTakeDamage>(GET_VTABLE(pentVictim)[g_vtblOffsets[VO_TakeDamage]])(pentVictim->pvPrivateData, VARS(pentInflictor), VARS(pentAttacker), damage, bitsDamageType);
		}

#endif
	
#endif // _HOOKS_H
