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
#include "CVirtHook.h"


int g_iId;
int g_iWeaponIndex;
int g_iAmmoBoxIndex;

edict_t* g_pEntity;
edict_t* g_pWeapon;
edict_t* g_pPlayer;

BOOL g_InitWeapon;
BOOL g_CrowbarHooksEnabled;

CVirtHook g_VirtHook_Crowbar;

WeaponData WeaponInfoArray[MAX_WEAPONS];
AmmoBoxData AmmoBoxInfoArray[MAX_WEAPONS];

EntData *g_Ents = NULL;

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

		pItem->v.classname = MAKE_STRING(pszName(iId));
		pItem->v.angles = vecAngles;

		*((int *)pItem->pvPrivateData + m_iId) = iId;

		if (iMaxClip(iId) != -1)
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



#ifdef _WIN32
int __fastcall Weapon_GetItemInfo(void *pPrivate, int i, ItemInfo *p)
#elif __linux__
int Weapon_GetItemInfo(void *pPrivate, ItemInfo *p)
#endif
{
#ifdef _WIN32
	reinterpret_cast<int (__fastcall *)(void *, int, ItemInfo *)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_GetItemInfo))(pPrivate, 0, p);
#elif __linux__
	reinterpret_cast<int (*)(void *, ItemInfo *)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_GetItemInfo))(pPrivate, p);
#endif

	g_pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(g_pWeapon))
	{
		return 0;
	}

	g_iId = (int)*((int *)g_pWeapon->pvPrivateData + m_iId);

	if (g_InitWeapon)
	{
		g_InitWeapon = FALSE;

		g_iId = g_iWeaponIndex;
	}

	if (g_iId > LIMITER_WEAPON)
	{
		p->iId = g_iId;
		p->pszName = pszName(g_iId);
		p->iSlot = iSlot(g_iId);
		p->iPosition = iItemPosition(g_iId);
		p->iMaxAmmo1 = iMaxAmmo1(g_iId);
		p->iMaxAmmo2 = iMaxAmmo2(g_iId);
		p->iMaxClip = iMaxClip(g_iId);
		p->iFlags = iFlags(g_iId);
		p->iWeight = iWeight(g_iId);

		if (pszAmmo1(g_iId)[0])
		{
			p->pszAmmo1 = pszAmmo1(g_iId);
		}

		if (pszAmmo2(g_iId)[0])
		{
			p->pszAmmo2 = pszAmmo2(g_iId);
		}
	}

	return 1;
}



#ifdef _WIN32
BOOL __fastcall Weapon_CanDeploy(void *pPrivate)
#elif __linux__
BOOL Weapon_CanDeploy(void *pPrivate)
#endif
{
	g_pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(g_pWeapon))
	{
		return FALSE;
	}

	g_iId = (int)*((int *)g_pWeapon->pvPrivateData + m_iId);

	if (g_iId <= LIMITER_WEAPON || !WeaponInfoArray[g_iId].iForward[Fwd_Wpn_CanDeploy])
	{
#ifdef _WIN32
		return reinterpret_cast<int (__fastcall *)(void *, int)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_CanDeploy))(pPrivate, 0);
#elif __linux__
		return reinterpret_cast<int (*)(void *)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_CanDeploy))(pPrivate);
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
#elif __linux__
BOOL Weapon_Deploy(void *pPrivate)
#endif
{
	g_pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(g_pWeapon))
	{
		return FALSE;
	}

	g_iId = (int)*((int *)g_pWeapon->pvPrivateData + m_iId);

	if (g_iId <= LIMITER_WEAPON)
	{
#ifdef _WIN32
		return reinterpret_cast<int (__fastcall *)(void *, int)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_Deploy))(pPrivate, NULL);
#elif __linux__
		return reinterpret_cast<int (*)(void *)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_Deploy))(pPrivate);
#endif
	}

	g_pPlayer = GetPrivateCbase(g_pWeapon, m_pPlayer);

	if (!IsValidPev(g_pPlayer))
	{
		return FALSE;
	}

	int iReturn = FALSE;

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
#elif __linux__
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

	if (g_iId <= LIMITER_WEAPON)
	{
#ifdef _WIN32
		reinterpret_cast<int (__fastcall *)(void *, int)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_ItemPostFrame))(pPrivate, 0);
#elif __linux__
		reinterpret_cast<int (*)(void *)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_ItemPostFrame))(pPrivate);
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
		int j = min(iMaxClip(g_iId) - iClip, iAmmoPrimary);	

		iClip += j;
		iAmmoPrimary -= j;

		// Add them to the clip
		*((int *)g_pWeapon->pvPrivateData + m_iClip) = iClip;
		*((int *)g_pWeapon->pvPrivateData + m_fInReload) = FALSE;	

		Player_Set_AmmoInventory(g_pPlayer, g_pWeapon, TRUE, iAmmoPrimary);
	}

	if ((g_pPlayer->v.button & IN_ATTACK2) && flNextSecondaryAttack < 0.0)
	{
		if (pszAmmo2(g_iId) && !iAmmoSecondary)
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
		if ((!iClip && pszAmmo1(g_iId)) || (iMaxClip(g_iId) == -1 && !iAmmoPrimary ) )
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
	else if (g_pPlayer->v.button & IN_RELOAD && iMaxClip(g_iId) != -1 && !iInReload ) 
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
	else if (!(g_pPlayer->v.button & (IN_ATTACK|IN_ATTACK2)))
	{
		// no fire buttons down

		*((int *)g_pWeapon->pvPrivateData + m_fFireOnEmpty) = FALSE;

		if (iClip <= 0 && iAmmoPrimary <= 0 && iMaxAmmo1(g_iId) != -1 && flNextPrimaryAttack < 0.0) 
		{
			// weapon isn't useable, switch.
#ifdef _WIN32
			reinterpret_cast<int (__fastcall *)(void *, int)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_ItemPostFrame))(pPrivate, 0);
#elif __linux__
			reinterpret_cast<int (*)(void *)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_ItemPostFrame))(pPrivate);
#endif
			return;
		}
		else
		{
			// weapon is useable. Reload if empty and weapon has waited as long as it has to after firing
			if (!iClip && !(iFlags(g_iId) & ITEM_FLAG_NOAUTORELOAD) && flNextPrimaryAttack < 0.0)
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
#elif __linux__
BOOL Weapon_IsUseable(void *pPrivate)
#endif
{
	g_pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(g_pWeapon))
	{
		return FALSE;
	}

	g_iId = (int)*((int *)g_pWeapon->pvPrivateData + m_iId);

	if (g_iId <= LIMITER_WEAPON || !WeaponInfoArray[g_iId].iForward[Fwd_Wpn_IsUseable])
	{
#ifdef _WIN32
		return reinterpret_cast<int (__fastcall *)(void *, int)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_IsUseable))(pPrivate, 0);
#elif __linux__
		return reinterpret_cast<int (*)(void *)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_IsUseable))(pPrivate);
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
#elif __linux__
BOOL Weapon_CanHolster(void *pPrivate)
#endif
{
	g_pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(g_pWeapon))
	{
		return FALSE;
	}

	g_iId = (int)*((int *)g_pWeapon->pvPrivateData + m_iId);

	if (g_iId <= LIMITER_WEAPON || !WeaponInfoArray[g_iId].iForward[Fwd_Wpn_CanHolster])
	{
#ifdef _WIN32
		return reinterpret_cast<int (__fastcall *)(void *, int)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_CanHolster))(pPrivate, 0);
#elif __linux__
		return reinterpret_cast<int (*)(void *)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_CanHolster))(pPrivate);
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
#elif __linux__
void Weapon_Holster(void *pPrivate, int skiplocal)
#endif
{
	g_pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(g_pWeapon))
	{
		return;
	}

	g_iId = (int)*((int *)g_pWeapon->pvPrivateData + m_iId);

	if (g_iId <= LIMITER_WEAPON)
	{
#ifdef _WIN32
		reinterpret_cast<int (__fastcall *)(void *, int, int)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_Holster))(pPrivate, 0, skiplocal);
#elif __linux__
		reinterpret_cast<int (*)(void *, int)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_Holster))(pPrivate, skiplocal);
#endif
		return;
	}

	Set_Think(ENTINDEX(g_pWeapon), 0);
	Set_Touch(ENTINDEX(g_pWeapon), 0);

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
}


#ifdef _WIN32
int __fastcall Weapon_AddToPlayer(void *pPrivate, int i, void *pPrivate2)
#elif __linux__
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

	if (g_iId > LIMITER_WEAPON && IsValidPev(g_pPlayer))
	{
		if (!CVAR_GET_POINTER("aghl.ru"))
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
	return reinterpret_cast<int (__fastcall *)(void *, int, void *)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_AddToPlayer))(pPrivate, 0, pPrivate2);
#elif __linux__
	return reinterpret_cast<int (*)(void *, void *)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_AddToPlayer))(pPrivate, pPrivate2);
#endif
}



#ifdef _WIN32
int __fastcall Weapon_ItemSlot(void *pPrivate)
#elif __linux__
int Weapon_ItemSlot(void *pPrivate)
#endif
{
	g_pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(g_pWeapon))
	{
		return 0;
	}

	g_iId = (int)*((int *)g_pWeapon->pvPrivateData + m_iId);

	if (g_iId > LIMITER_WEAPON)
	{
		return iSlot(g_iId) + 1;
	}

#ifdef _WIN32
	return reinterpret_cast<int (__fastcall *)(void *, int)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_ItemSlot))(pPrivate, 0);
#elif __linux__
	return reinterpret_cast<int (*)(void *)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_ItemSlot))(pPrivate);
#endif
}



#ifdef _WIN32
void __fastcall Weapon_Drop(void *pPrivate)
#elif __linux__
void Weapon_Drop(void *pPrivate)
#endif
{
	g_pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(g_pWeapon))
	{
		return;
	}

	g_iId = (int)*((int *)g_pWeapon->pvPrivateData + m_iId);

	if (g_iId > LIMITER_WEAPON)
	{
		REMOVE_ENTITY(g_pWeapon);
		return;
	}
#ifdef _WIN32
	reinterpret_cast<int (__fastcall *)(void *, int)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_Drop))(pPrivate, 0);
#elif __linux__
	reinterpret_cast<int (*)(void *)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_Drop))(pPrivate);
#endif
}



#ifdef _WIN32
void* __fastcall Weapon_Respawn(void *pPrivate)
#elif __linux__
void* Weapon_Respawn(void *pPrivate)
#endif
{
	g_pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(g_pWeapon))
	{
		return 0;
	}

	g_iId = (int)*((int *)g_pWeapon->pvPrivateData + m_iId);

	if (g_iId <= LIMITER_WEAPON)
	{
#ifdef _WIN32
		return reinterpret_cast<void* (__fastcall *)(void *, int)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_Respawn))(pPrivate, NULL);
#elif __linux__
		return reinterpret_cast<void* (*)(void *)>(g_VirtHook_Crowbar.GetOrigFunc(VirtFunc_Respawn))(pPrivate);
#endif
	}

	edict_t* pItem = Weapon_Spawn(g_iId, g_pWeapon->v.origin, g_pWeapon->v.angles);

	if (IsValidPev(pItem))
	{
		float flNextRespawn;

		if (CVAR_GET_FLOAT("mp_weaponstay") && !(iFlags(g_iId) & ITEM_FLAG_LIMITINWORLD))
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



void Ammo_Materialize(edict_t *pEntity)
{
	if (!IsValidPev(pEntity))
	{
		return;
	}

	EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "items/suitchargeok1.wav", 1.0, ATTN_NORM, 0, 150);

	pEntity->v.effects &= ~EF_NODRAW;
	pEntity->v.effects |= EF_MUZZLEFLASH;
	pEntity->v.solid = SOLID_TRIGGER;
}



void Ammo_Respawn(edict_t *pAmmoBox)
{
	if (pAmmoBox->v.spawnflags & SF_NORESPAWN)
	{
		return;
	}

	*((int *)pAmmoBox->pvPrivateData + m_pfnThink) = (int)(Ammo_Materialize);

	pAmmoBox->v.nextthink = gpGlobals->time + AMMO_RESPAWN_TIME;
	pAmmoBox->v.effects |= EF_NODRAW;
	pAmmoBox->v.solid = SOLID_NOT;
}



#ifdef _WIN32
void __fastcall Ammo_Touch(CBaseEntity *pEntity, int i, CBaseEntity *pEntity2)
#elif __linux__
void Ammo_Touch(CBaseEntity *pEntity, CBaseEntity *pEntity2)
#endif
{
	static int k;
	static edict_t* pOther;

	g_pEntity = pEntity->pev->pContainingEntity;
	pOther = pEntity2->pev->pContainingEntity;

	if (!IsValidPev(g_pEntity))
	{
		return;
	}

	if (MF_IsPlayerAlive(ENTINDEX(pOther)))
	{
		for (k = 0; k < g_iAmmoBoxIndex; k++)
		{
			if (!strcmp(STRING(g_pEntity->v.classname), AmmoBoxInfoArray[k].pszName) && AmmoBoxInfoArray[k].iForward[Fwd_Ammo_AddAmmo])
			{
				if 
				(
					MF_ExecuteForward
					(
						AmmoBoxInfoArray[k].iForward[Fwd_Ammo_AddAmmo], 
						static_cast<cell>(ENTINDEX(g_pEntity)),
						static_cast<cell>(ENTINDEX(pOther))
					)
				)
				{
					(g_pEntity->v.spawnflags & SF_NORESPAWN) ? REMOVE_ENTITY(g_pEntity) : Ammo_Respawn(g_pEntity);
					return;
				}
			}
		}
	}
}



edict_t* Ammo_Spawn(int iId, Vector vecOrigin, Vector vecAngles)
{
	edict_t* pAmmoBox = NULL;

	static int iszAllocStringCached;

	if (iszAllocStringCached || (iszAllocStringCached = MAKE_STRING("info_target")))
	{
		pAmmoBox = CREATE_NAMED_ENTITY(iszAllocStringCached);
	}

	if (IsValidPev(pAmmoBox))
	{
		pAmmoBox->v.classname = MAKE_STRING(AmmoBoxInfoArray[iId].pszName);

		if (AmmoBoxInfoArray[iId].iForward[Fwd_Ammo_Spawn])
		{
			MF_ExecuteForward
			(
				AmmoBoxInfoArray[iId].iForward[Fwd_Ammo_Spawn],
				static_cast<cell>(ENTINDEX(pAmmoBox)),
				static_cast<cell>(0)
			);
		}

		pAmmoBox->v.movetype = MOVETYPE_TOSS;
		pAmmoBox->v.solid = SOLID_TRIGGER;

		SET_SIZE(pAmmoBox, Vector(-16, -16, 0), Vector(16, 16, 16));
		SET_ORIGIN(pAmmoBox, vecOrigin);

		pAmmoBox->v.angles = vecAngles;

		Set_Touch(ENTINDEX(pAmmoBox), 0);
		*((int *)pAmmoBox->pvPrivateData + m_pfnTouch) = (int)(Ammo_Touch);
	}

	return pAmmoBox;
}



//void Global_Think(edict_t *pEntity)
#ifdef _WIN32
void __fastcall Global_Think(CBaseEntity *pEntity)
#elif __linux__
void Global_Think(CBaseEntity *pEntity)
#endif
{
	g_pEntity = pEntity->pev->pContainingEntity;

	if (!IsValidPev(g_pEntity))
	{
		return;
	}

	int iThinkForward = Get_Think(ENTINDEX(g_pEntity));

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
void __fastcall Global_Touch(CBaseEntity *pEntity, int i, CBaseEntity *pEntity2)
#elif __linux__
void Global_Touch(CBaseEntity *pEntity, CBaseEntity *pEntity2)
#endif
{
	static edict_t* pOther;

	g_pEntity = pEntity->pev->pContainingEntity;
	pOther = pEntity2->pev->pContainingEntity;

	if (!IsValidPev(g_pEntity))
	{
		return;
	}

	int iTouchForward = Get_Touch(ENTINDEX(g_pEntity));

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



void ActivateCrowbarHooks()
{
	edict_t *pEdict = CREATE_ENTITY();

	CALL_GAME_ENTITY(PLID, "weapon_crowbar", &pEdict->v);

	if (pEdict->pvPrivateData == NULL)
	{
		REMOVE_ENTITY(pEdict);
		return;
	}

	g_VirtHook_Crowbar.SetHook(pEdict, VirtFunc_GetItemInfo,	(int)Weapon_GetItemInfo);
	g_VirtHook_Crowbar.SetHook(pEdict, VirtFunc_CanDeploy,		(int)Weapon_CanDeploy);
	g_VirtHook_Crowbar.SetHook(pEdict, VirtFunc_Deploy,			(int)Weapon_Deploy);
	g_VirtHook_Crowbar.SetHook(pEdict, VirtFunc_CanHolster,		(int)Weapon_CanHolster);
	g_VirtHook_Crowbar.SetHook(pEdict, VirtFunc_Holster,		(int)Weapon_Holster);
	g_VirtHook_Crowbar.SetHook(pEdict, VirtFunc_Respawn,		(int)Weapon_Respawn);
	g_VirtHook_Crowbar.SetHook(pEdict, VirtFunc_AddToPlayer,	(int)Weapon_AddToPlayer);
	g_VirtHook_Crowbar.SetHook(pEdict, VirtFunc_ItemPostFrame,	(int)Weapon_ItemPostFrame);
	g_VirtHook_Crowbar.SetHook(pEdict, VirtFunc_Drop,			(int)Weapon_Drop);
	g_VirtHook_Crowbar.SetHook(pEdict, VirtFunc_ItemSlot,		(int)Weapon_ItemSlot);
	g_VirtHook_Crowbar.SetHook(pEdict, VirtFunc_IsUseable,		(int)Weapon_IsUseable);

	REMOVE_ENTITY(pEdict);
}
