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

#include "wpnmod_parse.h"
#include "wpnmod_utils.h"
#include "wpnmod_hooks.h"

EntData* g_Ents = NULL;

VirtualHookData	g_RpgAddAmmo_Hook		= { "ammo_rpgclip",	VO_AddAmmo,		(void*)AmmoBox_AddAmmo,	NULL, NULL };
VirtualHookData g_PlayerSpawn_Hook		= { "player",		VO_Spawn,		(void*)Player_Spawn,	NULL, NULL };
VirtualHookData g_WorldPrecache_Hook	= { "worldspawn",	VO_Precache,	(void*)World_Precache,	NULL, NULL };

function g_dllFuncs[Func_End] =
{
	HOOK(NULL),
	HOOK(NULL),
	HOOK(NULL),
	HOOK(NULL),
	HOOK(NULL),
	HOOK(PrecacheOtherWeapon_HookHandler),
	HOOK(GiveNamedItem_HookHandler),
	HOOK(CheatImpulseCommands_HookHandler)
};

module g_GameDllModule = { NULL, NULL, NULL };

VirtualHookData g_CrowbarHooks[CrowbarHook_End] = 
{
	VHOOK(Respawn),
	VHOOK(AddToPlayer),
	VHOOK(GetItemInfo),
	VHOOK(CanDeploy),
	VHOOK(Deploy),
	VHOOK(CanHolster),
	VHOOK(Holster),
	VHOOK(ItemPostFrame),
	VHOOK(ItemSlot),
	VHOOK(IsUseable)
};


#ifdef _WIN32
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


#ifdef _WIN32
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


#ifdef _WIN32
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

	if (iInReload && flNextAttack <= gpGlobals->time)
	{
		// complete the reload. 
		int j = min(GetWeapon_MaxClip(iId) - iClip, iAmmoPrimary);	

		iClip += j;
		iAmmoPrimary -= j;

		// Add them to the clip
		SetPrivateInt(pWeapon, pvData_iClip, iClip);
		SetPrivateInt(pWeapon, pvData_fInReload, FALSE);

		SetAmmoInventory(pPlayer, PrimaryAmmoIndex(pWeapon), iAmmoPrimary);
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

		if (!IS_USEABLE(pWeapon) && flNextPrimaryAttack < 0.0) 
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


#ifdef _WIN32
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


#ifdef _WIN32
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
			static_cast<cell>(GetAmmoInventory(pPlayer, PrimaryAmmoIndex(pWeapon))),
			static_cast<cell>(GetAmmoInventory(pPlayer, SecondaryAmmoIndex(pWeapon)))
		);
	}

	g_engfuncs.pfnSetClientKeyValue(ENTINDEX(pPlayer), g_engfuncs.pfnGetInfoKeyBuffer(pPlayer), "cl_lw", "1");
}


#ifdef _WIN32
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
					static_cast<cell>(GetAmmoInventory(pPlayer, PrimaryAmmoIndex(pWeapon))),
					static_cast<cell>(GetAmmoInventory(pPlayer, SecondaryAmmoIndex(pWeapon)))
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
				static_cast<cell>(GetAmmoInventory(pPlayer, PrimaryAmmoIndex(pWeapon))),
				static_cast<cell>(GetAmmoInventory(pPlayer, SecondaryAmmoIndex(pWeapon)))
			);
		}
	}

	return ADD_TO_PLAYER(pvItem, pvPlayer);
}


#ifdef _WIN32
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
		return GetWeapon_Slot(iId) + 1;
	}

	return ITEM_SLOT(pvItem);
}


#ifdef _WIN32
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


#ifdef _WIN32
	int __fastcall Item_Block(void* pvItem, int DUMMY, void* pvOther)
#else
	int Item_Block(void* pvItem, void* pvOther)
#endif
{
	edict_t* pWeapon = PrivateToEdict(pvItem);

	if (IsValidPev(pWeapon))
	{
		pWeapon->v.flags |= FL_KILLME;
	}

	return FALSE;
}


#ifdef _WIN32
	void __fastcall World_Precache(void* pvEntity)
#else
	void World_Precache(void* pvEntity)
#endif
{
	SetConfigFile();

	if (ParseConfigSection(g_ConfigFilepath, "[block]", (void*)ParseBlockItems_Handler) && (int)g_BlockedItems.size())
	{
		printf("\n[WEAPONMOD] default items blocked:\n");

		for (int i = 0; i < (int)g_BlockedItems.size(); i++)
		{
			printf("   %s\n", g_BlockedItems[i]->classname);
		}

		printf("\n");
	}
	
	WORLD_PRECACHE(pvEntity);
}


#ifdef _WIN32
	void __fastcall Equipment_Think(void* pvEntity)
#else
	void Equipment_Think(void* pvEntity)
#endif
{
	edict_t* pEntity = PrivateToEdict(pvEntity);

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

	if (IsValidPev(g_EquipEnt))
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
			SetAmmoInventory(pPlayer, iAmmoIndex, g_StartAmmo[i]->count);
		}
	}
}


#ifdef _WIN32
	void __fastcall Global_Think(void* pvEntity)
#else
	void Global_Think(void* pvEntity)
#endif
{
	static edict_t* pEntity;
	
	pEntity = PrivateToEdict(pvEntity);

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
				static_cast<cell>(-1), 
				static_cast<cell>(-1), 
				static_cast<cell>(-1),
				static_cast<cell>(-1)
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
				static_cast<cell>(GetAmmoInventory(pPlayer, PrimaryAmmoIndex(pEntity))),
				static_cast<cell>(GetAmmoInventory(pPlayer, SecondaryAmmoIndex(pEntity)))
			);
		}
	}
}


#ifdef _WIN32
	void __fastcall Global_Touch(void* pvEntity, int DUMMY, void* pvOther)
#else
	void Global_Touch(void* pvEntity, void* pvOther)
#endif
{
	static edict_t* pEntity;
	static edict_t* pOther;

	pEntity = PrivateToEdict(pvEntity);
	pOther = PrivateToEdict(pvOther);

	if (!IsValidPev(pEntity) || !FNullEnt(pOther))
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


void PrecacheOtherWeapon_HookHandler(const char *szClassname)
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
	PRECACHE_OTHER_WEAPON(szClassname);
	SetHook(&g_dllFuncs[Func_PrecacheOtherWeapon]);
}


#ifdef _WIN32
	void __fastcall GiveNamedItem_HookHandler(void *pvPlayer, int DUMMY, const char *szName)
#else
	void GiveNamedItem_HookHandler(void *pvPlayer, const char *szName)
#endif
{
	if (szName)
	{
		GiveNamedItem(PrivateToEdict(pvPlayer), szName);
	}

	UnsetHook(&g_dllFuncs[Func_GiveNamedItem]);
	GIVE_NAMED_ITEM(pvPlayer, szName);
	SetHook(&g_dllFuncs[Func_GiveNamedItem]);
}


#ifdef _WIN32
	void __fastcall CheatImpulseCommands_HookHandler(void* pvPlayer, int DUMMY, int iImpulse)
#else
	void CheatImpulseCommands_HookHandler(void* pvPlayer, int iImpulse)
#endif
{
	// check cheat impulse command now
	if (iImpulse == 101 && cvar_sv_cheats->value)
	{
		edict_t *pPlayer = PrivateToEdict(pvPlayer);

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
	CHEAT_IMPULSE_COMMANDS(pvPlayer, iImpulse);
	SetHook(&g_dllFuncs[Func_CheatImpulseCommands]);
}
