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

#include "weaponmod.h"
#include "hooks.h"
#include "utils.h"


module hl_dll = {NULL, 0, NULL};

int g_vtblOffsets[VO_End];


VirtHookData g_PlayerSpawn_Hook = { "player", NULL, NULL, NULL, (void*)Player_Spawn };
VirtHookData g_RpgAddAmmo_Hook = { "ammo_rpgclip", NULL, NULL, NULL, (void*)AmmoBox_AddAmmo };
VirtHookData g_WorldPrecache_Hook = { "worldspawn", NULL, NULL, NULL, (void*)World_Precache };

VirtHookData g_CrowbarHooks[CrowbarHook_End] = 
{
	_CBHOOK(Respawn),
	_CBHOOK(AddToPlayer),
	_CBHOOK(GetItemInfo),
	_CBHOOK(CanDeploy),
	_CBHOOK(Deploy),
	_CBHOOK(CanHolster),
	_CBHOOK(Holster),
	_CBHOOK(ItemPostFrame),
	_CBHOOK(ItemSlot),
	_CBHOOK(IsUseable)
};

function g_dllFuncs[Func_End] =
{
	{ "", &hl_dll, {"", "", 0}, NULL, NULL, {}, {}, 0 },
	{ "", &hl_dll, {"", "", 0}, NULL, NULL, {}, {}, 0 },
	{ "", &hl_dll, {"", "", 0}, NULL, NULL, {}, {}, 0 },
	{ "", &hl_dll, {"", "", 0}, NULL, NULL, {}, {}, 0 },
	{ "", &hl_dll, {"", "", 0}, NULL, NULL, {}, {}, 0 },
	{ "", &hl_dll, {"", "", 0}, NULL, (void*)PrecacheOtherWeapon_HookHandler, {}, {}, 0 },
	{ "", &hl_dll, {"", "", 0}, NULL, (void*)GiveNamedItem_HookHandler, {}, {}, 0 },
	{ "", &hl_dll, {"", "", 0}, NULL, (void*)CheatImpulseCommands_HookHandler, {}, {}, 0 }
};



#ifdef _WIN32
int __fastcall Weapon_GetItemInfo(void *pPrivate, int i, ItemInfo *p)
#else
int Weapon_GetItemInfo(void *pPrivate, ItemInfo *p)
#endif
{
#ifdef _WIN32
	reinterpret_cast<int (__fastcall *)(void *, int, ItemInfo *)>(g_CrowbarHooks[CrowbarHook_GetItemInfo].address)(pPrivate, 0, p);
#else
	reinterpret_cast<int (*)(void *, ItemInfo *)>(g_CrowbarHooks[CrowbarHook_GetItemInfo].address)(pPrivate, p);
#endif

	static int iId = 0;
	static edict_t* pWeapon;

	pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(pWeapon))
	{
		return 0;
	}

	if (!g_iWeaponInitID)
	{
		iId = GetPrivateInt(pWeapon, pvData_iId);
	}
	else
	{
		iId = g_iWeaponInitID;
		g_iWeaponInitID = 0;
	}

	if (WeaponInfoArray[iId].iType == Wpn_Custom)
	{
		p->iId = iId;
		p->pszName = GetWeapon_pszName(iId);
		p->iSlot = GetWeapon_Slot(iId);
		p->iPosition = GetWeapon_ItemPosition(iId);
		p->iMaxAmmo1 = GetWeapon_MaxAmmo1(iId);
		p->iMaxAmmo2 = GetWeapon_MaxAmmo2(iId);
		p->iMaxClip = GetWeapon_MaxClip(iId);
		p->iFlags = GetWeapon_Flags(iId);
		p->iWeight = GetWeapon_Weight(iId);

		if (GetWeapon_pszAmmo1(iId)[0])
		{
			p->pszAmmo1 = GetWeapon_pszAmmo1(iId);
		}

		if (GetWeapon_pszAmmo2(iId)[0])
		{
			p->pszAmmo2 = GetWeapon_pszAmmo2(iId);
		}
	}

	return 1;
}



#ifdef _WIN32
BOOL __fastcall Weapon_CanDeploy(void *pPrivate)
#else
BOOL Weapon_CanDeploy(void *pPrivate)
#endif
{
	edict_t* pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(pWeapon))
	{
		return FALSE;
	}

	int iId = GetPrivateInt(pWeapon, pvData_iId);

	if (WeaponInfoArray[iId].iType == Wpn_Default || !WeaponInfoArray[iId].iForward[Fwd_Wpn_CanDeploy])
	{
#ifdef _WIN32
		return reinterpret_cast<int (__fastcall *)(void *, int)>(g_CrowbarHooks[CrowbarHook_CanDeploy].address)(pPrivate, 0);
#else
		return reinterpret_cast<int (*)(void *)>(g_CrowbarHooks[CrowbarHook_CanDeploy].address)(pPrivate);
#endif
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
		static_cast<cell>(Player_AmmoInventory(pPlayer, pWeapon, TRUE)),
		static_cast<cell>(Player_AmmoInventory(pPlayer, pWeapon, FALSE))
	);
}



#ifdef _WIN32
BOOL __fastcall Weapon_Deploy(void *pPrivate)
#else
BOOL Weapon_Deploy(void *pPrivate)
#endif
{
	edict_t* pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(pWeapon))
	{
		return FALSE;
	}

	int iId = GetPrivateInt(pWeapon, pvData_iId);

	if (WeaponInfoArray[iId].iType == Wpn_Default)
	{
#ifdef _WIN32
		return reinterpret_cast<int (__fastcall *)(void *, int)>(g_CrowbarHooks[CrowbarHook_Deploy].address)(pPrivate, NULL);
#else
		return reinterpret_cast<int (*)(void *)>(g_CrowbarHooks[CrowbarHook_Deploy].address)(pPrivate);
#endif
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
			static_cast<cell>(Player_AmmoInventory(pPlayer, pWeapon, TRUE)),
			static_cast<cell>(Player_AmmoInventory(pPlayer, pWeapon, FALSE))
		);
	}

	return iReturn;
}



#ifdef _WIN32
void __fastcall Weapon_ItemPostFrame(void *pPrivate)
#else
void Weapon_ItemPostFrame(void *pPrivate)
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

	pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(pWeapon))
	{
		return;
	}

	iId = GetPrivateInt(pWeapon, pvData_iId);

	if (WeaponInfoArray[iId].iType == Wpn_Default)
	{
#ifdef _WIN32
		reinterpret_cast<int (__fastcall *)(void *, int)>(g_CrowbarHooks[CrowbarHook_ItemPostFrame].address)(pPrivate, 0);
#else
		reinterpret_cast<int (*)(void *)>(g_CrowbarHooks[CrowbarHook_ItemPostFrame].address)(pPrivate);
#endif
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

	iAmmoPrimary		= Player_AmmoInventory(pPlayer, pWeapon, TRUE);
	iAmmoSecondary		= Player_AmmoInventory(pPlayer, pWeapon, FALSE);

	iClip		= GetPrivateInt(pWeapon, pvData_iClip);
	iInReload	= GetPrivateInt(pWeapon, pvData_fInReload);

	if (iInReload && flNextAttack <= gpGlobals->time)
	{
		// complete the reload. 
		int j = min(GetWeapon_MaxClip(iId) - iClip, iAmmoPrimary);	

		iClip += j;
		iAmmoPrimary -= j;

		// Add them to the clip
		SetPrivateInt(pWeapon, pvData_iClip, iClip);
		SetPrivateInt(pWeapon, pvData_fInReload, FALSE);

		Player_Set_AmmoInventory(pPlayer, pWeapon, TRUE, iAmmoPrimary);
	}

	if ((pPlayer->v.button & IN_ATTACK2) && flNextSecondaryAttack < 0.0)
	{
		if (GetWeapon_pszAmmo2(iId) && !iAmmoSecondary)
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
		if ((!iClip && GetWeapon_pszAmmo1(iId)) || (GetWeapon_MaxClip(iId) == -1 && !iAmmoPrimary ) )
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
	else if (pPlayer->v.button & IN_RELOAD && GetWeapon_MaxClip(iId) != -1 && !iInReload ) 
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

		if (!Weapon_IsUseable(pPrivate) && flNextPrimaryAttack < 0.0) 
		{
			// weapon isn't useable, switch.
			if (!(GetWeapon_Flags(iId) & ITEM_FLAG_NOAUTOSWITCHEMPTY) && GetNextBestWeapon(pPlayer, pWeapon))
			{
				SetPrivateFloat(pWeapon, pvData_flNextPrimaryAttack, 0.3);
				return;
			}
		}
		else
		{
			// weapon is useable. Reload if empty and weapon has waited as long as it has to after firing
			if (!iClip && !(GetWeapon_Flags(iId) & ITEM_FLAG_NOAUTORELOAD) && flNextPrimaryAttack < 0.0)
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



#ifdef _WIN32
BOOL __fastcall Weapon_IsUseable(void *pPrivate)
#else
BOOL Weapon_IsUseable(void *pPrivate)
#endif
{
	edict_t* pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(pWeapon))
	{
		return FALSE;
	}

	int iId = GetPrivateInt(pWeapon, pvData_iId);

	if (WeaponInfoArray[iId].iType == Wpn_Default || !WeaponInfoArray[iId].iForward[Fwd_Wpn_IsUseable])
	{
#ifdef _WIN32
		return reinterpret_cast<int (__fastcall *)(void *, int)>(g_CrowbarHooks[CrowbarHook_IsUseable].address)(pPrivate, 0);
#else
		return reinterpret_cast<int (*)(void *)>(g_CrowbarHooks[CrowbarHook_IsUseable].address)(pPrivate);
#endif
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
		static_cast<cell>(Player_AmmoInventory(pPlayer, pWeapon, TRUE)),
		static_cast<cell>(Player_AmmoInventory(pPlayer, pWeapon, FALSE))
	);
}



#ifdef _WIN32
BOOL __fastcall Weapon_CanHolster(void *pPrivate)
#else
BOOL Weapon_CanHolster(void *pPrivate)
#endif
{
	edict_t* pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(pWeapon))
	{
		return FALSE;
	}

	int iId = GetPrivateInt(pWeapon, pvData_iId);

	if (WeaponInfoArray[iId].iType == Wpn_Default || !WeaponInfoArray[iId].iForward[Fwd_Wpn_CanHolster])
	{
#ifdef _WIN32
		return reinterpret_cast<int (__fastcall *)(void *, int)>(g_CrowbarHooks[CrowbarHook_CanHolster].address)(pPrivate, 0);
#else
		return reinterpret_cast<int (*)(void *)>(g_CrowbarHooks[CrowbarHook_CanHolster].address)(pPrivate);
#endif
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
		static_cast<cell>(Player_AmmoInventory(pPlayer, pWeapon, TRUE)),
		static_cast<cell>(Player_AmmoInventory(pPlayer, pWeapon, FALSE))
	);
}



#ifdef _WIN32
void __fastcall Weapon_Holster(void *pPrivate, int i, int skiplocal)
#else
void Weapon_Holster(void *pPrivate, int skiplocal)
#endif
{
	edict_t* pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(pWeapon))
	{
		return;
	}

	int iId = GetPrivateInt(pWeapon, pvData_iId);

	if (WeaponInfoArray[iId].iType == Wpn_Default)
	{
#ifdef _WIN32
		reinterpret_cast<int (__fastcall *)(void *, int, int)>(g_CrowbarHooks[CrowbarHook_Holster].address)(pPrivate, 0, skiplocal);
#else
		reinterpret_cast<int (*)(void *, int)>(g_CrowbarHooks[CrowbarHook_Holster].address)(pPrivate, skiplocal);
#endif
		return;
	}

	SetEntForward(pWeapon, Think, NULL, NULL);
	SetEntForward(pWeapon, Touch, NULL, NULL);

	edict_t* pPlayer = GetPrivateCbase(pWeapon, pvData_pPlayer);

	if (IsValidPev(pPlayer) && WeaponInfoArray[iId].iForward[Fwd_Wpn_Holster])
	{
		MF_ExecuteForward
		(
			WeaponInfoArray[iId].iForward[Fwd_Wpn_Holster],

			static_cast<cell>(ENTINDEX(pWeapon)), 
			static_cast<cell>(ENTINDEX(pPlayer)), 
			static_cast<cell>(GetPrivateInt(pWeapon, pvData_iClip)), 
			static_cast<cell>(Player_AmmoInventory(pPlayer, pWeapon, TRUE)),
			static_cast<cell>(Player_AmmoInventory(pPlayer, pWeapon, FALSE))
		);
	}

	g_engfuncs.pfnSetClientKeyValue(ENTINDEX(pPlayer), g_engfuncs.pfnGetInfoKeyBuffer(pPlayer), "cl_lw", "1");
}



#ifdef _WIN32
int __fastcall Weapon_AddToPlayer(void *pPrivate, int i, void *pPrivate2)
#else
int Weapon_AddToPlayer(void *pPrivate, void *pPrivate2)
#endif
{
	edict_t* pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(pWeapon))
	{
		return 0;
	}

	int iId = GetPrivateInt(pWeapon, pvData_iId);
	edict_t* pPlayer = PrivateToEdict(pPrivate2);

	if (WeaponInfoArray[iId].iType == Wpn_Custom && IsValidPev(pPlayer))
	{
		if (!stricmp(STRING(pWeapon->v.classname), "weapon_crowbar"))
		{
			pWeapon->v.flags |= FL_KILLME;
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
					static_cast<cell>(Player_AmmoInventory(pPlayer, pWeapon, TRUE)),
					static_cast<cell>(Player_AmmoInventory(pPlayer, pWeapon, FALSE))
				)
			)
			{
				return FALSE;
			}
		}

		if (!cvar_aghlru)
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
				static_cast<cell>(Player_AmmoInventory(pPlayer, pWeapon, TRUE)),
				static_cast<cell>(Player_AmmoInventory(pPlayer, pWeapon, FALSE))
			);
		}
	}

#ifdef _WIN32
	return reinterpret_cast<int (__fastcall *)(void *, int, void *)>(g_CrowbarHooks[CrowbarHook_AddToPlayer].address)(pPrivate, 0, pPrivate2);
#else
	return reinterpret_cast<int (*)(void *, void *)>(g_CrowbarHooks[CrowbarHook_AddToPlayer].address)(pPrivate, pPrivate2);
#endif
}



#ifdef _WIN32
int __fastcall Weapon_ItemSlot(void *pPrivate)
#else
int Weapon_ItemSlot(void *pPrivate)
#endif
{
	edict_t* pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(pWeapon))
	{
		return 0;
	}

	int iId = GetPrivateInt(pWeapon, pvData_iId);

	if (WeaponInfoArray[iId].iType == Wpn_Custom)
	{
		return GetWeapon_Slot(iId) + 1;
	}

#ifdef _WIN32
	return reinterpret_cast<int (__fastcall *)(void *, int)>(g_CrowbarHooks[CrowbarHook_ItemSlot].address)(pPrivate, 0);
#else
	return reinterpret_cast<int (*)(void *)>(g_CrowbarHooks[CrowbarHook_ItemSlot].address)(pPrivate);
#endif
}



#ifdef _WIN32
void* __fastcall Weapon_Respawn(void *pPrivate)
#else
void* Weapon_Respawn(void *pPrivate)
#endif
{
	edict_t* pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(pWeapon))
	{
		return 0;
	}

	int iId = GetPrivateInt(pWeapon, pvData_iId);

	if (WeaponInfoArray[iId].iType == Wpn_Default)
	{
#ifdef _WIN32
		return reinterpret_cast<void* (__fastcall *)(void *, int)>(g_CrowbarHooks[CrowbarHook_Respawn].address)(pPrivate, NULL);
#else
		return reinterpret_cast<void* (*)(void *)>(g_CrowbarHooks[CrowbarHook_Respawn].address)(pPrivate);
#endif
	}

	edict_t* pItem = Weapon_Spawn(GetWeapon_pszName(iId), pWeapon->v.origin, pWeapon->v.angles);

	if (IsValidPev(pItem))
	{
		float flNextRespawn;

		if (cvar_mp_weaponstay->value && !(GetWeapon_Flags(iId) & ITEM_FLAG_LIMITINWORLD))
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



#ifdef _WIN32
BOOL __fastcall AmmoBox_AddAmmo(void *pPrivate, int i, void *pPrivateOther)
#else
BOOL AmmoBox_AddAmmo(void *pPrivate, void *pPrivateOther)
#endif
{
	edict_t* pAmmobox = PrivateToEdict(pPrivate);
	edict_t* pOther = PrivateToEdict(pPrivateOther);

	if (!IsValidPev(pAmmobox) || !IsValidPev(pOther))
	{
		return FALSE;
	}

	if (!stricmp(STRING(pAmmobox->v.classname), "ammo_rpgclip"))
	{
		for (int k = 0; k < (int)g_BlockedItems.size(); k++)
		{
			if (!stricmp(g_BlockedItems[k]->classname, "ammo_rpgclip"))
			{
				pAmmobox->v.flags |= FL_KILLME;
				return FALSE;
			}
		}

#ifdef _WIN32
		return reinterpret_cast<BOOL (__fastcall *)(void *, int, void *)>(g_RpgAddAmmo_Hook.address)(pPrivate, i, pPrivateOther);
#else
		return reinterpret_cast<BOOL (*)(void *, void *)>(g_RpgAddAmmo_Hook.address)(pPrivate, pPrivateOther);
#endif
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



#ifdef _WIN32
int __fastcall Item_Block(void *pPrivate, int i, void *pPrivate2)
#else
int Item_Block(void *pPrivate, void *pPrivate2)
#endif
{
	edict_t* pWeapon = PrivateToEdict(pPrivate);

	if (IsValidPev(pWeapon))
	{
		pWeapon->v.flags |= FL_KILLME;
	}

	return FALSE;
}



#ifdef _WIN32
void __fastcall World_Precache(void *pPrivate)
#else
void World_Precache(void *pPrivate)
#endif
{
	SetConfigFile();

	if (ParseConfigSection(g_ConfigFilepath, "[block]", (void*)ParseBlockItems_Handler))
	{
		print_srvconsole("\n[WEAPONMOD] default items blocked:\n");

		for (int i = 0; i < (int)g_BlockedItems.size(); i++)
		{
			print_srvconsole("   %s\n", g_BlockedItems[i]->classname);
		}

		print_srvconsole("\n");
	}
	
#ifdef _WIN32
	reinterpret_cast<int (__fastcall *)(void *, int)>(g_WorldPrecache_Hook.address)(pPrivate, 0);
#else
	reinterpret_cast<int (*)(void *)>(g_WorldPrecache_Hook.address)(pPrivate);
#endif
}



#ifdef _WIN32
void __fastcall Equipment_Think(void *pPrivate)
#else
void Equipment_Think(void *pPrivate)
#endif
{
	edict_t* pEntity = PrivateToEdict(pPrivate);

	if (!IsValidPev(pEntity) || !IsValidPev(g_EquipEnt))
	{
		return;
	}

	int iPlayer = GetPrivateInt(pEntity, pvData_fireState);

	if (MF_IsPlayerValid(iPlayer) && MF_IsPlayerAlive(iPlayer))
	{
		MDLL_Touch(g_EquipEnt, INDEXENT(iPlayer));
		pEntity->v.flags |= FL_KILLME;
	}
}



#ifdef _WIN32
void __fastcall Player_Spawn(void *pPrivate)
#else
void Player_Spawn(void *pPrivate)
#endif
{
#ifdef _WIN32
	reinterpret_cast<void (__fastcall *)(void *, int)>(g_PlayerSpawn_Hook.address)(pPrivate, 0);
#else
	reinterpret_cast<void (*)(void *)>(g_PlayerSpawn_Hook.address)(pPrivate);
#endif

	edict_t* pPlayer = PrivateToEdict(pPrivate);

	if (!IsValidPev(pPlayer))
	{
		return;
	}

	if (IsValidPev(g_EquipEnt) && pPlayer->v.movetype != MOVETYPE_NOCLIP)
	{
		edict_t* pTaskEnt = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));

		if (IsValidPev(pTaskEnt))
		{
			SetThink_(pTaskEnt, (void*)Equipment_Think);
			SetPrivateInt(pTaskEnt, pvData_fireState, ENTINDEX(pPlayer));

			pTaskEnt->v.classname = MAKE_STRING("equipment_task");
			pTaskEnt->v.nextthink = gpGlobals->time + 0.08;
		}
	}

	int iAmmoIndex;

	for (int i = 0; i < (int)g_StartAmmo.size(); i++)
	{
		iAmmoIndex = GET_AMMO_INDEX(g_StartAmmo[i]->ammoname);

		if (iAmmoIndex != -1)
		{
			SetAmmoInventory(pPlayer, iAmmoIndex - 1, g_StartAmmo[i]->count);
		}
	}
}



#ifdef _WIN32
void __cdecl PrecacheOtherWeapon_HookHandler(const char *szClassname)
#else
void PrecacheOtherWeapon_HookHandler(const char *szClassname)
#endif
{
	edict_t	*pEntity = CREATE_NAMED_ENTITY(MAKE_STRING(szClassname));
	
	if (IsValidPev(pEntity))
	{
		for (int i = 0; i < (int)g_BlockedItems.size(); i++)
		{
			if (!stricmp(g_BlockedItems[i]->classname,szClassname))
			{
				MDLL_Spawn(pEntity);
				REMOVE_ENTITY(pEntity);
				return;
			}
		}

		ItemInfo pII;
		GET_ITEM_INFO(pEntity, &pII);

		WeaponInfoArray[pII.iId].ItemData = pII;
		WeaponInfoArray[pII.iId].iType = Wpn_Default;

		g_iCurrentSlots[pII.iSlot][pII.iPosition] = TRUE;
		REMOVE_ENTITY(pEntity);

		g_iWeaponsCount++;
	}

	UnsetHook(&g_dllFuncs[Func_PrecacheOtherWeapon]);
#ifdef _WIN32
	reinterpret_cast<int (__cdecl *)(const char *)>(g_dllFuncs[Func_PrecacheOtherWeapon].address)(szClassname);
#else
	reinterpret_cast<int (*)(const char *)>(g_dllFuncs[Func_PrecacheOtherWeapon].address)(szClassname);
#endif
	SetHook(&g_dllFuncs[Func_PrecacheOtherWeapon]);
}



#ifdef _WIN32
void __fastcall GiveNamedItem_HookHandler(void *pPrivate, int i, const char *szName)
#else
void GiveNamedItem_HookHandler(void *pPrivate, const char *szName)
#endif
{
	if (szName)
	{
		GiveNamedItem(PrivateToEdict(pPrivate), szName);
	}

	UnsetHook(&g_dllFuncs[Func_GiveNamedItem]);
#ifdef _WIN32
	reinterpret_cast<int (__fastcall *)(void *, int, const char *)>(g_dllFuncs[Func_GiveNamedItem].address)(pPrivate, i, szName);
#else
	reinterpret_cast<int (*)(void *, const char *)>(g_dllFuncs[Func_GiveNamedItem].address)(pPrivate, szName);
#endif
	SetHook(&g_dllFuncs[Func_GiveNamedItem]);
}



#ifdef _WIN32
void __fastcall CheatImpulseCommands_HookHandler(void *pPrivate, int i, int iImpulse)
#else
void CheatImpulseCommands_HookHandler(void *pPrivate, int iImpulse)
#endif
{
	// check cheat impulse command now
	if (iImpulse == 101 && cvar_sv_cheats->value)
	{
		edict_t *pPlayer = PrivateToEdict(pPrivate);

		for (int k = 1; k <= g_iWeaponsCount; k++)
		{
			GiveNamedItem(pPlayer, GetWeapon_pszName(k));
		}

		for (int k = 1; k <= g_iAmmoBoxIndex; k++)
		{
			GiveNamedItem(pPlayer, AmmoBoxInfoArray[k].classname.c_str());
		}
	}

	UnsetHook(&g_dllFuncs[Func_CheatImpulseCommands]);
#ifdef _WIN32
	reinterpret_cast<int (__fastcall *)(void *, int, int)>(g_dllFuncs[Func_CheatImpulseCommands].address)(pPrivate, i, iImpulse);
#else
	reinterpret_cast<int (*)(void *, int)>(g_dllFuncs[Func_CheatImpulseCommands].address)(pPrivate, iImpulse);
#endif
	SetHook(&g_dllFuncs[Func_CheatImpulseCommands]);
}



#ifdef _WIN32
void __fastcall Global_Think(void *pPrivate)
#else
void Global_Think(void *pPrivate)
#endif
{
	static edict_t* pEntity;
	
	pEntity = PrivateToEdict(pPrivate);

	if (!IsValidPev(pEntity))
	{
		return;
	}

	int iThinkForward = g_Ents[ENTINDEX(pEntity)].iThink;

	if (iThinkForward)
	{
		if (!strstr(STRING(pEntity->v.classname), "weapon_"))
		{
			MF_ExecuteForward
			(
				iThinkForward,

				static_cast<cell>(ENTINDEX(pEntity)), 
				static_cast<cell>(0), 
				static_cast<cell>(0), 
				static_cast<cell>(0),
				static_cast<cell>(0)
			);
		}
		else
		{
			edict_t* pPlayer = GetPrivateCbase(pEntity, pvData_pPlayer);

			MF_ExecuteForward
			(
				iThinkForward,

				static_cast<cell>(ENTINDEX(pEntity)), 
				static_cast<cell>(ENTINDEX(pPlayer)), 
				static_cast<cell>(GetPrivateInt(pEntity, pvData_iClip)), 
				static_cast<cell>(Player_AmmoInventory(pPlayer, pEntity, TRUE)),
				static_cast<cell>(Player_AmmoInventory(pPlayer, pEntity, FALSE))
			);
		}
	}
}



#ifdef _WIN32
void __fastcall Global_Touch(void *pPrivate, int i, void *pPrivate2)
#else
void Global_Touch(void *pPrivate, void *pPrivate2)
#endif
{
	static edict_t* pEntity;
	static edict_t* pOther;

	pEntity = PrivateToEdict(pPrivate);
	pOther = PrivateToEdict(pPrivate2);

	if (!IsValidPev(pEntity))
	{
		return;
	}

	int iTouchForward = g_Ents[ENTINDEX(pEntity)].iTouch;

	if (iTouchForward)
	{
		MF_ExecuteForward
		(
			iTouchForward,

			static_cast<cell>(ENTINDEX(pEntity)), 
			static_cast<cell>(ENTINDEX(pOther))
		);
	}
}



edict_t* Weapon_Spawn(const char* szName, Vector vecOrigin, Vector vecAngles)
{
	if (!szName)
	{
		return NULL;
	}

	int iId = 0;
	
	for (int i = 1; i <= g_iWeaponsCount; i++)
	{
		if (WeaponInfoArray[i].iType == Wpn_Custom && !_stricmp(GetWeapon_pszName(i), szName))
		{
			iId = i;
			break;
		}
	}

	if (!iId)
	{
		return NULL;
	}

	edict_t* pItem = CREATE_NAMED_ENTITY(MAKE_STRING("weapon_crowbar"));

	if (IsValidPev(pItem))
	{
		MDLL_Spawn(pItem);
		SET_ORIGIN(pItem, vecOrigin);

		pItem->v.classname = MAKE_STRING(GetWeapon_pszName(iId));
		pItem->v.angles = vecAngles;

		SetPrivateInt(pItem, pvData_iId, iId);

		if (GetWeapon_MaxClip(iId) != -1)
		{
			SetPrivateInt(pItem, pvData_iClip, 0);
		}

		if (WeaponInfoArray[iId].iForward[Fwd_Wpn_Spawn])
		{
			MF_ExecuteForward
			(
				WeaponInfoArray[iId].iForward[Fwd_Wpn_Spawn],

				static_cast<cell>(ENTINDEX(pItem)), 
				static_cast<cell>(0), 
				static_cast<cell>(0), 
				static_cast<cell>(0),
				static_cast<cell>(0)
			);
		}	

		return pItem;
	}

	return NULL;
}



edict_t* Ammo_Spawn(const char* szName, Vector vecOrigin, Vector vecAngles)
{
	if (!szName)
	{
		return NULL;
	}

	int iId = 0;
	
	for (int i = 1; i <= g_iAmmoBoxIndex; i++)
	{
		if (!_stricmp(AmmoBoxInfoArray[i].classname.c_str(), szName))
		{
			iId = i;
			break;
		}
	}

	if (!iId)
	{
		return NULL;
	}

	edict_t* pAmmoBox = CREATE_NAMED_ENTITY(MAKE_STRING("ammo_rpgclip"));

	if (IsValidPev(pAmmoBox))
	{
		MDLL_Spawn(pAmmoBox);
		SET_ORIGIN(pAmmoBox, vecOrigin);
		
		pAmmoBox->v.classname = MAKE_STRING(AmmoBoxInfoArray[iId].classname.c_str());
		pAmmoBox->v.angles = vecAngles;

		if (AmmoBoxInfoArray[iId].iForward[Fwd_Ammo_Spawn])
		{
			MF_ExecuteForward
			(
				AmmoBoxInfoArray[iId].iForward[Fwd_Ammo_Spawn],
				static_cast<cell>(ENTINDEX(pAmmoBox)),
				static_cast<cell>(0)
			);
		}

		//SET_SIZE(pAmmoBox, Vector(-16, -16, 0), Vector(16, 16, 16));
		return pAmmoBox;
	}

	return NULL;
}


void SetVDataOffsets()
{
	g_RpgAddAmmo_Hook.offset = g_vtblOffsets[VO_AddAmmo];
	g_PlayerSpawn_Hook.offset = g_vtblOffsets[VO_Spawn];
	g_WorldPrecache_Hook.offset = g_vtblOffsets[VO_Precache];

	g_CrowbarHooks[CrowbarHook_Respawn].offset = g_vtblOffsets[VO_Respawn];
	g_CrowbarHooks[CrowbarHook_AddToPlayer].offset = g_vtblOffsets[VO_AddToPlayer];
	g_CrowbarHooks[CrowbarHook_GetItemInfo].offset = g_vtblOffsets[VO_GetItemInfo];
	g_CrowbarHooks[CrowbarHook_CanDeploy].offset = g_vtblOffsets[VO_CanDeploy];
	g_CrowbarHooks[CrowbarHook_Deploy].offset = g_vtblOffsets[VO_Deploy];
	g_CrowbarHooks[CrowbarHook_CanHolster].offset = g_vtblOffsets[VO_CanHolster];
	g_CrowbarHooks[CrowbarHook_Holster].offset = g_vtblOffsets[VO_Holster];
	g_CrowbarHooks[CrowbarHook_ItemPostFrame].offset = g_vtblOffsets[VO_ItemPostFrame];
	g_CrowbarHooks[CrowbarHook_ItemSlot].offset = g_vtblOffsets[VO_ItemSlot];
	g_CrowbarHooks[CrowbarHook_IsUseable].offset = g_vtblOffsets[VO_IsUseable];
}
