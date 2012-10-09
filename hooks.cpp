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

int g_iId;

edict_t* g_pEntity;
edict_t* g_pWeapon;
edict_t* g_pPlayer;

module hl_dll = {NULL, 0, NULL};

VirtHookData g_RpgAddAmmo_Hook =
{
	"ammo_rpgclip", VOffset_AddAmmo, NULL, NULL, (void*)AmmoBox_AddAmmo
};

VirtHookData g_WorldPrecache_Hook = 
{
	"worldspawn", VOffset_Precache, NULL, NULL, (void*)World_Precache
};

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
	{
		"RadiusDamage__FG6VectorP9entvars_sT1ffii", 
		&hl_dll,
		{
			"\xD9\x44\x24\x1C\xD8\x00\x00\x00\x00\x00\x83\xEC\x64", 
			"xxxxx?????xxx", 13
		},
		{
			"\x83\x00\x00\xD9\x00\xD9\x00\x00\x00\xD9\x00\x00\x00\xD9\x00\x00\x00\xD9\x00\x00\x00\xD9\x00\x00\x00\xD9\x00\x00\x00",
			"x??x?x???x???x???x???x???x???", 29
		},
		NULL, NULL, {}, {}, 0
	},
	{
		"GetAmmoIndex__11CBasePlayerPCc", 
		&hl_dll,
		{
			"\x56\x57\x8B\x7C\x24\x0C\x85\xFF", 
			"xxxxxxxx", 8
		},
		{
			"\x57\x8B\x7C\x24\x08\x85\xFF\x75\x05", 
			"xxxxxxxxx", 9
		},
		NULL, NULL, {}, {}, 0
	},
	{
		"ClearMultiDamage__Fv", 
		&hl_dll,
		{
			"\x33\xC0\xA3\x00\x00\x00\x00\xA3\x00\x00\x00\x00",
			"xxx????x????", 12
		},
		{
			"\xD9\xEE\x33\xC0\xD9\x00\x00\x00\x00\x00\xA3\x00\x00\x00\x00", 
			"xxxxx?????x????", 15
		},
		NULL, NULL, {}, {}, 0
	},
	{
		"ApplyMultiDamage__FP9entvars_sT0", 
		&hl_dll,
		{
			"\x8B\x0D\x00\x00\x00\x00\x85\xC9\x74\x1D",
			"xx????xxxx", 10
		},
		{
			"\x8B\x0D\x00\x00\x00\x00\x85\xC9\x74\x22", 
			"xx????xxxx", 10
		},
		NULL, NULL, {}, {}, 0
	},
	{
		"SetAnimation__11CBasePlayer11PLAYER_ANIM",
		&hl_dll,
		{
			"\x83\xEC\x44\x53\x55\x8B\xE9\x33\xDB\x56\x57", 
			"xxxxxxxxxxx", 11
		},
		{
			"\x83\xEC\x00\xA1\x00\x00\x00\x00\x33\xC4\x89\x00\x00\x00\x53\x56\x8B\xD9",
			"xx?x????xxx???xxxx", 18
		},
		NULL, NULL, {}, {}, 0
	},
	{
		"UTIL_PrecacheOtherWeapon__FPCc", 
		&hl_dll,
		{
			"\x8B\x00\x00\x00\x00\x00\x8B\x00\x00\x00\x83\x00\x00\x53\x56\x2B\x00\x00\x00\x00\x00\x50",
			"x?????x???x??xxx?????x", 22
		},
		{
			"\x8B\x00\x00\x00\x00\x00\x8B\x00\x00\x00\x2B\x00\x00\x00\x00\x00\x83\x00\x00\x53\x50",
			"x?????x???x?????x??xx", 21,
		},
		NULL, (void*)PrecacheOtherWeapon_HookHandler, {}, {}, 0
	},
	{
		"GiveNamedItem__11CBasePlayerPCc", 
		&hl_dll,
		{
			"\x8B\x44\x00\x00\x56\x57\x8B\xF9\x8B\x0D\x00\x00\x00\x00",
			"xx??xxxxxx????", 14
		},
		{
			"\x8B\x44\x00\x00\x56\x57\x8B\xF9\x8B\x0D\x00\x00\x00\x00",
			"xx??xxxxxx????", 14
		},
		NULL, (void*)GiveNamedItem_HookHandler, {}, {}, 0
	},
	{
		"CheatImpulseCommands__11CBasePlayeri", 
		&hl_dll,
		{
			"\xD9\x00\x00\x00\x00\x00\xDC\x00\x00\x00\x00\x00\x81\x00\x00\x00\x00\x00\x56\x57",
			"x?????x?????x?????xx", 20
		},
		{
			"\xD9\xEE\x81\xEC\x00\x00\x00\x00\xD8\x00\x00\x00\x00\x00\x56",
			"xxxx????x?????x", 15
		},
		NULL, 
		(void*)CheatImpulseCommands_HookHandler, {}, {}, 0
	}
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

	g_pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(g_pWeapon))
	{
		return 0;
	}

	g_iId = (int)*((int *)g_pWeapon->pvPrivateData + m_iId);

	if (g_iWeaponInitID)
	{
		g_iId = g_iWeaponInitID;
		g_iWeaponInitID = 0;
	}

	if (WeaponInfoArray[g_iId].iType == Wpn_Custom)
	{
		p->iId = g_iId;
		p->pszName = GetWeapon_pszName(g_iId);
		p->iSlot = GetWeapon_Slot(g_iId);
		p->iPosition = GetWeapon_ItemPosition(g_iId);
		p->iMaxAmmo1 = GetWeapon_MaxAmmo1(g_iId);
		p->iMaxAmmo2 = GetWeapon_MaxAmmo2(g_iId);
		p->iMaxClip = GetWeapon_MaxClip(g_iId);
		p->iFlags = GetWeapon_Flags(g_iId);
		p->iWeight = GetWeapon_Weight(g_iId);

		if (GetWeapon_pszAmmo1(g_iId)[0])
		{
			p->pszAmmo1 = GetWeapon_pszAmmo1(g_iId);
		}

		if (GetWeapon_pszAmmo2(g_iId)[0])
		{
			p->pszAmmo2 = GetWeapon_pszAmmo2(g_iId);
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
	g_pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(g_pWeapon))
	{
		return FALSE;
	}

	g_iId = (int)*((int *)g_pWeapon->pvPrivateData + m_iId);

	if (WeaponInfoArray[g_iId].iType == Wpn_Default || !WeaponInfoArray[g_iId].iForward[Fwd_Wpn_CanDeploy])
	{
#ifdef _WIN32
		return reinterpret_cast<int (__fastcall *)(void *, int)>(g_CrowbarHooks[CrowbarHook_CanDeploy].address)(pPrivate, 0);
#else
		return reinterpret_cast<int (*)(void *)>(g_CrowbarHooks[CrowbarHook_CanDeploy].address)(pPrivate);
#endif
	}

	g_pPlayer = GetPrivateCbase(g_pWeapon, m_pPlayer);

	if (!IsValidPev(g_pPlayer))
	{
		return FALSE;
	}

	return MF_ExecuteForward
	(
		WeaponInfoArray[g_iId].iForward[Fwd_Wpn_CanDeploy],

		static_cast<cell>(ENTINDEX(g_pWeapon)), 
		static_cast<cell>(ENTINDEX(g_pPlayer)), 
		static_cast<cell>((int)*((int *)g_pWeapon->pvPrivateData + m_iClip)), 
		static_cast<cell>(Player_AmmoInventory(g_pPlayer, g_pWeapon, TRUE)),
		static_cast<cell>(Player_AmmoInventory(g_pPlayer, g_pWeapon, FALSE))
	);
}

#ifdef _WIN32
BOOL __fastcall Weapon_Deploy(void *pPrivate)
#else
BOOL Weapon_Deploy(void *pPrivate)
#endif
{
	g_pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(g_pWeapon))
	{
		return FALSE;
	}

	g_iId = (int)*((int *)g_pWeapon->pvPrivateData + m_iId);

	if (WeaponInfoArray[g_iId].iType == Wpn_Default)
	{
#ifdef _WIN32
		return reinterpret_cast<int (__fastcall *)(void *, int)>(g_CrowbarHooks[CrowbarHook_Deploy].address)(pPrivate, NULL);
#else
		return reinterpret_cast<int (*)(void *)>(g_CrowbarHooks[CrowbarHook_Deploy].address)(pPrivate);
#endif
	}

	g_pPlayer = GetPrivateCbase(g_pWeapon, m_pPlayer);

	int iReturn = FALSE;

	if (!IsValidPev(g_pPlayer))
	{
		return iReturn;
	}

	g_engfuncs.pfnSetClientKeyValue(ENTINDEX(g_pPlayer), g_engfuncs.pfnGetInfoKeyBuffer(g_pPlayer), "cl_lw", "0");

	if (WeaponInfoArray[g_iId].iForward[Fwd_Wpn_Deploy])
	{
		iReturn = MF_ExecuteForward
		(
			WeaponInfoArray[g_iId].iForward[Fwd_Wpn_Deploy],

			static_cast<cell>(ENTINDEX(g_pWeapon)), 
			static_cast<cell>(ENTINDEX(g_pPlayer)), 
			static_cast<cell>((int)*((int *)g_pWeapon->pvPrivateData + m_iClip)), 
			static_cast<cell>(Player_AmmoInventory(g_pPlayer, g_pWeapon, TRUE)),
			static_cast<cell>(Player_AmmoInventory(g_pPlayer, g_pWeapon, FALSE))
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
	static int iClip;
	static int iInReload;

	static int iAmmoPrimary;
	static int iAmmoSecondary;

	static float flNextAttack;
	static float flNextPrimaryAttack;
	static float flNextSecondaryAttack;

	g_pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(g_pWeapon))
	{
		return;
	}

	g_iId = (int)*((int *)g_pWeapon->pvPrivateData + m_iId);

	if (WeaponInfoArray[g_iId].iType == Wpn_Default)
	{
#ifdef _WIN32
		reinterpret_cast<int (__fastcall *)(void *, int)>(g_CrowbarHooks[CrowbarHook_ItemPostFrame].address)(pPrivate, 0);
#else
		reinterpret_cast<int (*)(void *)>(g_CrowbarHooks[CrowbarHook_ItemPostFrame].address)(pPrivate);
#endif
		return;
	}

	g_pPlayer = GetPrivateCbase(g_pWeapon, m_pPlayer);

	if (!IsValidPev(g_pPlayer))
	{
		return;
	}

	flNextAttack = (float)*((float *)g_pPlayer->pvPrivateData + m_flNextAttack);
	flNextPrimaryAttack = (float)*((float *)g_pWeapon->pvPrivateData + m_flNextPrimaryAttack);
	flNextSecondaryAttack = (float)*((float *)g_pWeapon->pvPrivateData + m_flNextSecondaryAttack);

	iAmmoPrimary = Player_AmmoInventory(g_pPlayer, g_pWeapon, TRUE);
	iAmmoSecondary = Player_AmmoInventory(g_pPlayer, g_pWeapon, FALSE);

	iClip = (int)*((int *)g_pWeapon->pvPrivateData + m_iClip);
	iInReload = (int)*((int *)g_pWeapon->pvPrivateData + m_fInReload);

	if (iInReload && flNextAttack <= gpGlobals->time)
	{
		// complete the reload. 
		int j = min(GetWeapon_MaxClip(g_iId) - iClip, iAmmoPrimary);	

		iClip += j;
		iAmmoPrimary -= j;

		// Add them to the clip
		*((int *)g_pWeapon->pvPrivateData + m_iClip) = iClip;
		*((int *)g_pWeapon->pvPrivateData + m_fInReload) = FALSE;	

		Player_Set_AmmoInventory(g_pPlayer, g_pWeapon, TRUE, iAmmoPrimary);
	}

	if ((g_pPlayer->v.button & IN_ATTACK2) && flNextSecondaryAttack < 0.0)
	{
		if (GetWeapon_pszAmmo2(g_iId) && !iAmmoSecondary)
		{
			*((int *)g_pWeapon->pvPrivateData + m_fFireOnEmpty) = TRUE;
		}

		if (WeaponInfoArray[g_iId].iForward[Fwd_Wpn_SecondaryAttack])
		{
			MF_ExecuteForward
			(
				WeaponInfoArray[g_iId].iForward[Fwd_Wpn_SecondaryAttack],

				static_cast<cell>(ENTINDEX(g_pWeapon)), 
				static_cast<cell>(ENTINDEX(g_pPlayer)), 
				static_cast<cell>(iClip), 
				static_cast<cell>(iAmmoPrimary),
				static_cast<cell>(iAmmoSecondary)
			);
		}

		g_pPlayer->v.button &= ~IN_ATTACK2;
	}
	else if ((g_pPlayer->v.button & IN_ATTACK) && flNextPrimaryAttack < 0.0)
	{
		if ((!iClip && GetWeapon_pszAmmo1(g_iId)) || (GetWeapon_MaxClip(g_iId) == -1 && !iAmmoPrimary ) )
		{
			*((int *)g_pWeapon->pvPrivateData + m_fFireOnEmpty) = TRUE;
		}

		if (WeaponInfoArray[g_iId].iForward[Fwd_Wpn_PrimaryAttack])
		{
			MF_ExecuteForward
			(
				WeaponInfoArray[g_iId].iForward[Fwd_Wpn_PrimaryAttack],

				static_cast<cell>(ENTINDEX(g_pWeapon)), 
				static_cast<cell>(ENTINDEX(g_pPlayer)), 
				static_cast<cell>(iClip), 
				static_cast<cell>(iAmmoPrimary),
				static_cast<cell>(iAmmoSecondary)
			);
		}

		g_pPlayer->v.button &= ~IN_ATTACK;
	}
	else if (g_pPlayer->v.button & IN_RELOAD && GetWeapon_MaxClip(g_iId) != -1 && !iInReload ) 
	{
		// reload when reload is pressed, or if no buttons are down and weapon is empty.
		if (WeaponInfoArray[g_iId].iForward[Fwd_Wpn_Reload])
		{
			MF_ExecuteForward
			(
				WeaponInfoArray[g_iId].iForward[Fwd_Wpn_Reload],

				static_cast<cell>(ENTINDEX(g_pWeapon)), 
				static_cast<cell>(ENTINDEX(g_pPlayer)), 
				static_cast<cell>(iClip), 
				static_cast<cell>(iAmmoPrimary),
				static_cast<cell>(iAmmoSecondary)
			);
		}
	}
	else if (!(g_pPlayer->v.button & (IN_ATTACK | IN_ATTACK2)))
	{
		// no fire buttons down

		*((int *)g_pWeapon->pvPrivateData + m_fFireOnEmpty) = FALSE;

		if (iClip <= 0 && iAmmoPrimary <= 0 && GetWeapon_MaxAmmo1(g_iId) != -1 && flNextPrimaryAttack < 0.0) 
		{
			// weapon isn't useable, switch.
#ifdef _WIN32
			reinterpret_cast<int (__fastcall *)(void *, int)>(g_CrowbarHooks[CrowbarHook_ItemPostFrame].address)(pPrivate, 0);
#else
			reinterpret_cast<int (*)(void *)>(g_CrowbarHooks[CrowbarHook_ItemPostFrame].address)(pPrivate);
#endif
			return;
		}
		else
		{
			// weapon is useable. Reload if empty and weapon has waited as long as it has to after firing
			if (!iClip && !(GetWeapon_Flags(g_iId) & ITEM_FLAG_NOAUTORELOAD) && flNextPrimaryAttack < 0.0)
			{
				if (WeaponInfoArray[g_iId].iForward[Fwd_Wpn_Reload])
				{
					MF_ExecuteForward
					(
						WeaponInfoArray[g_iId].iForward[Fwd_Wpn_Reload],

						static_cast<cell>(ENTINDEX(g_pWeapon)), 
						static_cast<cell>(ENTINDEX(g_pPlayer)), 
						static_cast<cell>(iClip), 
						static_cast<cell>(iAmmoPrimary),
						static_cast<cell>(iAmmoSecondary)
					);
				}
				return;
			}
		}

		if (WeaponInfoArray[g_iId].iForward[Fwd_Wpn_Idle])
		{
			MF_ExecuteForward
			(
				WeaponInfoArray[g_iId].iForward[Fwd_Wpn_Idle],

				static_cast<cell>(ENTINDEX(g_pWeapon)), 
				static_cast<cell>(ENTINDEX(g_pPlayer)), 
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
	g_pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(g_pWeapon))
	{
		return FALSE;
	}

	g_iId = (int)*((int *)g_pWeapon->pvPrivateData + m_iId);

	if (WeaponInfoArray[g_iId].iType == Wpn_Default || !WeaponInfoArray[g_iId].iForward[Fwd_Wpn_IsUseable])
	{
#ifdef _WIN32
		return reinterpret_cast<int (__fastcall *)(void *, int)>(g_CrowbarHooks[CrowbarHook_IsUseable].address)(pPrivate, 0);
#else
		return reinterpret_cast<int (*)(void *)>(g_CrowbarHooks[CrowbarHook_IsUseable].address)(pPrivate);
#endif
	}

	g_pPlayer = GetPrivateCbase(g_pWeapon, m_pPlayer);

	if (!IsValidPev(g_pPlayer))
	{
		return FALSE;
	}

	return MF_ExecuteForward
	(
		WeaponInfoArray[g_iId].iForward[Fwd_Wpn_IsUseable],

		static_cast<cell>(ENTINDEX(g_pWeapon)), 
		static_cast<cell>(ENTINDEX(g_pPlayer)), 
		static_cast<cell>((int)*((int *)g_pWeapon->pvPrivateData + m_iClip)), 
		static_cast<cell>(Player_AmmoInventory(g_pPlayer, g_pWeapon, TRUE)),
		static_cast<cell>(Player_AmmoInventory(g_pPlayer, g_pWeapon, FALSE))
	);
}

#ifdef _WIN32
BOOL __fastcall Weapon_CanHolster(void *pPrivate)
#else
BOOL Weapon_CanHolster(void *pPrivate)
#endif
{
	g_pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(g_pWeapon))
	{
		return FALSE;
	}

	g_iId = (int)*((int *)g_pWeapon->pvPrivateData + m_iId);

	if (WeaponInfoArray[g_iId].iType == Wpn_Default || !WeaponInfoArray[g_iId].iForward[Fwd_Wpn_CanHolster])
	{
#ifdef _WIN32
		return reinterpret_cast<int (__fastcall *)(void *, int)>(g_CrowbarHooks[CrowbarHook_CanHolster].address)(pPrivate, 0);
#else
		return reinterpret_cast<int (*)(void *)>(g_CrowbarHooks[CrowbarHook_CanHolster].address)(pPrivate);
#endif
	}

	g_pPlayer = GetPrivateCbase(g_pWeapon, m_pPlayer);

	if (!IsValidPev(g_pPlayer))
	{
		return FALSE;
	}

	return MF_ExecuteForward
	(
		WeaponInfoArray[g_iId].iForward[Fwd_Wpn_CanHolster],

		static_cast<cell>(ENTINDEX(g_pWeapon)), 
		static_cast<cell>(ENTINDEX(g_pPlayer)), 
		static_cast<cell>((int)*((int *)g_pWeapon->pvPrivateData + m_iClip)), 
		static_cast<cell>(Player_AmmoInventory(g_pPlayer, g_pWeapon, TRUE)),
		static_cast<cell>(Player_AmmoInventory(g_pPlayer, g_pWeapon, FALSE))
	);
}

#ifdef _WIN32
void __fastcall Weapon_Holster(void *pPrivate, int i, int skiplocal)
#else
void Weapon_Holster(void *pPrivate, int skiplocal)
#endif
{
	g_pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(g_pWeapon))
	{
		return;
	}

	g_iId = (int)*((int *)g_pWeapon->pvPrivateData + m_iId);

	if (WeaponInfoArray[g_iId].iType == Wpn_Default)
	{
#ifdef _WIN32
		reinterpret_cast<int (__fastcall *)(void *, int, int)>(g_CrowbarHooks[CrowbarHook_Holster].address)(pPrivate, 0, skiplocal);
#else
		reinterpret_cast<int (*)(void *, int)>(g_CrowbarHooks[CrowbarHook_Holster].address)(pPrivate, skiplocal);
#endif
		return;
	}

	SetEntForward(g_pWeapon, Think, NULL, NULL);
	SetEntForward(g_pWeapon, Touch, NULL, NULL);

	g_pPlayer = GetPrivateCbase(g_pWeapon, m_pPlayer);

	if (IsValidPev(g_pPlayer) && WeaponInfoArray[g_iId].iForward[Fwd_Wpn_Holster])
	{
		MF_ExecuteForward
		(
			WeaponInfoArray[g_iId].iForward[Fwd_Wpn_Holster],

			static_cast<cell>(ENTINDEX(g_pWeapon)), 
			static_cast<cell>(ENTINDEX(g_pPlayer)), 
			static_cast<cell>((int)*((int *)g_pWeapon->pvPrivateData + m_iClip)), 
			static_cast<cell>(Player_AmmoInventory(g_pPlayer, g_pWeapon, TRUE)),
			static_cast<cell>(Player_AmmoInventory(g_pPlayer, g_pWeapon, FALSE))
		);
	}

	g_engfuncs.pfnSetClientKeyValue(ENTINDEX(g_pPlayer), g_engfuncs.pfnGetInfoKeyBuffer(g_pPlayer), "cl_lw", "1");
}

#ifdef _WIN32
int __fastcall Weapon_AddToPlayer(void *pPrivate, int i, void *pPrivate2)
#else
int Weapon_AddToPlayer(void *pPrivate, void *pPrivate2)
#endif
{
	g_pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(g_pWeapon))
	{
		return 0;
	}

	g_iId = (int)*((int *)g_pWeapon->pvPrivateData + m_iId);
	g_pPlayer = PrivateToEdict(pPrivate2);

	if (WeaponInfoArray[g_iId].iType == Wpn_Custom && IsValidPev(g_pPlayer))
	{
		if (!_strcmpi(STRING(g_pWeapon->v.classname), "weapon_crowbar"))
		{
			g_pWeapon->v.flags |= FL_KILLME;
			return 0;
		}

		if (!cvar_aghlru)
		{
			static int msgWeapPickup = 0;
			if (msgWeapPickup || (msgWeapPickup = REG_USER_MSG( "WeapPickup", 1 )))		
			{
				MESSAGE_BEGIN(MSG_ONE, msgWeapPickup, NULL, g_pPlayer);
				WRITE_BYTE(g_iId);
				MESSAGE_END();
			}
		}

		if (WeaponInfoArray[g_iId].iForward[Fwd_Wpn_AddToPlayer])
		{
			MF_ExecuteForward
			(
				WeaponInfoArray[g_iId].iForward[Fwd_Wpn_AddToPlayer],

				static_cast<cell>(ENTINDEX(g_pWeapon)), 
				static_cast<cell>(ENTINDEX(g_pPlayer)), 
				static_cast<cell>((int)*((int *)g_pWeapon->pvPrivateData + m_iClip)), 
				static_cast<cell>(Player_AmmoInventory(g_pPlayer, g_pWeapon, TRUE)),
				static_cast<cell>(Player_AmmoInventory(g_pPlayer, g_pWeapon, FALSE))
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
	g_pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(g_pWeapon))
	{
		return 0;
	}

	g_iId = (int)*((int *)g_pWeapon->pvPrivateData + m_iId);

	if (WeaponInfoArray[g_iId].iType == Wpn_Custom)
	{
		return GetWeapon_Slot(g_iId) + 1;
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
	g_pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(g_pWeapon))
	{
		return 0;
	}

	g_iId = (int)*((int *)g_pWeapon->pvPrivateData + m_iId);

	if (WeaponInfoArray[g_iId].iType == Wpn_Default)
	{
#ifdef _WIN32
		return reinterpret_cast<void* (__fastcall *)(void *, int)>(g_CrowbarHooks[CrowbarHook_Respawn].address)(pPrivate, NULL);
#else
		return reinterpret_cast<void* (*)(void *)>(g_CrowbarHooks[CrowbarHook_Respawn].address)(pPrivate);
#endif
	}

	edict_t* pItem = Weapon_Spawn(g_iId, g_pWeapon->v.origin, g_pWeapon->v.angles);

	if (IsValidPev(pItem))
	{
		float flNextRespawn;

		if (cvar_mp_weaponstay->value && !(GetWeapon_Flags(g_iId) & ITEM_FLAG_LIMITINWORLD))
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

	if (!_strcmpi(STRING(pAmmobox->v.classname), "ammo_rpgclip"))
	{
		for (int k = 0; k < (int)g_BlockedItems.size(); k++)
		{
			if (!_strcmpi(g_BlockedItems[k]->strName.c_str(), "ammo_rpgclip"))
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

	for (int k = 0; k < g_iAmmoBoxIndex; k++)
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
	g_pWeapon = PrivateToEdict(pPrivate);

	if (IsValidPev(g_pWeapon))
	{
		g_pWeapon->v.flags |= FL_KILLME;
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

	if (ParseConfigSection("[block]", ParseBlockItems_Handler))
	{
		print_srvconsole("\n[WEAPONMOD] default items blocked:\n");

		for (int i = 0; i < (int)g_BlockedItems.size(); i++)
		{
			print_srvconsole("   %s\n", g_BlockedItems[i]->strName.c_str());
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
			if (!_strcmpi(g_BlockedItems[i]->strName.c_str(),szClassname))
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
	if (cvar_sv_cheats->value)
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
			if (WeaponInfoArray[k].iType == Wpn_Custom)
			{
				GiveNamedItem(pPlayer, GetWeapon_pszName(k));
			}
		}

		for (int k = 0; k < g_iAmmoBoxIndex; k++)
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
	g_pEntity = PrivateToEdict(pPrivate);

	if (!IsValidPev(g_pEntity))
	{
		return;
	}

	int iThinkForward = GetEntForward(g_pEntity, Think);

	if (iThinkForward)
	{
		if (!strstr(STRING(g_pEntity->v.classname), "weapon_"))
		{
			MF_ExecuteForward
			(
				iThinkForward,

				static_cast<cell>(ENTINDEX(g_pEntity)), 
				static_cast<cell>(0), 
				static_cast<cell>(0), 
				static_cast<cell>(0),
				static_cast<cell>(0)
			);
		}
		else
		{
			edict_t* pPlayer = GetPrivateCbase(g_pEntity, m_pPlayer);

			MF_ExecuteForward
			(
				iThinkForward,

				static_cast<cell>(ENTINDEX(g_pEntity)), 
				static_cast<cell>(ENTINDEX(pPlayer)), 
				static_cast<cell>((int)*((int *)g_pEntity->pvPrivateData + m_iClip)), 
				static_cast<cell>(Player_AmmoInventory(pPlayer, g_pEntity, TRUE)),
				static_cast<cell>(Player_AmmoInventory(pPlayer, g_pEntity, FALSE))
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
	static edict_t* pOther;

	g_pEntity = PrivateToEdict(pPrivate);
	pOther = PrivateToEdict(pPrivate2);

	if (!IsValidPev(g_pEntity))
	{
		return;
	}

	int iTouchForward = GetEntForward(g_pEntity, Touch);

	if (iTouchForward)
	{
		MF_ExecuteForward
		(
			iTouchForward,

			static_cast<cell>(ENTINDEX(g_pEntity)), 
			static_cast<cell>(ENTINDEX(pOther))
		);
	}
}

edict_t* Weapon_Spawn(int iId, Vector vecOrigin, Vector vecAngles)
{
	edict_t* pItem = NULL;

	static int iszAllocStringCached;

	if (iszAllocStringCached || (iszAllocStringCached = MAKE_STRING("weapon_crowbar")))
	{
		pItem = CREATE_NAMED_ENTITY(iszAllocStringCached);
	}

	if (IsValidPev(pItem))
	{
		MDLL_Spawn(pItem);
		SET_ORIGIN(pItem, vecOrigin);

		pItem->v.classname = MAKE_STRING(GetWeapon_pszName(iId));
		pItem->v.angles = vecAngles;

		*((int *)pItem->pvPrivateData + m_iId) = iId;

		if (GetWeapon_MaxClip(iId) != -1)
		{
			*((int *)pItem->pvPrivateData + m_iClip) = 0;
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
	}

	return pItem;
}

edict_t* Ammo_Spawn(int iId, Vector vecOrigin, Vector vecAngles)
{
	edict_t* pAmmoBox = NULL;

	static int iszAllocStringCached;

	if (iszAllocStringCached || (iszAllocStringCached = MAKE_STRING("ammo_rpgclip")))
	{
		pAmmoBox = CREATE_NAMED_ENTITY(iszAllocStringCached);
	}

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
	}

	return pAmmoBox;
}
