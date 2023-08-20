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

#ifndef _HOOKS_H
#define _HOOKS_H

#include "wpnmod_hooker.h"
#include "wpnmod_memory.h"
#include "wpnmod_config.h"
#include "wpnmod_utils.h"
#include "wpnmod_vtable.h"

void Hooks_InitReferenceEntities();

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

	typedef void				(*ENTITY_FN)					(entvars_t *);
	typedef void				(*DISPATCHFUNCTION)				(struct entvars_s *, void *);

	typedef void				(*FuncClearMultiDamage)			(void);
	typedef void				(*FuncApplyMultiDamage)			(entvars_t *, entvars_t *);
	typedef void				(*FuncWorldPrecache)			(void);
	typedef DISPATCHFUNCTION	(*FuncGetDispatch)				(char *);

	DISPATCHFUNCTION	GetDispatch_HookHandler			(char *pname);
	qboolean			CallGameEntity_HookHandler		(plid_t plid, const char *entStr, entvars_t *pev);
	void				WorldPrecache_HookHandler		(void);

	// void ClearMultiDamage(void)
	//
		inline void CLEAR_MULTI_DAMAGE(void)
		{
			reinterpret_cast<FuncClearMultiDamage>(g_Memory.m_pClearMultiDamage)();
		}

	// void ApplyMultiDamage(entvars_t *pevInflictor, entvars_t *pevAttacker)
	//
		inline void APPLY_MULTI_DAMAGE(edict_t* pentInflictor, edict_t* pentAttacker)
		{
			reinterpret_cast<FuncApplyMultiDamage>(g_Memory.m_pApplyMultiDamage)(VARS(pentInflictor), VARS(pentAttacker));
		}

	// void W_Precache(void)
	//
		inline void WORLD_PRECACHE(void)
		{
			reinterpret_cast<FuncWorldPrecache>(g_fh_WorldPrecache.address)();
		}

	// DISPATCHFUNCTION GetDispatch(char *pname)
	//
		inline DISPATCHFUNCTION ENGINE_GET_DISPATCH(char *pname)
		{
			return reinterpret_cast<FuncGetDispatch>(g_fh_GetDispatch.address)(pname);
		}

#ifdef WIN32

	typedef void	(__fastcall *FuncSetAnimation)				(void*, DUMMY, int);
	typedef void	(__fastcall *FuncFallthink)					(void*);
	typedef void	(__fastcall *FuncAmmoSpawn)					(void*);
	typedef void	(__fastcall *FuncItemSpawn)					(void*);

	void	__fastcall	GiveNamedItem_HookHandler				(void* pvPlayer, int DUMMY, const char* szName);
	void	__fastcall	CBasePlayerItem_FallThink_HookHandler	(void *pvItem);
	void	__fastcall	CBasePlayerAmmoSpawn_HookHandler		(void *pvItem);
	void	__fastcall	CItemSpawn_HookHandler					(void *pvItem);

	// void SetAnimation(PLAYER_ANIM playerAnim);
	//
		inline void SET_ANIMATION(edict_t* pentPlayer, int animation)
		{
			reinterpret_cast<FuncSetAnimation>(g_Memory.m_pPlayerSetAnimation)(pentPlayer->pvPrivateData, DUMMY_VAL, animation);
		}

#else

	typedef void	(*FuncSetAnimation)			(void*, int);
	typedef void	(*FuncFallthink)			(void*);
	typedef void	(*FuncAmmoSpawn)			(void*);
	typedef void	(*FuncItemSpawn)			(void*);

	void	GiveNamedItem_HookHandler				(void* pvPlayer, const char *szName);
	void	CBasePlayerItem_FallThink_HookHandler	(void *pvItem);
	void	CBasePlayerAmmoSpawn_HookHandler		(void *pvItem);
	void	CItemSpawn_HookHandler					(void *pvItem);

	// void SetAnimation(PLAYER_ANIM playerAnim);
	//
		inline void SET_ANIMATION(edict_t* pentPlayer, int animation)
		{
			reinterpret_cast<FuncSetAnimation>(g_Memory.m_pPlayerSetAnimation)(pentPlayer->pvPrivateData, animation);
		}

#endif

	// void CBasePlayerItem::FallThink(void);
	//
		inline void ITEM_FALLTHINK(void* pvItem)
		{
			reinterpret_cast<FuncFallthink>(g_fh_FallThink.address)(pvItem);
		}

	// void CBasePlayerAmmo::Spawn(void);
	//
		inline void PLAYER_AMMO_SPAWN(void* pvItem)
		{
			reinterpret_cast<FuncAmmoSpawn>(g_fh_AmmoSpawn.address)(pvItem);
		}

	// void CItem::Spawn(void);
	//
		inline void GAME_ITEM_SPAWN(void* pvItem)
		{
			reinterpret_cast<FuncItemSpawn>(g_fh_ItemSpawn.address)(pvItem);
		}

//
// VIRTUAL FUNCTIONS
//

	#define VHOOK(classname, offset, call)				\
	{													\
		classname, offset, (void*)call, NULL, NULL,		\
	}													\

	#define VHOOK_WEAPON_REF(call)										\
	{																	\
		nullptr, VO_##call, (void*)Weapon_##call, NULL, NULL,			\
	}																	\

	#define VHOOK_AMMOBOX_REF(call)											\
	{																		\
		nullptr, VO_##call, (void*)AmmoBox_##call, NULL, NULL,				\
	}																		\

	enum AmmoBoxRefHooks
	{
		AmmoBoxRefHook_Spawn,
		AmmoBoxRefHook_AddAmmo,

		AmmoBoxRefHook_End
	};

	enum WeaponRefHooks
	{
		WeaponRefHook_Spawn,
		WeaponRefHook_Respawn,
		WeaponRefHook_AddToPlayer,
		WeaponRefHook_CanDeploy,
		WeaponRefHook_Deploy,
		WeaponRefHook_CanHolster,
		WeaponRefHook_Holster,
		WeaponRefHook_ItemPostFrame,
		WeaponRefHook_ItemSlot,
		WeaponRefHook_IsUseable,

		WeaponRefHook_End
	};

	extern VirtualHookData g_PlayerSpawn_Hook;
	extern VirtualHookData g_PlayerPostThink_Hook;
	extern VirtualHookData g_CrowbarHooks[WeaponRefHook_End];
	extern VirtualHookData g_AmmoBoxRefHooks[AmmoBoxRefHook_End];

#ifdef WIN32

	typedef BOOL	(__fastcall *FuncCanDeploy)			(void*, DUMMY);
	typedef BOOL	(__fastcall *FuncDeploy)			(void*, DUMMY);
	typedef BOOL	(__fastcall *FuncCanHolster)		(void*, DUMMY);
	typedef void	(__fastcall *FuncHolster)			(void*, DUMMY, int);
	typedef void	(__fastcall *FuncItemPostFrame)		(void*, DUMMY);
	typedef BOOL	(__fastcall *FuncIsUseable)			(void*, DUMMY);
	typedef int		(__fastcall *FuncAddToPlayer)		(void*, DUMMY, void*);
	typedef int		(__fastcall *FuncItemSlot)			(void*, DUMMY);
	typedef void*	(__fastcall *FuncRespawn)			(void*, DUMMY);
	typedef BOOL	(__fastcall *FuncAddAmmo)			(void*, DUMMY, void*);
	typedef void	(__fastcall *FuncSpawn)				(void*, DUMMY);
	typedef void	(__fastcall *FuncPlayerPostThink)	(void*, DUMMY);
	typedef void	(__fastcall *FuncPrecache)			(void*, DUMMY);
	typedef int		(__fastcall *FuncDamageDecal)		(void*, DUMMY, int);
	typedef int		(__fastcall *FuncClassify)			(void*, DUMMY);
	typedef int		(__fastcall *FuncTraceAttack)		(void*, DUMMY, entvars_t *, float, Vector, TraceResult*, int);
	typedef int		(__fastcall *FuncTakeDamage)		(void*, DUMMY, entvars_t *, entvars_t *, float, int);

	void	__fastcall AmmoBox_Spawn		(void* pvItem);
	BOOL	__fastcall AmmoBox_AddAmmo		(void* pvAmmo, DUMMY, void* pvOther);
	void	__fastcall Weapon_Spawn			(void* pvItem);
	BOOL	__fastcall Weapon_CanDeploy		(void* pvItem);
	BOOL	__fastcall Weapon_Deploy		(void* pvItem);
	BOOL	__fastcall Weapon_CanHolster	(void* pvItem);
	void	__fastcall Weapon_Holster		(void* pvItem, DUMMY, int skiplocal);
	void	__fastcall Weapon_ItemPostFrame	(void* pvItem);
	BOOL	__fastcall Weapon_IsUseable		(void* pvItem);
	int		__fastcall Weapon_AddToPlayer	(void* pvItem, DUMMY, void* pvPlayer);
	int		__fastcall Weapon_ItemSlot		(void* pvItem);
	void*	__fastcall Weapon_Respawn		(void* pvItem);
	int		__fastcall Item_Block			(void* pvItem, int DUMMY, void* pvOther);
	void	__fastcall Player_Spawn			(void* pvPlayer);
	void	__fastcall Player_PostThink		(void* pvPlayer);

	// void Spawn(void);
	// 
		inline void AMMOBOX_REF_SPAWN(void* pvAmmoBox)
		{
			reinterpret_cast<FuncSpawn>(g_AmmoBoxRefHooks[AmmoBoxRefHook_Spawn].address)(pvAmmoBox, DUMMY_VAL);
		}

	// virtual BOOL AddAmmo(CBaseEntity* pOther);
	//
		inline BOOL AMMOBOX_REF_ADD_AMMO(void* pvAmmo, void* pvOther)
		{
			return reinterpret_cast<FuncAddAmmo>(g_AmmoBoxRefHooks[AmmoBoxRefHook_AddAmmo].address)(pvAmmo, DUMMY_VAL, pvOther);
		}

	// virtual BOOL CanDeploy(void)
	//
		inline BOOL WEAPON_REF_CAN_DEPLOY(void* pvItem)
		{
			return reinterpret_cast<FuncCanDeploy>(g_CrowbarHooks[WeaponRefHook_CanDeploy].address)(pvItem, DUMMY_VAL);
		}

		inline BOOL CAN_DEPLOY(edict_t* pentItem)
		{
			return GetEntityVTableFunc<FuncCanDeploy>(pentItem, VO_CanDeploy)(pentItem->pvPrivateData, DUMMY_VAL);
		}

	// virtual BOOL Deploy(void);
	//
		inline BOOL WEAPON_REF_DEPLOY(void* pvItem)
		{
			return reinterpret_cast<FuncDeploy>(g_CrowbarHooks[WeaponRefHook_Deploy].address)(pvItem, DUMMY_VAL);
		}

		inline BOOL DEPLOY(edict_t* pentItem)
		{
				return GetEntityVTableFunc<FuncDeploy>(pentItem, VO_Deploy)(pentItem->pvPrivateData, DUMMY_VAL);
		}

	// virtual BOOL CanHolster(void);
	//
		inline BOOL WEAPON_REF_CAN_HOLSTER(void* pvItem)
		{
			return reinterpret_cast<FuncCanHolster>(g_CrowbarHooks[WeaponRefHook_CanHolster].address)(pvItem, DUMMY_VAL);
		}

		inline BOOL CAN_HOLSTER( edict_t* pentItem )
		{
			return GetEntityVTableFunc<FuncCanHolster>(pentItem, VO_CanHolster)(pentItem->pvPrivateData, DUMMY_VAL);
		}

	// virtual void Holster(int skiplocal = 0);
	//
		inline void WEAPON_REF_HOLSTER(void* pvItem)
		{
			reinterpret_cast<FuncHolster>(g_CrowbarHooks[WeaponRefHook_Holster].address)(pvItem, DUMMY_VAL, 0);
		}

		inline void HOLSTER(edict_t* pentItem)
		{
			GetEntityVTableFunc<FuncHolster>(pentItem, VO_Holster)(pentItem->pvPrivateData, DUMMY_VAL, 0);
		}

	// virtual void ItemPostFrame(void);
	//
		inline void WEAPON_REF_ITEM_POST_FRAME(void* pvItem)
		{
			reinterpret_cast<FuncItemPostFrame>(g_CrowbarHooks[WeaponRefHook_ItemPostFrame].address)(pvItem, DUMMY_VAL);
		}

	// virtual BOOL IsUseable(void);
	//
		inline BOOL WEAPON_REF_IS_USEABLE(void* pvItem)
		{
			return reinterpret_cast<FuncIsUseable>(g_CrowbarHooks[WeaponRefHook_IsUseable].address)(pvItem, DUMMY_VAL);
		}

		inline BOOL IS_USEABLE(edict_t* pentItem)
		{
			return GetEntityVTableFunc<FuncIsUseable>(pentItem, VO_IsUseable)(pentItem->pvPrivateData, DUMMY_VAL);
		}

	// virtual int AddToPlayer(CBasePlayer* pPlayer);
	//
		inline int WEAPON_REF_ADD_TO_PLAYER(void* pvItem, void* pvPlayer)
		{
			return reinterpret_cast<FuncAddToPlayer>(g_CrowbarHooks[WeaponRefHook_AddToPlayer].address)(pvItem, DUMMY_VAL, pvPlayer);
		}

		inline int ADD_TO_PLAYER(edict_t* pentItem, edict_t* pentPlayer)
		{
			return GetEntityVTableFunc<FuncAddToPlayer>(pentItem, VO_AddToPlayer)(pentItem->pvPrivateData, DUMMY_VAL, pentPlayer->pvPrivateData);
		}

	// virtual int ItemSlot(void);
	//
		inline int WEAPON_REF_ITEM_SLOT(void* pvItem)
		{
			return reinterpret_cast<FuncItemSlot>(g_CrowbarHooks[WeaponRefHook_ItemSlot].address)(pvItem, DUMMY_VAL);
		}

		inline int ITEM_SLOT(edict_t* pentItem)
		{
			return GetEntityVTableFunc<FuncItemSlot>(pentItem, VO_ItemSlot)(pentItem->pvPrivateData, DUMMY_VAL);
		}

	// virtual CBaseEntity* Respawn(void);
	//
		inline void* WEAPON_REF_RESPAWN(void* pvItem)
		{
			return reinterpret_cast<FuncRespawn>(g_CrowbarHooks[WeaponRefHook_Respawn].address)(pvItem, DUMMY_VAL);
		}

		inline void* RESPAWN(edict_t* pentItem)
		{
			return GetEntityVTableFunc<FuncRespawn>(pentItem, VO_Respawn)(pentItem->pvPrivateData, DUMMY_VAL);
		}

	// void Spawn(void);
	// 
		inline void WEAPON_REF_SPAWN(void* pvItem)
		{
			reinterpret_cast<FuncSpawn>(g_CrowbarHooks[WeaponRefHook_Spawn].address)(pvItem, DUMMY_VAL);
		}

	// void Spawn(void);
	// 
		inline void PLAYER_SPAWN(void* pvPlayer)
		{
			reinterpret_cast<FuncSpawn>(g_PlayerSpawn_Hook.address)(pvPlayer, DUMMY_VAL);
		}

	// void PostThink();
	//
		inline void PLAYER_POST_THINK(void* pvPlayer)
		{
			reinterpret_cast<FuncPlayerPostThink>(g_PlayerPostThink_Hook.address)(pvPlayer, DUMMY_VAL);
		}

	// virtual int DamageDecal(CBaseEntity* pEntity, int bitsDamageType);
	//
		inline int GET_DAMAGE_DECAL(edict_t* pentEntity)
		{
			return GetEntityVTableFunc<FuncDamageDecal>(pentEntity, VO_DamageDecal)(pentEntity->pvPrivateData, DUMMY_VAL, 0);
		}

	// int Classify( void );
	//
		inline int CLASSIFY(edict_t* pentEntity)
		{
			return GetEntityVTableFunc<FuncClassify>(pentEntity, VO_Classify)(pentEntity->pvPrivateData, DUMMY_VAL);
		}

	// virtual void TraceAttack(entvars_t* pevAttacker, float flDamage, Vector vecDir, TraceResult* ptr, int bitsDamageType);
	//
		inline void TRACE_ATTACK(edict_t* pentVictim, edict_t* pentAttacker, float damage, Vector vecDir, TraceResult tr, int bitsDamageType)
		{
			GetEntityVTableFunc<FuncTraceAttack>(pentVictim, VO_TraceAttack)(pentVictim->pvPrivateData, DUMMY_VAL, VARS(pentAttacker), damage, vecDir, &tr, bitsDamageType);
		}

	// virtual int TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	//
		inline int TAKE_DAMAGE(edict_t* pentVictim, edict_t* pentInflictor, edict_t* pentAttacker, float damage, int bitsDamageType)
		{
			return GetEntityVTableFunc<FuncTakeDamage>(pentVictim, VO_TakeDamage)(pentVictim->pvPrivateData, DUMMY_VAL, VARS(pentInflictor), VARS(pentAttacker), damage, bitsDamageType);
		}

#else

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
	typedef void	(*FuncPlayerPostThink)	(void*);
	typedef void	(*FuncPrecache)			(void*);
	typedef int		(*FuncDamageDecal)		(void*, int);
	typedef int		(*FuncClassify)			(void*);
	typedef int		(*FuncTraceAttack)		(void*, entvars_t *, float, Vector, TraceResult*, int);
	typedef int		(*FuncTakeDamage)		(void*, entvars_t *, entvars_t *, float, int);

	void	AmmoBox_Spawn			(void* pvItem);
	BOOL	AmmoBox_AddAmmo			(void* pvAmmo, void* pvOther);
	void	Weapon_Spawn			(void* pvItem);
	BOOL	Weapon_CanDeploy		(void* pvItem);
	BOOL	Weapon_Deploy			(void* pvItem);
	BOOL	Weapon_CanHolster		(void* pvItem);
	void	Weapon_Holster			(void* pvItem, int skiplocal);
	void	Weapon_ItemPostFrame	(void* pvItem);
	BOOL	Weapon_IsUseable		(void* pvItem);
	int		Weapon_AddToPlayer		(void* pvItem, void* pvPlayer);
	int		Weapon_ItemSlot			(void* pvItem);
	void*	Weapon_Respawn			(void* pvItem);
	int		Item_Block				(void* pvItem, void* pvOther);
	void	Player_Spawn			(void* pvPlayer);
	void	Player_PostThink		(void* pvPlayer);

	// void Spawn(void);
	// 
		inline void AMMOBOX_REF_SPAWN(void* pvAmmoBox)
		{
			reinterpret_cast<FuncSpawn>(g_AmmoBoxRefHooks[AmmoBoxRefHook_Spawn].address)(pvAmmoBox);
		}

	// virtual BOOL AddAmmo(CBaseEntity* pOther);
	//
		inline BOOL AMMOBOX_REF_ADD_AMMO(void* pvAmmo, void* pvOther)
		{
			return reinterpret_cast<FuncAddAmmo>(g_AmmoBoxRefHooks[AmmoBoxRefHook_AddAmmo].address)(pvAmmo, pvOther);
		}

	// virtual BOOL CanDeploy(void)
	//
		inline BOOL WEAPON_REF_CAN_DEPLOY(void* pvItem)
		{
			return reinterpret_cast<FuncCanDeploy>(g_CrowbarHooks[WeaponRefHook_CanDeploy].address)(pvItem);
		}

		inline BOOL CAN_DEPLOY(edict_t* pentItem)
		{
			return GetEntityVTableFunc<FuncCanDeploy>(pentItem, VO_CanDeploy)(pentItem->pvPrivateData);
		}

	// virtual BOOL Deploy(void);
	//
		inline BOOL WEAPON_REF_DEPLOY(void* pvItem)
		{
			return reinterpret_cast<FuncDeploy>(g_CrowbarHooks[WeaponRefHook_Deploy].address)(pvItem);
		}

		inline BOOL DEPLOY(edict_t* pentItem)
		{
			return GetEntityVTableFunc<FuncDeploy>(pentItem, VO_Deploy)(pentItem->pvPrivateData);
		}

	// virtual BOOL CanHolster(void);
	//
		inline BOOL WEAPON_REF_CAN_HOLSTER(void* pvItem)
		{
			return reinterpret_cast<FuncCanHolster>(g_CrowbarHooks[WeaponRefHook_CanHolster].address)(pvItem);
		}

		inline BOOL CAN_HOLSTER(edict_t* pentItem)
		{
			return GetEntityVTableFunc<FuncCanHolster>(pentItem, VO_CanHolster)(pentItem->pvPrivateData);
		}

	// virtual void Holster(int skiplocal = 0);
	//
		inline void WEAPON_REF_HOLSTER(void* pvItem)
		{
			reinterpret_cast<FuncHolster>(g_CrowbarHooks[WeaponRefHook_Holster].address)(pvItem, 0);
		}

		inline void HOLSTER(edict_t* pentItem)
		{
			GetEntityVTableFunc<FuncHolster>(pentItem, VO_Holster)(pentItem->pvPrivateData, 0);
		}

	// virtual void ItemPostFrame(void);
	//
		inline void WEAPON_REF_ITEM_POST_FRAME(void* pvItem)
		{
			reinterpret_cast<FuncItemPostFrame>(g_CrowbarHooks[WeaponRefHook_ItemPostFrame].address)(pvItem);
		}

	// virtual BOOL IsUseable(void);
	//
		inline BOOL WEAPON_REF_IS_USEABLE(void* pvItem)
		{
			return reinterpret_cast<FuncIsUseable>(g_CrowbarHooks[WeaponRefHook_IsUseable].address)(pvItem);
		}

		inline BOOL IS_USEABLE(edict_t* pentItem)
		{
			return GetEntityVTableFunc<FuncIsUseable>(pentItem, VO_IsUseable)(pentItem->pvPrivateData);
		}

	// virtual int AddToPlayer(CBasePlayer* pPlayer);
	//
		inline int WEAPON_REF_ADD_TO_PLAYER(void* pvItem, void* pvPlayer)
		{
			return reinterpret_cast<FuncAddToPlayer>(g_CrowbarHooks[WeaponRefHook_AddToPlayer].address)(pvItem, pvPlayer);
		}

		inline int ADD_TO_PLAYER(edict_t* pentItem, edict_t* pentPlayer)
		{
			return GetEntityVTableFunc<FuncAddToPlayer>(pentItem, VO_AddToPlayer)(pentItem->pvPrivateData, pentPlayer->pvPrivateData);
		}

	// virtual int ItemSlot(void);
	//
		inline int WEAPON_REF_ITEM_SLOT(void* pvItem)
		{
			return reinterpret_cast<FuncItemSlot>(g_CrowbarHooks[WeaponRefHook_ItemSlot].address)(pvItem);
		}

		inline int ITEM_SLOT(edict_t* pentItem)
		{
			return GetEntityVTableFunc<FuncItemSlot>(pentItem, VO_ItemSlot)(pentItem->pvPrivateData);
		}

	// virtual CBaseEntity* Respawn(void);
	//
		inline void* WEAPON_REF_RESPAWN(void* pvItem)
		{
			return reinterpret_cast<FuncRespawn>(g_CrowbarHooks[WeaponRefHook_Respawn].address)(pvItem);
		}

		inline void* RESPAWN(edict_t* pentItem)
		{
			return GetEntityVTableFunc<FuncRespawn>(pentItem, VO_Respawn)(pentItem->pvPrivateData);
		}

	// void Spawn(void);
	// 
		inline void WEAPON_REF_SPAWN(void* pvItem)
		{
			reinterpret_cast<FuncSpawn>(g_CrowbarHooks[WeaponRefHook_Spawn].address)(pvItem);
		}

	// void Spawn(void);
	//
		inline void PLAYER_SPAWN(void* pvPlayer)
		{
			reinterpret_cast<FuncSpawn>(g_PlayerSpawn_Hook.address)(pvPlayer);
		}

	// void PostThink();
	//
		inline void PLAYER_POST_THINK(void* pvPlayer)
		{
			reinterpret_cast<FuncPlayerPostThink>(g_PlayerPostThink_Hook.address)(pvPlayer);
		}

	// virtual int DamageDecal(CBaseEntity* pEntity, int bitsDamageType);
	//
		inline int GET_DAMAGE_DECAL(edict_t* pentEntity)
		{
			return GetEntityVTableFunc<FuncDamageDecal>(pentEntity, VO_DamageDecal)(pentEntity->pvPrivateData, 0);
		}
	
	// int Classify( void );
	//
		inline int CLASSIFY(edict_t* pentEntity)
		{
			return GetEntityVTableFunc<FuncClassify>(pentEntity, VO_Classify)(pentEntity->pvPrivateData);
		}

	// virtual void TraceAttack(entvars_t* pevAttacker, float flDamage, Vector vecDir, TraceResult* ptr, int bitsDamageType);
	//
		inline void TRACE_ATTACK(edict_t* pentVictim, edict_t* pentAttacker, float damage, Vector vecDir, TraceResult tr, int bitsDamageType)
		{
			GetEntityVTableFunc<FuncTraceAttack>(pentVictim, VO_TraceAttack)(pentVictim->pvPrivateData, VARS(pentAttacker), damage, vecDir, &tr, bitsDamageType);
		}

	// virtual int TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	//
		inline int TAKE_DAMAGE(edict_t* pentVictim, edict_t* pentInflictor, edict_t* pentAttacker, float damage, int bitsDamageType)
		{
			return GetEntityVTableFunc<FuncTakeDamage>(pentVictim, VO_TakeDamage)(pentVictim->pvPrivateData, VARS(pentInflictor), VARS(pentAttacker), damage, bitsDamageType);
		}

#endif
	
#endif // _HOOKS_H
