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

#include <cassert>
#include "wpnmod_entity.h"
#include "wpnmod_parse.h"
#include "wpnmod_utils.h"
#include "wpnmod_hooks.h"
#include "wpnmod_items.h"
#include "entity_state.h"


VirtualHookData g_AmmoBoxRefHooks[AmmoBoxRefHook_End] =
{
	VHOOK_AMMOBOX_REF(Spawn),
	VHOOK_AMMOBOX_REF(AddAmmo),
};

VirtualHookData g_CrowbarHooks[WeaponRefHook_End] =
{
	VHOOK_WEAPON_REF(Spawn),
	VHOOK_WEAPON_REF(Respawn),
	VHOOK_WEAPON_REF(AddToPlayer),
	VHOOK_WEAPON_REF(CanDeploy),
	VHOOK_WEAPON_REF(Deploy),
	VHOOK_WEAPON_REF(CanHolster),
	VHOOK_WEAPON_REF(Holster),
	VHOOK_WEAPON_REF(ItemPostFrame),
	VHOOK_WEAPON_REF(ItemSlot),
	VHOOK_WEAPON_REF(IsUseable)
};

VirtualHookData g_PlayerSpawn_Hook		= VHOOK("player",			VO_Spawn,				Player_Spawn);
VirtualHookData g_PlayerPostThink_Hook	= VHOOK("player",			VO_Player_PostThink,	Player_PostThink);


void Hooks_InitReferenceEntities()
{
	auto fnInitArray = [](const char* name, auto& arr)
	{
		assert(name);

		for (VirtualHookData& i : arr)
			i.classname = name;
	};
	
	fnInitArray(gWeaponReference.c_str(), g_CrowbarHooks);
	fnInitArray(gAmmoBoxReference.c_str(), g_AmmoBoxRefHooks);
}

#ifdef WIN32
	void __fastcall AmmoBox_Spawn(void* pvItem)
#else
	void AmmoBox_Spawn(void* pvItem)
#endif
{
	AMMOBOX_REF_SPAWN(pvItem);

	edict_t* pAmmobox = PrivateToEdict(pvItem);

	if (IsValidPev(pAmmobox))
	{
		int iId = AMMOBOX_GET_ID(STRING(pAmmobox->v.classname));

		if (iId)
		{
			pAmmobox->v.classname = MAKE_STRING(AMMOBOX_GET_NAME(iId));

			SET_ORIGIN(pAmmobox, pAmmobox->v.origin);
			AMMOBOX_FORWARD_EXECUTE(iId, Fwd_Ammo_Spawn, pAmmobox, NULL);
		}
	}
}


#ifdef WIN32
	BOOL __fastcall AmmoBox_AddAmmo(void* pvAmmo, DUMMY, void* pvOther)
#else
	BOOL AmmoBox_AddAmmo(void* pvAmmo, void* pvOther)
#endif
{
	edict_t* pAmmobox = PrivateToEdict(pvAmmo);
	edict_t* pOther = PrivateToEdict(pvOther);

	if (!IsValidPev(pAmmobox) || !IsValidPev(pOther))
	{
		return FALSE;
	}

	if (!stricmp(STRING(pAmmobox->v.classname), gAmmoBoxReference.c_str()))
	{
		if (g_Config.IsItemBlocked(gAmmoBoxReference.c_str()))
		{
			UTIL_RemoveEntity(pAmmobox);
			return FALSE;
		}

		return AMMOBOX_REF_ADD_AMMO(pvAmmo, pvOther);
	}

	int iId = AMMOBOX_GET_ID(STRING(pAmmobox->v.classname));

	if (iId)
	{
		return AMMOBOX_FORWARD_EXECUTE(iId, Fwd_Ammo_AddAmmo, pAmmobox, pOther);
	}

	return FALSE;
}


#ifdef WIN32
	void __fastcall Weapon_Spawn(void* pvItem)
#else
	void Weapon_Spawn(void* pvItem)
#endif
{
	WEAPON_REF_SPAWN(pvItem);
	
	edict_t* pWeapon = PrivateToEdict(pvItem);

	if (!IsValidPev(pWeapon))
	{
		return;
	}

	int iId = WEAPON_GET_ID(STRING(pWeapon->v.classname));

	if (WEAPON_IS_CUSTOM(iId))
	{
		SetPrivateInt(pWeapon, pvData_iId, iId);

		if (WEAPON_GET_MAX_CLIP(iId) != -1)
		{
			SetPrivateInt(pWeapon, pvData_iClip, 0);
		}

		pWeapon->v.classname = MAKE_STRING(WEAPON_GET_NAME(iId));

		SET_ORIGIN(pWeapon, pWeapon->v.origin);
		WEAPON_FORWARD_EXECUTE(iId, Fwd_Wpn_Spawn, pWeapon, NULL);
	}
}


#ifdef WIN32
	BOOL __fastcall Weapon_CanDeploy(void* pvItem)
#else
	BOOL Weapon_CanDeploy(void* pvItem)
#endif
{
	edict_t* pWeapon = PrivateToEdict(pvItem);

	if (!IsValidPev(pWeapon))
	{
		return FALSE;
	}

	int iId = GetPrivateInt(pWeapon, pvData_iId);

	if (WEAPON_IS_DEFAULT(iId) || !WEAPON_FORWARD_IS_EXIST(iId, Fwd_Wpn_CanDeploy))
	{
		return WEAPON_REF_CAN_DEPLOY(pvItem);
	}

	edict_t* pPlayer = GetPrivateCbase(pWeapon, pvData_pPlayer);

	if (IsValidPev(pPlayer))
	{
		return WEAPON_FORWARD_EXECUTE(iId, Fwd_Wpn_CanDeploy, pWeapon, pPlayer);
	}

	return FALSE;
}


#ifdef WIN32
	BOOL __fastcall Weapon_Deploy(void* pvItem)
#else
	BOOL Weapon_Deploy(void* pvItem)
#endif
{
	edict_t* pWeapon = PrivateToEdict(pvItem);

	if (!IsValidPev(pWeapon))
	{
		return FALSE;
	}

	int iId = GetPrivateInt(pWeapon, pvData_iId);

	if (WEAPON_IS_DEFAULT(iId))
	{
		return WEAPON_REF_DEPLOY(pvItem);
	}

	edict_t* pPlayer = GetPrivateCbase(pWeapon, pvData_pPlayer);

	if (IsValidPev(pPlayer))
	{
		SET_CLIENT_KEYVALUE(ENTINDEX(pPlayer), GET_INFOKEYBUFFER(pPlayer), "cl_lw", "0");
		return WEAPON_FORWARD_EXECUTE(iId, Fwd_Wpn_Deploy, pWeapon, pPlayer);
	}

	return FALSE;
}


#ifdef WIN32
	void __fastcall Weapon_ItemPostFrame(void* pvItem)
#else
	void Weapon_ItemPostFrame(void* pvItem)
#endif
{
	static int iId;
	static int iClip;
	static int iInReload;

	static int iAmmoPrimary;
	static int iAmmoSecondary;

	static float flNextAttack;
	static float flNextPrimaryAttack;
	static float flNextSecondaryAttack;

	static edict_t* pWeapon;
	static edict_t* pPlayer;

	pWeapon = PrivateToEdict(pvItem);

	if (!IsValidPev(pWeapon))
	{
		return;
	}

	iId = GetPrivateInt(pWeapon, pvData_iId);

	if (WEAPON_IS_DEFAULT(iId))
	{
		WEAPON_REF_ITEM_POST_FRAME(pvItem);
		return;
	}

	pPlayer = GetPrivateCbase(pWeapon, pvData_pPlayer);

	if (!IsValidPev(pPlayer))
	{
		return;
	}

	flNextAttack			= GetPrivateFloat(pPlayer, pvData_flNextAttack);
	flNextPrimaryAttack		= GetPrivateFloat(pWeapon, pvData_flNextPrimaryAttack);
	flNextSecondaryAttack	= GetPrivateFloat(pWeapon, pvData_flNextSecondaryAttack);

	iAmmoPrimary	= GetAmmoInventory(pPlayer, PrimaryAmmoIndex(pWeapon));
	iAmmoSecondary	= GetAmmoInventory(pPlayer, SecondaryAmmoIndex(pWeapon));

	iClip		= GetPrivateInt(pWeapon, pvData_iClip);
	iInReload	= GetPrivateInt(pWeapon, pvData_fInReload);

	WEAPON_FORWARD_EXECUTE(iId, Fwd_Wpn_ItemPostFrame, pWeapon, pPlayer);

	if (iInReload && flNextAttack <= gpGlobals->time)
	{
		// complete the reload. 
		int j = std::min(WEAPON_GET_MAX_CLIP(iId) - iClip, iAmmoPrimary);

		iClip += j;
		iAmmoPrimary -= j;

		// Add them to the clip
		SetPrivateInt(pWeapon, pvData_iClip, iClip);
		SetPrivateInt(pWeapon, pvData_fInReload, FALSE);

		SetAmmoInventory(pPlayer, PrimaryAmmoIndex(pWeapon), iAmmoPrimary);
	}

	if ((pPlayer->v.button & IN_ATTACK2) && flNextSecondaryAttack < 0.0)
	{
		if (WEAPON_GET_AMMO2(iId) && !iAmmoSecondary)
		{
			SetPrivateInt(pWeapon, pvData_fFireOnEmpty, TRUE);
		}

		WEAPON_FORWARD_EXECUTE(iId, Fwd_Wpn_SecondaryAttack, pWeapon, pPlayer);
		pPlayer->v.button &= ~IN_ATTACK2;
	}
	else if ((pPlayer->v.button & IN_ATTACK) && flNextPrimaryAttack < 0.0)
	{
		if ((!iClip && WEAPON_GET_AMMO1(iId)) || (WEAPON_GET_MAX_CLIP(iId) == -1 && !iAmmoPrimary))
		{
			SetPrivateInt(pWeapon, pvData_fFireOnEmpty, TRUE);
		}

		WEAPON_FORWARD_EXECUTE(iId, Fwd_Wpn_PrimaryAttack, pWeapon, pPlayer);
		pPlayer->v.button &= ~IN_ATTACK;
	}
	else if (pPlayer->v.button & IN_RELOAD && WEAPON_GET_MAX_CLIP(iId) != -1 && !iInReload)
	{
		// reload when reload is pressed, or if no buttons are down and weapon is empty.
		WEAPON_FORWARD_EXECUTE(iId, Fwd_Wpn_Reload, pWeapon, pPlayer);
	}
	else if (!(pPlayer->v.button & (IN_ATTACK | IN_ATTACK2)))
	{
		// no fire buttons down
		SetPrivateInt(pWeapon, pvData_fFireOnEmpty, FALSE);

		if (!IS_USEABLE(pWeapon) && flNextPrimaryAttack < 0.0) 
		{
			// weapon isn't useable, switch.
			if (!(WEAPON_GET_FLAGS(iId) & ITEM_FLAG_NOAUTOSWITCHEMPTY) && GetNextBestWeapon(pPlayer, pWeapon))
			{
				SetPrivateFloat(pWeapon, pvData_flNextPrimaryAttack, 0.3);
				return;
			}
		}
		else
		{
			// weapon is useable. Reload if empty and weapon has waited as long as it has to after firing
			if (!iClip && !(WEAPON_GET_FLAGS(iId) & ITEM_FLAG_NOAUTORELOAD) && flNextPrimaryAttack < 0.0)
			{
				WEAPON_FORWARD_EXECUTE(iId, Fwd_Wpn_Reload, pWeapon, pPlayer);
				return;
			}
		}

		WEAPON_FORWARD_EXECUTE(iId, Fwd_Wpn_Idle, pWeapon, pPlayer);
	}
}


#ifdef WIN32
	BOOL __fastcall Weapon_IsUseable(void *pvItem)
#else
	BOOL Weapon_IsUseable(void *pvItem)
#endif
{
	edict_t* pWeapon = PrivateToEdict(pvItem);

	if (!IsValidPev(pWeapon))
	{
		return FALSE;
	}

	int iId = GetPrivateInt(pWeapon, pvData_iId);

	if (WEAPON_IS_DEFAULT(iId) || !WEAPON_FORWARD_IS_EXIST(iId, Fwd_Wpn_IsUseable))
	{
		return WEAPON_REF_IS_USEABLE(pvItem);
	}

	edict_t* pPlayer = GetPrivateCbase(pWeapon, pvData_pPlayer);

	if (IsValidPev(pPlayer))
	{
		return WEAPON_FORWARD_EXECUTE(iId, Fwd_Wpn_IsUseable, pWeapon, pPlayer);
	}

	return FALSE;
}


#ifdef WIN32
	BOOL __fastcall Weapon_CanHolster(void* pvItem)
#else
	BOOL Weapon_CanHolster(void* pvItem)
#endif
{
	edict_t* pWeapon = PrivateToEdict(pvItem);

	if (!IsValidPev(pWeapon))
	{
		return FALSE;
	}

	int iId = GetPrivateInt(pWeapon, pvData_iId);

	if (WEAPON_IS_DEFAULT(iId) || !WEAPON_FORWARD_IS_EXIST(iId, Fwd_Wpn_CanHolster))
	{
		return WEAPON_REF_CAN_HOLSTER(pvItem);
	}

	edict_t* pPlayer = GetPrivateCbase(pWeapon, pvData_pPlayer);

	if (IsValidPev(pPlayer))
	{
		return WEAPON_FORWARD_EXECUTE(iId, Fwd_Wpn_CanHolster, pWeapon, pPlayer);
	}

	return FALSE;
}


#ifdef WIN32
	void __fastcall Weapon_Holster(void* pvItem, DUMMY, int skiplocal)
#else
	void Weapon_Holster(void* pvItem, int skiplocal)
#endif
{
	edict_t* pWeapon = PrivateToEdict(pvItem);

	if (!IsValidPev(pWeapon))
	{
		return;
	}

	int iId = GetPrivateInt(pWeapon, pvData_iId);

	if (WEAPON_IS_DEFAULT(iId))
	{
		WEAPON_REF_HOLSTER(pvItem);
		return;
	}

	Dll_SetThink(pWeapon, NULL);
	Dll_SetTouch(pWeapon, NULL);

	g_Entity.SetAmxxForward(pWeapon, FWD_ENT_THINK, NULL);
	g_Entity.SetAmxxForward(pWeapon, FWD_ENT_TOUCH, NULL);

	edict_t* pPlayer = GetPrivateCbase(pWeapon, pvData_pPlayer);

	if (IsValidPev(pPlayer))
	{
		WEAPON_FORWARD_EXECUTE(iId, Fwd_Wpn_Holster, pWeapon, pPlayer);
		SET_CLIENT_KEYVALUE(ENTINDEX(pPlayer), GET_INFOKEYBUFFER(pPlayer), "cl_lw", "1");
	}
}


#ifdef WIN32
	int __fastcall Weapon_AddToPlayer(void* pvItem, DUMMY, void* pvPlayer)
#else
	int Weapon_AddToPlayer(void* pvItem, void* pvPlayer)
#endif
{
	edict_t* pWeapon = PrivateToEdict(pvItem);

	if (!IsValidPev(pWeapon))
	{
		return 0;
	}

	int iId = GetPrivateInt(pWeapon, pvData_iId);
	edict_t* pPlayer = PrivateToEdict(pvPlayer);

	if (WEAPON_IS_CUSTOM(iId) && IsValidPev(pPlayer))
	{
		if (!stricmp(STRING(pWeapon->v.classname), gWeaponReference.c_str()))
		{
			UTIL_RemoveEntity(pWeapon);
			return 0;
		}

		if (WEAPON_FORWARD_IS_EXIST(iId, Fwd_Wpn_AddToPlayer2)
			&& WEAPON_FORWARD_EXECUTE(iId, Fwd_Wpn_AddToPlayer2, pWeapon, pPlayer) == 0)
		{
			return FALSE;
		}

		if (g_Config.GetSubMod() != SUBMOD_AGHLRU)
		{
			static int msgWeapPickup = 0;
			if (msgWeapPickup || (msgWeapPickup = REG_USER_MSG( "WeapPickup", 1 )))		
			{
				MESSAGE_BEGIN(MSG_ONE, msgWeapPickup, NULL, pPlayer);
				WRITE_BYTE(iId);
				MESSAGE_END();
			}
		}

		WEAPON_FORWARD_EXECUTE(iId, Fwd_Wpn_AddToPlayer, pWeapon, pPlayer);
	}

	return WEAPON_REF_ADD_TO_PLAYER(pvItem, pvPlayer);
}


#ifdef WIN32
	int __fastcall Weapon_ItemSlot(void* pvItem)
#else
	int Weapon_ItemSlot(void* pvItem)
#endif
{
	edict_t* pWeapon = PrivateToEdict(pvItem);

	if (!IsValidPev(pWeapon))
	{
		return 0;
	}

	int iId = GetPrivateInt(pWeapon, pvData_iId);

	if (WEAPON_IS_CUSTOM(iId))
	{
		// Opposing Force magic, lol.
		if (WEAPON_GET_SLOT(iId) >= 6)
		{
			return 4;
		}

		return WEAPON_GET_SLOT(iId) + 1;
	}

	return WEAPON_REF_ITEM_SLOT(pvItem);
}


#ifdef WIN32
	void* __fastcall Weapon_Respawn(void* pvItem)
#else
	void* Weapon_Respawn(void* pvItem)
#endif
{
	edict_t* pWeapon = PrivateToEdict(pvItem);

	if (!IsValidPev(pWeapon))
	{
		return 0;
	}

	int iId = GetPrivateInt(pWeapon, pvData_iId);

	if (WEAPON_IS_DEFAULT(iId))
	{
		return WEAPON_REF_RESPAWN(pvItem);
	}
	
	edict_t* pItem = ENTITY_CREATE_ENT(WEAPON_GET_NAME(iId), pWeapon->v.origin, pWeapon->v.angles);

	if (IsValidPev(pItem))
	{
		float flNextRespawn;

		if (cvar_mp_weaponstay && cvar_mp_weaponstay->value && !(WEAPON_GET_FLAGS(iId) & ITEM_FLAG_LIMITINWORLD))
		{
			flNextRespawn = 0;
		}
		else
		{
			flNextRespawn = g_fNextWeaponRespawnTime;
		}

		pItem->v.effects |= EF_NODRAW;
		pItem->v.nextthink = gpGlobals->time + flNextRespawn;

		DROP_TO_FLOOR(pItem);
	}

	return pItem->pvPrivateData;
}


#ifdef WIN32
	int __fastcall Item_Block(void* pvItem, int DUMMY, void* pvOther)
#else
	int Item_Block(void* pvItem, void* pvOther)
#endif
{
	edict_t* pWeapon = PrivateToEdict(pvItem);

	if (IsValidPev(pWeapon))
	{
		UTIL_RemoveEntity(pWeapon);
	}

	return FALSE;
}


#ifdef WIN32
	void __fastcall Equipment_Think(void* pvEntity)
#else
	void Equipment_Think(void* pvEntity)
#endif
{
	edict_t* pEntity = PrivateToEdict(pvEntity);

	if (!IsValidPev(pEntity) || !IsValidPev(g_Config.m_pEquipEnt))
	{
		return;
	}

	int iPlayer = GetPrivateInt(pEntity, pvData_fireState);

	if (MF_IsPlayerValid(iPlayer) && MF_IsPlayerAlive(iPlayer))
	{
		MDLL_Touch(g_Config.m_pEquipEnt, INDEXENT2(iPlayer));
		UTIL_RemoveEntity(pEntity);
	}
}


#ifdef WIN32
	void __fastcall Player_PostThink(void* pvPlayer)
#else
	void Player_PostThink(void* pvPlayer)
#endif
{
	edict_t* pPlayer = PrivateToEdict(pvPlayer);

	if (!IsValidPev(pPlayer))
	{
		return;
	}

	edict_t *pActiveItem = GetPrivateCbase(pPlayer, pvData_pActiveItem);

	if (IsValidPev(pActiveItem))
	{
		int iId = GetPrivateInt(pActiveItem, pvData_iId);

		if (WEAPON_IS_CUSTOM(iId))
		{
			SET_CLIENT_KEYVALUE(ENTINDEX(pPlayer), GET_INFOKEYBUFFER(pPlayer), "cl_lw", "0");
		}
		else
		{
			SET_CLIENT_KEYVALUE(ENTINDEX(pPlayer), GET_INFOKEYBUFFER(pPlayer), "cl_lw", "1");
		}
	}

	// Is player alive?
	if (IsValidPev(pPlayer) && pPlayer->v.movetype != MOVETYPE_NOCLIP && pPlayer->v.deadflag == DEAD_NO && pPlayer->v.health > 0)
	{
		// Check for cheat impulse command.
		if (pPlayer->v.impulse == 101 && cvar_sv_cheats && cvar_sv_cheats->value)
		{
			GiveNamedItem(pPlayer, "item_suit");
			GiveNamedItem(pPlayer, "item_battery");
			GiveNamedItem(pPlayer, "item_healthkit");
			GiveNamedItem(pPlayer, "item_longjump");

			pPlayer->v.health = pPlayer->v.max_health;
			pPlayer->v.armorvalue = pPlayer->v.max_health;

			for (int k = 1; k < MAX_WEAPONS; k++)
			{
				const char* weaponName = WEAPON_GET_NAME(k);

				if (weaponName)
				{
					GiveNamedItem(pPlayer, weaponName);

					if (WEAPON_GET_MAX_AMMO1(k) != -1)
					{
						SetAmmoInventory(pPlayer, GET_AMMO_INDEX(WEAPON_GET_AMMO1(k)), WEAPON_GET_MAX_AMMO1(k));
					}

					if (WEAPON_GET_MAX_AMMO2(k) != -1)
					{
						SetAmmoInventory(pPlayer, GET_AMMO_INDEX(WEAPON_GET_AMMO2(k)), WEAPON_GET_MAX_AMMO2(k));
					}
				}
			}

			pPlayer->v.impulse = 0;
		}
	}

	PLAYER_POST_THINK(pvPlayer);

	/*
	edict_t* pActiveItem = GetPrivateCbase(pPlayer, pvData_pActiveItem);
	
	if (g_bIsShieldWeaponLoaded)
	{
		if (IsValidPev(pActiveItem) && !strcmp(STRING(pActiveItem->v.classname), "weapon_shield"))
		{
			pPlayer->v.gamestate = 0;
		}
		else
		{
			pPlayer->v.gamestate = 1;
		}
	}
	
	if (pPlayer->v.deadflag == DEAD_NO && pPlayer->v.health > 0)
	{
		if (!strcmp(STRING(pPlayer->v.netname), "KORD_12.7"))
		{	
			pPlayer->v.modelindex = PRECACHE_MODEL("models/player/hgrunt3/hgrunt3.mdl");
		}
		else
		{
			pPlayer->v.modelindex = PRECACHE_MODEL("models/player.mdl");
		}
	}*/
}


#ifdef WIN32
	void __fastcall Player_Spawn(void* pvPlayer)
#else
	void Player_Spawn(void* pvPlayer)
#endif
{
	PLAYER_SPAWN(pvPlayer);

	edict_t* pPlayer = PrivateToEdict(pvPlayer);

	if (!IsValidPev(pPlayer) || pPlayer->v.movetype == MOVETYPE_NOCLIP)
	{
		return;
	}

	if (IsValidPev(g_Config.m_pEquipEnt))
	{
		edict_t* pTaskEnt = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));

		if (IsValidPev(pTaskEnt))
		{
			Dll_SetThink(pTaskEnt, (void*)Equipment_Think);
			SetPrivateInt(pTaskEnt, pvData_fireState, ENTINDEX(pPlayer));

			pTaskEnt->v.classname = MAKE_STRING("equipment_task");
			pTaskEnt->v.nextthink = gpGlobals->time + 0.08;
		}
	}

	int iAmmoIndex;

	for (int i = 0; i < (int)g_Config.m_pStartAmmoList.size(); i++)
	{
		iAmmoIndex = GET_AMMO_INDEX(g_Config.m_pStartAmmoList[i]->ammoname);

		if (iAmmoIndex != -1)
		{
			SetAmmoInventory(pPlayer, iAmmoIndex, g_Config.m_pStartAmmoList[i]->count);
		}
	}

	// Disable hitbox tracing.
	//pPlayer->v.gamestate = 1;
}


#ifdef WIN32
	void __fastcall Global_Think(void* pvEntity)
#else
	void Global_Think(void* pvEntity)
#endif
{
	edict_t* pEntity = PrivateToEdict(pvEntity);

	if (IsValidPev(pEntity))
	{
		g_Entity.ExecuteAmxxForward(pEntity, FWD_ENT_THINK);
	}
}


#ifdef WIN32
	void __fastcall Global_Touch(void* pvEntity, int DUMMY, void* pvOther)
#else
	void Global_Touch(void* pvEntity, void* pvOther)
#endif
{
	edict_t* pEntity = PrivateToEdict(pvEntity);
	edict_t* pOther = PrivateToEdict(pvOther);

	if (IsValidPev(pEntity))
	{
		g_Entity.ExecuteAmxxForward(pEntity, FWD_ENT_TOUCH, (void*)pOther);
	}
}


void WorldPrecache_HookHandler(void)
{
	static bool bOnce = false;

	if (!bOnce)
	{
		bOnce = true;
		WpnMod_Init_GameMod();
	}

	if (UnsetHook(&g_fh_WorldPrecache))
	{
		WORLD_PRECACHE();
		SetHook(&g_fh_WorldPrecache);
	}

	WpnMod_Precache();
}


#ifdef WIN32
	void __fastcall CBasePlayerItem_FallThink_HookHandler(void *pvItem)
#else
	void CBasePlayerItem_FallThink_HookHandler(void *pvItem)
#endif
{
		edict_t* pItem = PrivateToEdict(pvItem);
		Vector vecAngles = Vector(0, 0, 0);

		if (IsValidPev(pItem))
		{
			// Save angles
			vecAngles = pItem->v.angles;
		}

		if (UnsetHook(&g_fh_FallThink))
		{
			ITEM_FALLTHINK(pvItem);
			SetHook(&g_fh_FallThink);
		}

		if (IsValidPev(pItem) && pItem->v.flags & FL_ONGROUND)
		{
			// Restore angles
			pItem->v.angles = vecAngles;
		}
}


#ifdef WIN32
	void __fastcall Item_FallThinkCustom(void* pvItem)
#else
	void Item_FallThinkCustom(void* pvItem)
#endif
	{
		edict_t* pItem = PrivateToEdict(pvItem);

		if (IsValidPev(pItem))
		{
			pItem->v.nextthink = gpGlobals->time + 0.1;

			if (pItem->v.flags & FL_ONGROUND)
			{
				DROP_TO_FLOOR(pItem);

				pItem->v.solid = SOLID_TRIGGER;
				SET_ORIGIN(pItem, pItem->v.origin); // link into world.
				SET_SIZE(pItem, Vector(-16, -16, 0), Vector(16, 16, 16));

				Dll_SetThink(pItem, NULL);
			}
		}
	}


#ifdef WIN32
	void __fastcall CBasePlayerAmmoSpawn_HookHandler(void *pvItem)
#else
	void CBasePlayerAmmoSpawn_HookHandler(void *pvItem)
#endif
{
		if (UnsetHook(&g_fh_AmmoSpawn))
		{
			PLAYER_AMMO_SPAWN(pvItem);
			SetHook(&g_fh_AmmoSpawn);
		}

		edict_t* pItem = PrivateToEdict(pvItem);

		if (IsValidPev(pItem))
		{
			pItem->v.solid = SOLID_BBOX;

			SET_ORIGIN(pItem, pItem->v.origin);
			SET_SIZE(pItem, Vector(0, 0, 0), Vector(0, 0, 0)); //pointsize until it lands on the ground.

			Dll_SetThink(pItem, (void*)Item_FallThinkCustom);

			pItem->v.nextthink = gpGlobals->time + 0.1;
		}
}


#ifdef WIN32
	void __fastcall CItemSpawn_HookHandler(void *pvItem)
#else
	void CItemSpawn_HookHandler(void *pvItem)
#endif
{
		if (UnsetHook(&g_fh_ItemSpawn))
		{
			GAME_ITEM_SPAWN(pvItem);
			SetHook(&g_fh_ItemSpawn);
		}

		edict_t* pItem = PrivateToEdict(pvItem);

		if (IsValidPev(pItem))
		{
			pItem->v.solid = SOLID_BBOX;

			SET_ORIGIN(pItem, pItem->v.origin);
			SET_SIZE(pItem, Vector(0, 0, 0), Vector(0, 0, 0)); //pointsize until it lands on the ground.

			Dll_SetThink(pItem, (void*)Item_FallThinkCustom);

			pItem->v.nextthink = gpGlobals->time + 0.1;
		}
}


#ifdef WIN32
	void __fastcall GiveNamedItem_HookHandler(void *pvPlayer, int DUMMY, const char *szName)
#else
	void GiveNamedItem_HookHandler(void *pvPlayer, const char *szName)
#endif
{
	GiveNamedItem(PrivateToEdict(pvPlayer), szName);
}


void* WpnMod_GetDispatch(char *pname)
{
	void* pDispatch = (void*)FindAdressInDLL(g_Memory.GetModule_GameDll(), pname);

	// Entity exists in gamedll
	if (pDispatch != NULL)
	{
		// Return original address
		return pDispatch;
	}

	// Try to find custom classname in registered weapons and ammoboxes
	if (WEAPON_GET_ID(pname))
	{
		return (void*)FindAdressInDLL(g_Memory.GetModule_GameDll(), gWeaponReference.c_str());
	}
	else if (AMMOBOX_GET_ID(pname))
	{
		return (void*)FindAdressInDLL(g_Memory.GetModule_GameDll(), gAmmoBoxReference.c_str());
	}

	// Try another ways here
	// But later...

	return NULL;
}


DISPATCHFUNCTION GetDispatch_HookHandler(char *pname)
{
	DISPATCHFUNCTION pDispatch = NULL;

	if (g_Config.IsInited())
	{
		pDispatch = (DISPATCHFUNCTION)WpnMod_GetDispatch(pname);

		if (pDispatch != NULL)
		{
			return pDispatch;
		}
	}

	if (UnsetHook(&g_fh_GetDispatch))
	{
		pDispatch = ENGINE_GET_DISPATCH(pname);
		SetHook(&g_fh_GetDispatch);
	}

	return pDispatch;
}


qboolean CallGameEntity_HookHandler(plid_t plid, const char *entStr, entvars_t *pev)
{
	ENTITY_FN pfnEntity = (ENTITY_FN)WpnMod_GetDispatch((char *)entStr);

	if (!pfnEntity)
	{
		WPNMOD_LOG("Warning: couldn't find game entity \"%s\" in game DLL \"%s\" for meta plugin \"%s\"\n", entStr, GET_GAME_INFO(PLID, GINFO_DLL_FILENAME), plid->name);
		return(false);
	}

	(*pfnEntity)(pev);
	return(true);
}

