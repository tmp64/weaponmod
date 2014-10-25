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

#include "wpnmod_entity.h"
#include "wpnmod_parse.h"
#include "wpnmod_utils.h"
#include "wpnmod_hooks.h"
#include "entity_state.h"


VirtualHookData g_CrowbarHooks[CrowbarHook_End] = 
{
	VHOOK_CROWBAR(Respawn),
	VHOOK_CROWBAR(AddToPlayer),
	VHOOK_CROWBAR(GetItemInfo),
	VHOOK_CROWBAR(CanDeploy),
	VHOOK_CROWBAR(Deploy),
	VHOOK_CROWBAR(CanHolster),
	VHOOK_CROWBAR(Holster),
	VHOOK_CROWBAR(ItemPostFrame),
	VHOOK_CROWBAR(ItemSlot),
	VHOOK_CROWBAR(IsUseable)
};

VirtualHookData	g_RpgAddAmmo_Hook		= VHOOK(gAmmoBoxReference,	VO_AddAmmo,				AmmoBox_AddAmmo);
VirtualHookData g_PlayerSpawn_Hook		= VHOOK("player",			VO_Spawn,				Player_Spawn);
VirtualHookData g_PlayerPostThink_Hook	= VHOOK("player",			VO_Player_PostThink,	Player_PostThink);


#ifdef WIN32
	int __fastcall Weapon_GetItemInfo(void* pvItem, DUMMY, ItemInfo* p)
#else
	int Weapon_GetItemInfo(void* pvItem, ItemInfo* p)
#endif
{
	GET_ITEM_INFO(pvItem, p);

	static int iId = 0;
	static edict_t* pWeapon;

	pWeapon = PrivateToEdict(pvItem);

	if (!IsValidPev(pWeapon))
	{
		return 0;
	}

	iId = GetPrivateInt(pWeapon, pvData_iId);

	if (WeaponInfoArray[iId].iType == Wpn_Custom)
	{
		p->iId = iId;
		p->pszName = WEAPON_GET_NAME(iId);
		p->iSlot = WEAPON_GET_SLOT(iId);
		p->iPosition = WEAPON_GET_SLOT_POSITION(iId);
		p->iMaxAmmo1 = WEAPON_GET_MAX_AMMO1(iId);
		p->iMaxAmmo2 = WEAPON_GET_MAX_AMMO2(iId);
		p->iMaxClip = WEAPON_GET_MAX_CLIP(iId);
		p->iFlags = WEAPON_GET_FLAGS(iId);
		p->iWeight = WEAPON_GET_WEIGHT(iId);

		if (WEAPON_GET_AMMO1(iId)[0])
		{
			p->pszAmmo1 = WEAPON_GET_AMMO1(iId);
		}

		if (WEAPON_GET_AMMO2(iId)[0])
		{
			p->pszAmmo2 = WEAPON_GET_AMMO2(iId);
		}
	}

	return 1;
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

	if (WeaponInfoArray[iId].iType == Wpn_Default || !WeaponInfoArray[iId].iForward[Fwd_Wpn_CanDeploy])
	{
		return CAN_DEPLOY(pvItem);
	}

	edict_t* pPlayer = GetPrivateCbase(pWeapon, pvData_pPlayer);

	if (!IsValidPev(pPlayer))
	{
		return FALSE;
	}

	return MF_ExecuteForward
	(
		WeaponInfoArray[iId].iForward[Fwd_Wpn_CanDeploy],

		static_cast<cell>(ENTINDEX(pWeapon)), 
		static_cast<cell>(ENTINDEX(pPlayer)), 
		static_cast<cell>(GetPrivateInt(pWeapon, pvData_iClip)), 
		static_cast<cell>(GetAmmoInventory(pPlayer, PrimaryAmmoIndex(pWeapon))),
		static_cast<cell>(GetAmmoInventory(pPlayer, SecondaryAmmoIndex(pWeapon)))
	);
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

	if (WeaponInfoArray[iId].iType == Wpn_Default)
	{
		return DEPLOY(pvItem);
	}

	edict_t* pPlayer = GetPrivateCbase(pWeapon, pvData_pPlayer);

	int iReturn = FALSE;

	if (!IsValidPev(pPlayer))
	{
		return iReturn;
	}

	g_engfuncs.pfnSetClientKeyValue(ENTINDEX(pPlayer), g_engfuncs.pfnGetInfoKeyBuffer(pPlayer), "cl_lw", "0");

	if (WeaponInfoArray[iId].iForward[Fwd_Wpn_Deploy])
	{
		iReturn = MF_ExecuteForward
		(
			WeaponInfoArray[iId].iForward[Fwd_Wpn_Deploy],

			static_cast<cell>(ENTINDEX(pWeapon)), 
			static_cast<cell>(ENTINDEX(pPlayer)), 
			static_cast<cell>(GetPrivateInt(pWeapon, pvData_iClip)), 
			static_cast<cell>(GetAmmoInventory(pPlayer, PrimaryAmmoIndex(pWeapon))),
			static_cast<cell>(GetAmmoInventory(pPlayer, SecondaryAmmoIndex(pWeapon)))
		);
	}

	return iReturn;
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

	if (WeaponInfoArray[iId].iType == Wpn_Default)
	{
		ITEM_POST_FRAME(pvItem);
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

	if (WeaponInfoArray[iId].iForward[Fwd_Wpn_ItemPostFrame])
	{
		MF_ExecuteForward
		(
			WeaponInfoArray[iId].iForward[Fwd_Wpn_ItemPostFrame],

			static_cast<cell>(ENTINDEX(pWeapon)), 
			static_cast<cell>(ENTINDEX(pPlayer)), 
			static_cast<cell>(iClip), 
			static_cast<cell>(iAmmoPrimary),
			static_cast<cell>(iAmmoSecondary)
		);
	}

	if (iInReload && flNextAttack <= gpGlobals->time)
	{
		// complete the reload. 
		int j = min(WEAPON_GET_MAX_CLIP(iId) - iClip, iAmmoPrimary);

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

		if (WeaponInfoArray[iId].iForward[Fwd_Wpn_SecondaryAttack])
		{
			MF_ExecuteForward
			(
				WeaponInfoArray[iId].iForward[Fwd_Wpn_SecondaryAttack],

				static_cast<cell>(ENTINDEX(pWeapon)), 
				static_cast<cell>(ENTINDEX(pPlayer)), 
				static_cast<cell>(iClip), 
				static_cast<cell>(iAmmoPrimary),
				static_cast<cell>(iAmmoSecondary)
			);
		}

		pPlayer->v.button &= ~IN_ATTACK2;
	}
	else if ((pPlayer->v.button & IN_ATTACK) && flNextPrimaryAttack < 0.0)
	{
		if ((!iClip && WEAPON_GET_AMMO1(iId)) || (WEAPON_GET_MAX_CLIP(iId) == -1 && !iAmmoPrimary))
		{
			SetPrivateInt(pWeapon, pvData_fFireOnEmpty, TRUE);
		}

		if (WeaponInfoArray[iId].iForward[Fwd_Wpn_PrimaryAttack])
		{
			MF_ExecuteForward
			(
				WeaponInfoArray[iId].iForward[Fwd_Wpn_PrimaryAttack],

				static_cast<cell>(ENTINDEX(pWeapon)), 
				static_cast<cell>(ENTINDEX(pPlayer)), 
				static_cast<cell>(iClip), 
				static_cast<cell>(iAmmoPrimary),
				static_cast<cell>(iAmmoSecondary)
			);
		}

		pPlayer->v.button &= ~IN_ATTACK;
	}
	else if (pPlayer->v.button & IN_RELOAD && WEAPON_GET_MAX_CLIP(iId) != -1 && !iInReload)
	{
		// reload when reload is pressed, or if no buttons are down and weapon is empty.
		if (WeaponInfoArray[iId].iForward[Fwd_Wpn_Reload])
		{
			MF_ExecuteForward
			(
				WeaponInfoArray[iId].iForward[Fwd_Wpn_Reload],

				static_cast<cell>(ENTINDEX(pWeapon)), 
				static_cast<cell>(ENTINDEX(pPlayer)), 
				static_cast<cell>(iClip), 
				static_cast<cell>(iAmmoPrimary),
				static_cast<cell>(iAmmoSecondary)
			);
		}
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
				if (WeaponInfoArray[iId].iForward[Fwd_Wpn_Reload])
				{
					MF_ExecuteForward
					(
						WeaponInfoArray[iId].iForward[Fwd_Wpn_Reload],

						static_cast<cell>(ENTINDEX(pWeapon)), 
						static_cast<cell>(ENTINDEX(pPlayer)), 
						static_cast<cell>(iClip), 
						static_cast<cell>(iAmmoPrimary),
						static_cast<cell>(iAmmoSecondary)
					);
				}
				return;
			}
		}

		if (WeaponInfoArray[iId].iForward[Fwd_Wpn_Idle])
		{
			MF_ExecuteForward
			(
				WeaponInfoArray[iId].iForward[Fwd_Wpn_Idle],

				static_cast<cell>(ENTINDEX(pWeapon)), 
				static_cast<cell>(ENTINDEX(pPlayer)), 
				static_cast<cell>(iClip), 
				static_cast<cell>(iAmmoPrimary),
				static_cast<cell>(iAmmoSecondary)
			);
		}
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

	if (WeaponInfoArray[iId].iType == Wpn_Default || !WeaponInfoArray[iId].iForward[Fwd_Wpn_IsUseable])
	{
		return IS_USEABLE(pvItem);
	}

	edict_t* pPlayer = GetPrivateCbase(pWeapon, pvData_pPlayer);

	if (!IsValidPev(pPlayer))
	{
		return FALSE;
	}

	return MF_ExecuteForward
	(
		WeaponInfoArray[iId].iForward[Fwd_Wpn_IsUseable],

		static_cast<cell>(ENTINDEX(pWeapon)), 
		static_cast<cell>(ENTINDEX(pPlayer)), 
		static_cast<cell>(GetPrivateInt(pWeapon, pvData_iClip)), 
		static_cast<cell>(GetAmmoInventory(pPlayer, PrimaryAmmoIndex(pWeapon))),
		static_cast<cell>(GetAmmoInventory(pPlayer, SecondaryAmmoIndex(pWeapon)))
	);
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

	if (WeaponInfoArray[iId].iType == Wpn_Default || !WeaponInfoArray[iId].iForward[Fwd_Wpn_CanHolster])
	{
		return CAN_HOLSTER(pvItem);
	}

	edict_t* pPlayer = GetPrivateCbase(pWeapon, pvData_pPlayer);

	if (!IsValidPev(pPlayer))
	{
		return FALSE;
	}

	return MF_ExecuteForward
	(
		WeaponInfoArray[iId].iForward[Fwd_Wpn_CanHolster],

		static_cast<cell>(ENTINDEX(pWeapon)), 
		static_cast<cell>(ENTINDEX(pPlayer)), 
		static_cast<cell>(GetPrivateInt(pWeapon, pvData_iClip)), 
		static_cast<cell>(GetAmmoInventory(pPlayer, PrimaryAmmoIndex(pWeapon))),
		static_cast<cell>(GetAmmoInventory(pPlayer, SecondaryAmmoIndex(pWeapon)))
	);
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

	if (WeaponInfoArray[iId].iType == Wpn_Default)
	{
		HOLSTER(pvItem);
		return;
	}

	Dll_SetThink(pWeapon, NULL);
	Dll_SetTouch(pWeapon, NULL);

	g_Entity.SetAmxxForward(pWeapon, FWD_ENT_THINK, NULL);
	g_Entity.SetAmxxForward(pWeapon, FWD_ENT_TOUCH, NULL);

	edict_t* pPlayer = GetPrivateCbase(pWeapon, pvData_pPlayer);

	if (IsValidPev(pPlayer) && WeaponInfoArray[iId].iForward[Fwd_Wpn_Holster])
	{
		MF_ExecuteForward
		(
			WeaponInfoArray[iId].iForward[Fwd_Wpn_Holster],

			static_cast<cell>(ENTINDEX(pWeapon)), 
			static_cast<cell>(ENTINDEX(pPlayer)), 
			static_cast<cell>(GetPrivateInt(pWeapon, pvData_iClip)), 
			static_cast<cell>(GetAmmoInventory(pPlayer, PrimaryAmmoIndex(pWeapon))),
			static_cast<cell>(GetAmmoInventory(pPlayer, SecondaryAmmoIndex(pWeapon)))
		);
	}

	g_engfuncs.pfnSetClientKeyValue(ENTINDEX(pPlayer), g_engfuncs.pfnGetInfoKeyBuffer(pPlayer), "cl_lw", "1");
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

	if (WeaponInfoArray[iId].iType == Wpn_Custom && IsValidPev(pPlayer))
	{
		if (!stricmp(STRING(pWeapon->v.classname), gWeaponReference))
		{
			UTIL_RemoveEntity(pWeapon);
			return 0;
		}

		if (WeaponInfoArray[iId].iForward[Fwd_Wpn_AddToPlayer2])
		{
			if 
			(
				!MF_ExecuteForward
				(
					WeaponInfoArray[iId].iForward[Fwd_Wpn_AddToPlayer2],

					static_cast<cell>(ENTINDEX(pWeapon)), 
					static_cast<cell>(ENTINDEX(pPlayer)), 
					static_cast<cell>(GetPrivateInt(pWeapon, pvData_iClip)), 
					static_cast<cell>(GetAmmoInventory(pPlayer, PrimaryAmmoIndex(pWeapon))),
					static_cast<cell>(GetAmmoInventory(pPlayer, SecondaryAmmoIndex(pWeapon)))
				)
			)
			{
				return FALSE;
			}
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

		if (WeaponInfoArray[iId].iForward[Fwd_Wpn_AddToPlayer])
		{
			MF_ExecuteForward
			(
				WeaponInfoArray[iId].iForward[Fwd_Wpn_AddToPlayer],

				static_cast<cell>(ENTINDEX(pWeapon)), 
				static_cast<cell>(ENTINDEX(pPlayer)), 
				static_cast<cell>(GetPrivateInt(pWeapon, pvData_iClip)), 
				static_cast<cell>(GetAmmoInventory(pPlayer, PrimaryAmmoIndex(pWeapon))),
				static_cast<cell>(GetAmmoInventory(pPlayer, SecondaryAmmoIndex(pWeapon)))
			);
		}
	}

	return ADD_TO_PLAYER(pvItem, pvPlayer);
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

	if (WeaponInfoArray[iId].iType == Wpn_Custom)
	{

		// Opposing Force magic, lol.
		if (WEAPON_GET_SLOT(iId) >= 6)
		{
			return 4;
		}

		return WEAPON_GET_SLOT(iId) + 1;
	}

	return ITEM_SLOT(pvItem);
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

	if (WeaponInfoArray[iId].iType == Wpn_Default)
	{
		return RESPAWN(pvItem);
	}
	
	edict_t* pItem = Wpnmod_SpawnItem(WEAPON_GET_NAME(iId), pWeapon->v.origin, pWeapon->v.angles);

	if (IsValidPev(pItem))
	{
		float flNextRespawn;

		if (cvar_mp_weaponstay && cvar_mp_weaponstay->value && !(WEAPON_GET_FLAGS(iId) & ITEM_FLAG_LIMITINWORLD))
		{
			flNextRespawn = 0;
		}
		else
		{
			flNextRespawn = WEAPON_RESPAWN_TIME;
		}

		pItem->v.effects |= EF_NODRAW;
		pItem->v.nextthink = gpGlobals->time + flNextRespawn;

		DROP_TO_FLOOR(pItem);
	}

	return pItem->pvPrivateData;
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

	if (!stricmp(STRING(pAmmobox->v.classname), gAmmoBoxReference))
	{
		if (g_Config.IsItemBlocked(gAmmoBoxReference))
		{
			UTIL_RemoveEntity(pAmmobox);
			return FALSE;
		}

		return ADD_AMMO(pvAmmo, pvOther);
	}

	BOOL bReturn = FALSE;

	for (int k = 1; k <= g_iAmmoBoxIndex; k++)
	{
		if (!strcmp(STRING(pAmmobox->v.classname), AmmoBoxInfoArray[k].classname.c_str()) && AmmoBoxInfoArray[k].iForward[Fwd_Ammo_AddAmmo])
		{
			bReturn = MF_ExecuteForward
			(
				AmmoBoxInfoArray[k].iForward[Fwd_Ammo_AddAmmo], 
				static_cast<cell>(ENTINDEX(pAmmobox)),
				static_cast<cell>(ENTINDEX(pOther))
			);

			break;
		}
	}

	return bReturn;
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

			for (int k = 1; k </*= g_iWeaponsCount*/MAX_WEAPONS; k++)
			{
				if (WEAPON_GET_NAME(k))
				{
					GiveNamedItem(pPlayer, WEAPON_GET_NAME(k));

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


#ifdef WIN32
	int __fastcall PackWeapon_HookHandler(void* pvWpnBox, int DUMMY, void* pvWeapon)
#else
	int PackWeapon_HookHandler(void* pvWpnBox, void* pvWeapon)
#endif
{
	int iResult = 0;

	if (UnsetHook(&g_fh_funcPackWeapon))
	{
		iResult = WEAPONBOX_PACK_WEAPON(pvWpnBox, pvWeapon);
		SetHook(&g_fh_funcPackWeapon);
	}

	if (!iResult)
	{
		return 0;
	}

	edict_t* pWeaponBox = PrivateToEdict(pvWpnBox);
	edict_t* pWeaponEnt = PrivateToEdict(pvWeapon);

	if (!IsValidPev(pWeaponBox) || !IsValidPev(pWeaponEnt))
	{
		return iResult;
	}

	if (g_Memory.m_pWpnBoxKillThink)
	{
		Dll_SetThink(pWeaponBox, g_Memory.m_pWpnBoxKillThink);
		pWeaponBox->v.nextthink = gpGlobals->time + g_Config.m_iWpnBoxLifeTime;
	}

	if (g_Config.m_iWpnBoxRenderColor)
	{
		pWeaponBox->v.renderfx = kRenderFxGlowShell;
		pWeaponBox->v.rendermode = kRenderNormal;
		pWeaponBox->v.renderamt = 16.0;

		if (g_Config.m_iWpnBoxRenderColor == 255255255)
		{
			pWeaponBox->v.rendercolor = Vector(RANDOM_LONG(0, 255), RANDOM_LONG(0, 255), RANDOM_LONG(0, 255));
		}
		else
		{
			Vector vecColor;
			int iColor = g_Config.m_iWpnBoxRenderColor;

			vecColor.x = iColor / 1000000;
			iColor %= 1000000;
			vecColor.y = iColor / 1000;
			vecColor.z = iColor % 1000;

			pWeaponBox->v.rendercolor = vecColor;
		}
	}

	int iId = GetPrivateInt(pWeaponEnt, pvData_iId);

	if (g_Config.m_bWpnBoxModels && WeaponInfoArray[iId].worldModel.empty())
	{
		edict_t* pTempEntity = NULL;

		if (WeaponInfoArray[iId].iType == Wpn_Default)
		{
			pTempEntity = CREATE_NAMED_ENTITY(pWeaponEnt->v.classname);
		}
		else
		{
			pTempEntity = Wpnmod_SpawnItem(STRING(pWeaponEnt->v.classname), Vector(0, 0, 0), Vector(0, 0, 0));
		}

		if (!IsValidPev(pTempEntity))
		{
			return iResult;
		}

		if (WeaponInfoArray[iId].iType == Wpn_Default)
		{
			MDLL_Spawn(pTempEntity);
		}

		WeaponInfoArray[iId].iWorldBody = pTempEntity->v.body;
		WeaponInfoArray[iId].iWorldSeq = pTempEntity->v.sequence;
		WeaponInfoArray[iId].worldModel.assign(STRING(pTempEntity->v.model));

		UTIL_RemoveEntity(pTempEntity);
	}

	if (g_Config.m_bWpnBoxModels && !WeaponInfoArray[iId].worldModel.empty())
	{
		pWeaponBox->v.body = WeaponInfoArray[iId].iWorldBody;
		pWeaponBox->v.sequence = WeaponInfoArray[iId].iWorldSeq;
		pWeaponBox->v.framerate = 1.0;

		SET_MODEL(pWeaponBox, WeaponInfoArray[iId].worldModel.c_str());
	}

	return 1;
}


void* WpnMod_GetDispatch(char *pname)
{
	void* pDispatch = FindFunction(g_Memory.GetModule_GameDll(), pname);

	// Entity is exist in gamedll
	if (pDispatch != NULL)
	{
		// Return original
		return pDispatch;
	}

	// Try to find custom classname and link it to reference value
	if (strstr(pname, "weapon_"))
	{
		for (int i = 1; i </*= g_iWeaponsCount*/MAX_WEAPONS; i++)
		{
			if (WEAPON_GET_NAME(i) && !_stricmp(WEAPON_GET_NAME(i), pname))
			{
				return FindFunction(g_Memory.GetModule_GameDll(), gWeaponReference);
			}
		}
	}
	else if (strstr(pname, "ammo_"))
	{
		for (int i = 1; i <= g_iAmmoBoxIndex; i++)
		{
			if (!_stricmp(AmmoBoxInfoArray[i].classname.c_str(), pname))
			{
				return FindFunction(g_Memory.GetModule_GameDll(), gAmmoBoxReference);
			}
		}
	}

	// Try another ways here
	return NULL;
}


DISPATCHFUNCTION GetDispatch_HookHandler(char *pname)
{
	return (DISPATCHFUNCTION)WpnMod_GetDispatch(pname);
}


qboolean CallGameEntity_HookHandler(plid_t plid, const char *entStr, entvars_t *pev)
{
	ENTITY_FN pfnEntity = (ENTITY_FN)WpnMod_GetDispatch((char *)entStr);

	if (!pfnEntity)
	{
		//META_WARNING("Couldn't find game entity '%s' in game DLL '%s' for plugin '%s'", entStr, GET_GAME_INFO(PLID, GINFO_DLL_FILENAME), plid->name);
		return(false);
	}

	(*pfnEntity)(pev);
	return(true);
}


void UpdateClientData_Post(const struct edict_s *ent, int sendweapons, struct clientdata_s *cd)
{
	if (IsValidPev(ent))
	{
		edict_t *pActiveItem = GetPrivateCbase((edict_t*)ent, pvData_pActiveItem);

		if (IsValidPev(pActiveItem))
		{
			int iId = GetPrivateInt(pActiveItem, pvData_iId);

			if (WeaponInfoArray[iId].iType == Wpn_Custom)
			{
				cd->m_iId = 0;
			}
		}
	}

	RETURN_META(MRES_IGNORED);
}

