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


int g_iWeaponIndex;
int VirtualFunction[VirtFunc_End];

CEntity g_EntData;
HookData CrowbarHook[VirtFunc_End];
WeaponData WeaponInfoArray[MAX_WEAPONS];
AmmoBoxData AmmoBoxInfoArray[MAX_WEAPONS];

BOOL g_HooksEnabled;
BOOL g_InitWeapon;
BOOL g_initialized;

void *pDbase = NULL;

//**********************************************
//* Weapon spawn.                              *
//**********************************************

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

		if (WeaponInfoArray[iId].iForward[Fwd_Spawn])
		{
			MF_ExecuteForward
			(
				WeaponInfoArray[iId].iForward[Fwd_Spawn],

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

	if (iszAllocStringCached || (iszAllocStringCached = MAKE_STRING("info_target")))
	{
		pAmmoBox = CREATE_NAMED_ENTITY(iszAllocStringCached);
	}

	if (IsValidPev(pAmmoBox))
	{
		MDLL_Spawn(pAmmoBox);

		pAmmoBox->v.classname = MAKE_STRING(AmmoBoxInfoArray[iId].pszName);
		pAmmoBox->v.movetype = MOVETYPE_TOSS;
		pAmmoBox->v.solid = SOLID_TRIGGER;

		SET_SIZE(pAmmoBox, Vector(-16.0, -16.0, 0.0), Vector(16.0, 16.0, 16.0));
		SET_ORIGIN(pAmmoBox, vecOrigin);

		pAmmoBox->v.angles = vecAngles;

		/*if (AmmoBoxInfoArray[iId].iForward[Fwd_Spawn])
		{
			MF_ExecuteForward
			(
				WeaponInfoArray[iId].iForward[Fwd_Spawn],

				static_cast<cell>(ENTINDEX(pItem)), 
				static_cast<cell>(0), 
				static_cast<cell>(0), 
				static_cast<cell>(0),
				static_cast<cell>(0)
			);
		}*/
	}

	return pAmmoBox;
}

int g_iId;

edict_t* g_pWeapon;
edict_t* g_pPlayer;


//**********************************************
//* Get Item Info.                             *
//**********************************************

#ifdef _WIN32
	int __fastcall Weapon_GetItemInfo(void *pPrivate, int i, ItemInfo *p)
#elif __linux__
	int Weapon_GetItemInfo(void *pPrivate, ItemInfo *p)
#endif
{
#ifdef _WIN32
	reinterpret_cast<int (__fastcall *)(void *, int, ItemInfo *)>(CrowbarHook[VirtFunc_GetItemInfo].pOrigFunc)(pPrivate, 0, p);
#elif __linux__
	reinterpret_cast<int (*)(void *, ItemInfo *)>(pOrigFunc_Crowbar_GetItemInfo)(pPrivate, p);
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

//**********************************************
//* Deploys the weapon.                        *
//**********************************************

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
		reinterpret_cast<int (__fastcall *)(void *, int)>(CrowbarHook[VirtFunc_Deploy].pOrigFunc)(pPrivate, 0);
#elif __linux__
		reinterpret_cast<int (*)(void *)>(CrowbarHook[VirtFunc_Deploy].pOrigFunc)(pPrivate);
#endif
		return TRUE;
	}
	
	g_pPlayer = GetPrivateCbase(g_pWeapon, m_pPlayer);

	if (!IsValidPev(g_pPlayer))
	{
		return FALSE;
	}

	int iReturn = FALSE;

	if (WeaponInfoArray[g_iId].iForward[Fwd_Deploy])
	{
		iReturn = MF_ExecuteForward
		(
			WeaponInfoArray[g_iId].iForward[Fwd_Deploy],

			static_cast<cell>(ENTINDEX(g_pWeapon)), 
			static_cast<cell>(ENTINDEX(g_pPlayer)), 
			static_cast<cell>((int)*((int *)g_pWeapon->pvPrivateData + m_iClip)), 
			static_cast<cell>(Player_AmmoInventory(g_pPlayer, g_pWeapon, TRUE)),
			static_cast<cell>(Player_AmmoInventory(g_pPlayer, g_pWeapon, FALSE))
		);
	}

	return iReturn;
}

//**********************************************
//* Called each frame for an item.             *
//**********************************************

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
		reinterpret_cast<int (__fastcall *)(void *, int)>(CrowbarHook[VirtFunc_ItemPostFrame].pOrigFunc)(pPrivate, 0);
#elif __linux__
		reinterpret_cast<int (*)(void *)>(CrowbarHook[VirtFunc_ItemPostFrame].pOrigFunc)(pPrivate);
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

		if (WeaponInfoArray[g_iId].iForward[Fwd_SecondaryAttack])
		{
			MF_ExecuteForward
			(
				WeaponInfoArray[g_iId].iForward[Fwd_SecondaryAttack],

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

		if (WeaponInfoArray[g_iId].iForward[Fwd_PrimaryAttack])
		{
			MF_ExecuteForward
			(
				WeaponInfoArray[g_iId].iForward[Fwd_PrimaryAttack],

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
		if (WeaponInfoArray[g_iId].iForward[Fwd_Reload])
		{
			MF_ExecuteForward
			(
				WeaponInfoArray[g_iId].iForward[Fwd_Reload],

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
			reinterpret_cast<int (__fastcall *)(void *, int)>(CrowbarHook[VirtFunc_ItemPostFrame].pOrigFunc)(pPrivate, 0);
#elif __linux__
			reinterpret_cast<int (*)(void *)>(CrowbarHook[VirtFunc_ItemPostFrame].pOrigFunc)(pPrivate);
#endif
			return;
		}
		else
		{
			// weapon is useable. Reload if empty and weapon has waited as long as it has to after firing
			if (!iClip && !(iFlags(g_iId) & ITEM_FLAG_NOAUTORELOAD) && flNextPrimaryAttack < 0.0)
			{
				if (WeaponInfoArray[g_iId].iForward[Fwd_Reload])
				{
					MF_ExecuteForward
					(
						WeaponInfoArray[g_iId].iForward[Fwd_Reload],

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

		if (WeaponInfoArray[g_iId].iForward[Fwd_Idle])
		{
			MF_ExecuteForward
			(
				WeaponInfoArray[g_iId].iForward[Fwd_Idle],

				static_cast<cell>(ENTINDEX(g_pWeapon)), 
				static_cast<cell>(ENTINDEX(g_pPlayer)), 
				static_cast<cell>(iClip), 
				static_cast<cell>(iAmmoPrimary),
				static_cast<cell>(iAmmoSecondary)
			);
		}
	}
}

//**********************************************
//* Called when the weapon is holster.         *
//**********************************************

#ifdef _WIN32
void __fastcall Weapon_Holster(void *pPrivate, int i, int skiplocal)
#elif __linux__
void Weapon_Holster(void *pPrivate)
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
		reinterpret_cast<int (__fastcall *)(void *, int, int)>(CrowbarHook[VirtFunc_Holster].pOrigFunc)(pPrivate, 0, skiplocal);
#elif __linux__
		reinterpret_cast<int (*)(void *, int)>(CrowbarHook[VirtFunc_Holster].pOrigFunc)(pPrivate, skiplocal);
#endif
		return;
	}

	g_EntData.Set_Think(ENTINDEX(g_pWeapon), 0);
	g_pPlayer = GetPrivateCbase(g_pWeapon, m_pPlayer);

	if (IsValidPev(g_pPlayer) && WeaponInfoArray[g_iId].iForward[Fwd_Holster])
	{
		MF_ExecuteForward
		(
			WeaponInfoArray[g_iId].iForward[Fwd_Holster],

			static_cast<cell>(ENTINDEX(g_pWeapon)), 
			static_cast<cell>(ENTINDEX(g_pPlayer)), 
			static_cast<cell>((int)*((int *)g_pWeapon->pvPrivateData + m_iClip)), 
			static_cast<cell>(Player_AmmoInventory(g_pPlayer, g_pWeapon, TRUE)),
			static_cast<cell>(Player_AmmoInventory(g_pPlayer, g_pWeapon, FALSE))
		);
	}
}

//**********************************************
//* Weapon think                               *
//**********************************************

#ifdef _WIN32
void __fastcall Weapon_Think(void *pPrivate, int i)
#elif __linux__
void Weapon_Think(void *pPrivate)
#endif
{
	g_pWeapon = PrivateToEdict(pPrivate);

	if (!IsValidPev(g_pWeapon))
	{
		return;
	}

	g_iId = (int)*((int *)g_pWeapon->pvPrivateData + m_iId);
	g_pPlayer = GetPrivateCbase(g_pWeapon, m_pPlayer);

	if (g_iId > LIMITER_WEAPON && IsValidPev(g_pPlayer))
	{
		int iThinkForward = g_EntData.Get_Think(ENTINDEX(g_pWeapon));

		if (iThinkForward)
		{
			MF_ExecuteForward
			(
				iThinkForward,

				static_cast<cell>(ENTINDEX(g_pWeapon)), 
				static_cast<cell>(ENTINDEX(g_pPlayer)), 
				static_cast<cell>((int)*((int *)g_pWeapon->pvPrivateData + m_iClip)), 
				static_cast<cell>(Player_AmmoInventory(g_pPlayer, g_pWeapon, TRUE)),
				static_cast<cell>(Player_AmmoInventory(g_pPlayer, g_pWeapon, FALSE))
			);
		}

		return;
	}
	
#ifdef _WIN32
	reinterpret_cast<int (__fastcall *)(void *, int)>(CrowbarHook[VirtFunc_Think].pOrigFunc)(pPrivate, 0);
#elif __linux__
	reinterpret_cast<int (*)(void *)>(CrowbarHook[VirtFunc_Think].pOrigFunc)(pPrivate);
#endif
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
		int msgWeapPickup = NULL;

		if (msgWeapPickup || (msgWeapPickup = REG_USER_MSG( "WeapPickup", 1 )))		
		{
			MESSAGE_BEGIN(MSG_ONE, msgWeapPickup, NULL, g_pPlayer);
				WRITE_BYTE(g_iId);
			MESSAGE_END();
		}
	}

#ifdef _WIN32
	int iOrigRet = reinterpret_cast<int (__fastcall *)(void *, int, void *)>(CrowbarHook[VirtFunc_AddToPlayer].pOrigFunc)(pPrivate, 0, pPrivate2);
#elif __linux__
	int iOrigRet = reinterpret_cast<int (*)(void *, void *)>(CrowbarHook[VirtFunc_AddToPlayer].pOrigFunc)(pPrivate, pPrivate2);
#endif

	return iOrigRet;
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
	int iOrigRet = reinterpret_cast<int (__fastcall *)(void *, int)>(CrowbarHook[VirtFunc_ItemSlot].pOrigFunc)(pPrivate, 0);
#elif __linux__
	int iOrigRet = reinterpret_cast<int (*)(void *)>(CrowbarHook[VirtFunc_ItemSlot].pOrigFunc)(pPrivate);
#endif

	return iOrigRet;
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
	reinterpret_cast<int (__fastcall *)(void *, int)>(CrowbarHook[VirtFunc_Drop].pOrigFunc)(pPrivate, 0);
#elif __linux__
	reinterpret_cast<int (*)(void *)>(CrowbarHook[VirtFunc_Drop].pOrigFunc)(pPrivate);
#endif
}

//**********************************************
//* Virtual functions hook                     *
//**********************************************

void MakeVirtualHooks(void)
{
	char *classname = "weapon_crowbar";

	CrowbarHook[VirtFunc_GetItemInfo].iTrampoline =		(int)Weapon_GetItemInfo;
	CrowbarHook[VirtFunc_Deploy].iTrampoline =			(int)Weapon_Deploy;
	CrowbarHook[VirtFunc_Holster].iTrampoline =			(int)Weapon_Holster;
	CrowbarHook[VirtFunc_Think].iTrampoline =			(int)Weapon_Think;
	CrowbarHook[VirtFunc_AddToPlayer].iTrampoline =		(int)Weapon_AddToPlayer;
	CrowbarHook[VirtFunc_ItemPostFrame].iTrampoline =	(int)Weapon_ItemPostFrame;
	CrowbarHook[VirtFunc_Drop].iTrampoline =			(int)Weapon_Drop;
	CrowbarHook[VirtFunc_ItemSlot].iTrampoline =		(int)Weapon_ItemSlot;

	edict_t *pEdict = CREATE_ENTITY();

    CALL_GAME_ENTITY(PLID, classname, &pEdict->v);
    
    if (pEdict->pvPrivateData == NULL)
    {
        REMOVE_ENTITY(pEdict);

		print_srvconsole("[WEAPONMOD] Failed to retrieve classtype for \"%s\".\n", classname);
       
		return;
    }

#ifdef _WIN32
	DWORD OldFlags;

    void **vtable = *((void***)((char*)pEdict->pvPrivateData));
#elif __linux__
    void **vtable = *((void***)(((char*)pEdict->pvPrivateData) + 0x60));
#endif

    REMOVE_ENTITY(pEdict);
    
    if (vtable == NULL)
	{
		print_srvconsole("[WEAPONMOD] Failed to retrieve vtable for \"%s\", hooks not active.\n", classname);

        return;
	}

	int **ivtable = (int **)vtable;

	for (int i = 0; i < VirtFunc_End; i++)
	{
		if (CrowbarHook[i].iTrampoline != NULL)
		{
			CrowbarHook[i].pOrigFunc = (void *)ivtable[VirtualFunction[i]];
#ifdef _WIN32
			VirtualProtect(&ivtable[VirtualFunction[i]], sizeof(int *), PAGE_READWRITE, &OldFlags);
#elif __linux__
			mprotect(&ivtable[CrowbarHook[i].iVirtFunc], sizeof(int*), PROT_READ | PROT_WRITE);
#endif
			ivtable[VirtualFunction[i]] = (int *)CrowbarHook[i].iTrampoline;
		}
	}
}

// *****************************************************
// class CEntity
// *****************************************************

int CEntity::Get_Think(int iEnt)
{
	int found = 0;

	Obj* a = head;

	while ( a )
	{
		if (a->iEntity == iEnt)
		{
			found = a->iThinkForward;
			break;
		}
		a = a->next;
	}
	return found;
}

void CEntity::Set_Think(int iEnt, int iForward )
{
	Obj* a = head;

	while ( a )
	{
		if (a->iEntity == iEnt)
		{
			a->iThinkForward = iForward;
			return;
		}

		a = a->next;
	}

	a = new Obj;

	if ( a == 0 ) 
		return;
	
	a->iThinkForward = iForward;
	a->iEntity = iEnt;
	a->next = head;
	
	head = a;
}

void CEntity::clear()
{
	while(head)
	{
		Obj* a = head->next;
		delete head;
		head = a;
	}
}