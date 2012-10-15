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
#include "effects.h"
#include "hooks.h"
#include "utils.h"


#define CHECK_OFFSET(x) \
	if ( x < 0 || x >= Offset_End) \
	{ \
		MF_LogError(amx, AMX_ERR_NATIVE, "Function out of bounds. Got: %d  Max: %d.", x, Offset_End - 1); \
		return 0; \
	}\
/*
#define CHECK_PARAMS(x) \
	cell count = params[0] / sizeof(cell); \
	if (count != x) \
	{ \
		MF_LogError(amx, AMX_ERR_NATIVE, "Expected %d parameters, got %d.", x, count); \
		return 0; \
	}\
*/

enum e_Offsets
{
	// Weapon
	Offset_flStartThrow,
	Offset_flReleaseThrow,
	Offset_iChargeReady,
	Offset_iInAttack,
	Offset_iFireState,
	Offset_iFireOnEmpty,				// true when the gun is empty and the player is still holding down the attack key(s)
	Offset_flPumpTime,
	Offset_iInSpecialReload,			// Are we in the middle of a reload for the shotguns
	Offset_flNextPrimaryAttack,			// soonest time ItemPostFrame will call PrimaryAttack
	Offset_flNextSecondaryAttack,		// soonest time ItemPostFrame will call SecondaryAttack
	Offset_flTimeWeaponIdle,			// soonest time ItemPostFrame will call WeaponIdle
	Offset_iPrimaryAmmoType,			// "primary" ammo index into players m_rgAmmo[]
	Offset_iSecondaryAmmoType,			// "secondary" ammo index into players m_rgAmmo[]
	Offset_iClip,						// number of shots left in the primary weapon clip, -1 it not used
	Offset_iInReload,					// are we in the middle of a reload;
	Offset_iDefaultAmmo,				// how much ammo you get when you pick up this weapon as placed by a level designer.
	
	// Player
	Offset_flNextAttack,				// cannot attack again until this time
	Offset_iWeaponVolume,				// how loud the player's weapon is right now
	Offset_iWeaponFlash,				// brightness of the weapon flash
	Offset_iLastHitGroup,
	Offset_iFOV,

	// Custom (for weapon and "info_target" entities only)
	Offset_iuser1,
	Offset_iuser2,
	Offset_iuser3,
	Offset_iuser4,
	Offset_fuser1,
	Offset_fuser2,
	Offset_fuser3,
	Offset_fuser4,
	
	Offset_End
};

int PvDataOffsets[Offset_End] =
{
	m_flStartThrow,
	m_flReleaseThrow,
	m_chargeReady,
	m_fInAttack,
	m_fireState,
	m_fFireOnEmpty,
	m_flPumpTime,
	m_fInSpecialReload,
	m_flNextPrimaryAttack,
	m_flNextSecondaryAttack,
	m_flTimeWeaponIdle,
	m_iPrimaryAmmoType,
	m_iSecondaryAmmoType,
	m_iClip,
	m_fInReload,
	m_iDefaultAmmo,
	m_flNextAttack,
	m_iWeaponVolume,
	m_iWeaponFlash,
	m_LastHitGroup,
	m_iFOV,
	XTRA_OFS_WEAPON + 8,
	XTRA_OFS_WEAPON + 9,
	XTRA_OFS_WEAPON + 10,
	XTRA_OFS_WEAPON + 11,
	XTRA_OFS_WEAPON + 12,
	XTRA_OFS_WEAPON + 13,
	XTRA_OFS_WEAPON + 14,
	XTRA_OFS_WEAPON + 15,
};



int g_iWeaponsCount = 0;
int g_iWeaponInitID = 0;
int g_iAmmoBoxIndex = 0;

int g_iCurrentSlots[MAX_WEAPON_SLOTS][MAX_WEAPON_POSITIONS];

BOOL g_CrowbarHooksEnabled;
BOOL g_AmmoBoxHooksEnabled;

WeaponData WeaponInfoArray[MAX_WEAPONS];
AmmoBoxData AmmoBoxInfoArray[MAX_WEAPONS];



void AutoSlotDetection(int iWeaponID, int iSlot, int iPosition)
{
	if (iSlot >= MAX_WEAPON_SLOTS || iSlot < 0)
	{
		iSlot = MAX_WEAPON_SLOTS - 1;
	}

	if (iPosition >= MAX_WEAPON_POSITIONS || iPosition < 0)
	{
		iPosition = MAX_WEAPON_POSITIONS - 1;
	}

	if (!g_iCurrentSlots[iSlot][iPosition])
	{
		g_iCurrentSlots[iSlot][iPosition] = iWeaponID;

		WeaponInfoArray[iWeaponID].ItemData.iSlot = iSlot;
		WeaponInfoArray[iWeaponID].ItemData.iPosition = iPosition;
	}
	else
	{
		BOOL bFound = FALSE;

		for (int k, i = 0; i < MAX_WEAPON_SLOTS && !bFound; i++)
		{
			for (k = 0; k < MAX_WEAPON_POSITIONS; k++)
			{
				if (!g_iCurrentSlots[i][k])
				{
					g_iCurrentSlots[i][k] = iWeaponID;

					WeaponInfoArray[iWeaponID].ItemData.iSlot = i;
					WeaponInfoArray[iWeaponID].ItemData.iPosition = k;

					print_srvconsole("[WEAPONMOD] \"%s\" is moved to slot %d-%d.\n", GetWeapon_pszName(iWeaponID), i + 1, k + 1);

					bFound = TRUE;
					break;
				}
			}
		}
		
		if (!bFound)
		{
			WeaponInfoArray[iWeaponID].ItemData.iPosition = MAX_WEAPONS;
			print_srvconsole("[WEAPONMOD] No free slot for \"%s\" in HUD!\n", GetWeapon_pszName(iWeaponID));
		}
	}
}

/**
 * Register new weapon in module.
 *
 * @param szName		The weapon name.
 * @param iSlot			SlotID (1...5).
 * @param iPosition		NumberInSlot (1...5).
 * @param szAmmo1		Primary ammo type ("9mm", "uranium", "MY_AMMO" etc).
 * @param iMaxAmmo1		Max amount of primary ammo.
 * @param szAmmo2		Secondary ammo type.
 * @param iMaxAmmo2		Max amount of secondary ammo.
 * @param iMaxClip		Max amount of ammo in weapon's clip.
 * @param iFlags		Weapon's flags (see defines).
 * @param iWeight		This value used to determine this weapon's importance in autoselection.
 * 
 * @return				The ID of registerd weapon or 0 on failure. (integer)
 *
 * native wpnmod_register_weapon(const szName[], const iSlot, const iPosition, const szAmmo1[], const iMaxAmmo1, const szAmmo2[], const iMaxAmmo2, const iMaxClip, const iFlags, const iWeight);
*/
static cell AMX_NATIVE_CALL wpnmod_register_weapon(AMX *amx, cell *params)
{
	#define UD_FINDPLUGIN 3

	const char *szWeaponName = MF_GetAmxString(amx, params[1], 0, NULL);

	for (int i = 1; i < MAX_WEAPONS; i++)
	{
		if (WeaponInfoArray[i].iType != Wpn_None && !_strcmpi(GetWeapon_pszName(i), szWeaponName))
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Weapon name is duplicated.");
			return -1;
		}

		if (WeaponInfoArray[i].iType == Wpn_None)
		{
			g_iWeaponsCount++;

			WeaponInfoArray[i].iType = Wpn_Custom;

			WeaponInfoArray[i].ItemData.pszName = STRING(ALLOC_STRING(szWeaponName));
			WeaponInfoArray[i].ItemData.pszAmmo1 = STRING(ALLOC_STRING(MF_GetAmxString(amx, params[4], 0, NULL)));
			WeaponInfoArray[i].ItemData.iMaxAmmo1 = params[5];
			WeaponInfoArray[i].ItemData.pszAmmo2 = STRING(ALLOC_STRING(MF_GetAmxString(amx, params[6], 0, NULL)));
			WeaponInfoArray[i].ItemData.iMaxAmmo2 = params[7];
			WeaponInfoArray[i].ItemData.iMaxClip = params[8];
			WeaponInfoArray[i].ItemData.iFlags = params[9];
			WeaponInfoArray[i].ItemData.iWeight = params[10];

			CPlugin* plugin = (CPlugin*)amx->userdata[UD_FINDPLUGIN];

			WeaponInfoArray[i].title = plugin->title;
			WeaponInfoArray[i].author = plugin->author;
			WeaponInfoArray[i].version = plugin->version;

			AutoSlotDetection(i, params[2] - 1, params[3] - 1);

			if (!g_CrowbarHooksEnabled)
			{
				g_CrowbarHooksEnabled = TRUE;
		
				for (int k = 0; k < CrowbarHook_End; k++)
				{
					SetHookVirt(&g_CrowbarHooks[k]);
				}
			}

			g_iWeaponInitID = i;
			
			UnsetHook(&g_dllFuncs[Func_PrecacheOtherWeapon]);
		#ifdef _WIN32
			reinterpret_cast<int (__cdecl *)(const char *)>(g_dllFuncs[Func_PrecacheOtherWeapon].address)("weapon_crowbar");
		#else
			reinterpret_cast<int (*)(const char *)>(g_dllFuncs[Func_PrecacheOtherWeapon].address)("weapon_crowbar");
		#endif
			SetHook(&g_dllFuncs[Func_PrecacheOtherWeapon]);

			return i;
		}
	}

	MF_LogError(amx, AMX_ERR_NATIVE, "Weapon limit reached.");
	return -1;
}

/**
 * Register weapon's forward.
 *
 * @param iWeaponID		The ID of registered weapon.
 * @param iForward		Forward type to register.
 * @param szCallBack	The forward to call.
 *
 * native wpnmod_register_weapon_forward(const iWeaponID, const e_Forwards: iForward, const szCallBack[]);
*/
static cell AMX_NATIVE_CALL wpnmod_register_weapon_forward(AMX *amx, cell *params)
{
	int iId = params[1];

	if (iId <= 0 || iId >= MAX_WEAPONS || WeaponInfoArray[iId].iType != Wpn_Custom)
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "Invalid weapon id provided. Got: %d  Valid: 16 up to %d.", iId, MAX_WEAPONS - 1);
		return 0;
	}

	int Fwd = params[2];

	if (Fwd < 0 || Fwd >= Fwd_Wpn_End)
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "Function out of bounds. Got: %d  Max: %d.", iId, Fwd_Wpn_End - 1);
		return 0;
	}

	const char *funcname = MF_GetAmxString(amx, params[3], 0, NULL);

	WeaponInfoArray[iId].iForward[Fwd] = MF_RegisterSPForwardByName
	(
		amx, 
		funcname, 
		FP_CELL, 
		FP_CELL, 
		FP_CELL, 
		FP_CELL, 
		FP_CELL, 
		FP_DONE
	);

	if (WeaponInfoArray[iId].iForward[Fwd] == -1)
	{
		WeaponInfoArray[iId].iForward[Fwd] = 0;
		MF_LogError(amx, AMX_ERR_NATIVE, "Function not found (%d, \"%s\").", Fwd, funcname);
		return 0;
	}

	return 1;
}

/**
 * Returns any ItemInfo variable for weapon. Use the e_ItemInfo_* enum.
 *
 * @param iId			The ID of registered weapon or weapon entity Id.
 * @param iInfoType		ItemInfo type.
 *
 * @return				Weapon's ItemInfo variable.
 *
 * native wpnmod_get_weapon_info(const iId, const e_ItemInfo: iInfoType, any:...);
 */
static cell AMX_NATIVE_CALL wpnmod_get_weapon_info(AMX *amx, cell *params)
{
	enum e_ItemInfo
	{
		ItemInfo_isCustom = 0,
		ItemInfo_iSlot,
		ItemInfo_iPosition,
		ItemInfo_iMaxAmmo1,
		ItemInfo_iMaxAmmo2,
		ItemInfo_iMaxClip,
		ItemInfo_iId,
		ItemInfo_iFlags,
		ItemInfo_iWeight,
		ItemInfo_szName,
		ItemInfo_szAmmo1,
		ItemInfo_szAmmo2,
		ItemInfo_szTitle,
		ItemInfo_szAuthor,
		ItemInfo_szVersion
	};

	int iId = params[1];
	int iSwitch = params[2];

	edict_t* pItem = NULL;

	if (iId >= MAX_WEAPONS)
	{
		CHECK_ENTITY(iId)
		pItem = INDEXENT2(iId);
	}
	else if (WeaponInfoArray[iId].iType == Wpn_None)
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "Invalid weapon id provided (%d).", iId);
		return 0;
	}
	
	if (iSwitch < ItemInfo_isCustom || iSwitch > ItemInfo_szVersion)
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "Undefined e_ItemInfo index: %d", iSwitch);
		return 0;
	}

	ItemInfo pII;
	memset(&pII, 0, sizeof pII);

	if (pItem)
	{
		GET_ITEM_INFO(pItem, &pII);
		iId = pII.iId;
	}
	else
	{
		pII.iId = iId;
		pII.iSlot = GetWeapon_Slot(iId);
		pII.iPosition = GetWeapon_ItemPosition(iId);
		pII.iMaxAmmo1 = GetWeapon_MaxAmmo1(iId);
		pII.iMaxAmmo2 = GetWeapon_MaxAmmo2(iId);
		pII.iMaxClip = GetWeapon_MaxClip(iId);
		pII.iFlags = GetWeapon_Flags(iId);
		pII.iWeight = GetWeapon_Weight(iId);
		pII.pszName = GetWeapon_pszName(iId);
		pII.pszAmmo1 = GetWeapon_pszAmmo1(iId);
		pII.pszAmmo2 = GetWeapon_pszAmmo2(iId);
	}

	size_t paramnum = params[0] / sizeof(cell);

	if (iSwitch >= ItemInfo_isCustom && iSwitch <= ItemInfo_iWeight && paramnum == 2)
	{
		switch (iSwitch)
		{
		case ItemInfo_isCustom:
			return WeaponInfoArray[iId].iType != Wpn_Default;
		case ItemInfo_iSlot:
			return pII.iSlot;
		case ItemInfo_iPosition:
			return pII.iPosition;
		case ItemInfo_iMaxAmmo1:
			return pII.iMaxAmmo1;
		case ItemInfo_iMaxAmmo2:
			return pII.iMaxAmmo2;
		case ItemInfo_iMaxClip:
			return pII.iMaxClip;
		case ItemInfo_iId:
			return pII.iId;
		case ItemInfo_iFlags:
			return pII.iFlags;
		case ItemInfo_iWeight:
			return pII.iWeight;
		}
	}
	else if (iSwitch >= ItemInfo_szName && iSwitch <= ItemInfo_szVersion && paramnum == 4)
	{
		const char* szReturnValue = NULL;

		switch (iSwitch)
		{
		case ItemInfo_szName:
			szReturnValue = pII.pszName;
			break;
		case ItemInfo_szAmmo1:
			szReturnValue = pII.pszAmmo1;
			break;
		case ItemInfo_szAmmo2:
			szReturnValue = pII.pszAmmo2;
			break;
		case ItemInfo_szTitle:
			szReturnValue = WeaponInfoArray[iId].title.c_str();
			break;
		case ItemInfo_szAuthor:
			szReturnValue = WeaponInfoArray[iId].author.c_str();
			break;
		case ItemInfo_szVersion:
			szReturnValue = WeaponInfoArray[iId].version.c_str();
			break;
		}	

		if (!szReturnValue)
			szReturnValue = "";

		return MF_SetAmxString(amx, params[3], szReturnValue, params[4]);
	}

	MF_LogError(amx, AMX_ERR_NATIVE, "Unknown e_ItemInfo index or return combination %d", iSwitch);
	return 0;
}

/**
 * Returns any AmmoInfo variable for ammobox. Use the e_AmmoInfo_* enum.
 *
 * @param iId			The ID of registered ammobox or ammobox entity Id.
 * @param iInfoType		e_AmmoInfo_* type.
 *
 * @return				Ammobox's AmmoInfo variable.
 *
 * native wpnmod_get_ammobox_info(const iId, const e_AmmoInfo: iInfoType, any:...);
 */
static cell AMX_NATIVE_CALL wpnmod_get_ammobox_info(AMX *amx, cell *params)
{
	enum e_AmmoInfo
	{
		AmmoInfo_szName
	};

	int iId = params[1];
	int iSwitch = params[2];

	edict_t* pAmmoBox = NULL;

	if (iId > g_iAmmoBoxIndex)
	{
		CHECK_ENTITY(iId)
		pAmmoBox = INDEXENT2(iId);
	}
	else if (iId < 0)
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "Invalid ammobox id provided. Got: %d  Valid: 0 up to %d.", iId, MAX_WEAPONS - 1);
		return 0;
	}

	if (iSwitch < AmmoInfo_szName || iSwitch > AmmoInfo_szName)
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "Undefined e_AmmoInfo index: %d", iSwitch);
		return 0;
	}

	if (pAmmoBox)
	{
		for (int i = 1; i <= g_iAmmoBoxIndex; i++)
		{
			if (!_stricmp(AmmoBoxInfoArray[i].classname.c_str(), STRING(pAmmoBox->v.classname)))
			{
				iId = i;
				break;
			}
		}
	}

	size_t paramnum = params[0] / sizeof(cell);

	if (iSwitch >= AmmoInfo_szName && iSwitch <= AmmoInfo_szName && paramnum == 4)
	{
		const char* szReturnValue = NULL;

		switch (iSwitch)
		{
		case AmmoInfo_szName:
			szReturnValue = AmmoBoxInfoArray[iId].classname.c_str();
			break;
		}	

		if (!szReturnValue)
			szReturnValue = "";

		return MF_SetAmxString(amx, params[3], szReturnValue, params[4]);
	}

	MF_LogError(amx, AMX_ERR_NATIVE, "Unknown e_AmmoInfo index or return combination %d", iSwitch);
	return 0;
}

/**
 * Gets number of registered weapons.
 *
 * @return		Number of registered weapons. (integer)
 *
 * native wpnmod_get_weapon_count();
*/
static cell AMX_NATIVE_CALL wpnmod_get_weapon_count(AMX *amx, cell *params)
{
	return g_iWeaponsCount;
}

/**
 * Gets number of registered ammoboxes.
 *
 * @return		Number of registered ammoboxes. (integer)
 *
 * native wpnmod_get_ammobox_count();
*/
static cell AMX_NATIVE_CALL wpnmod_get_ammobox_count(AMX *amx, cell *params)
{
	return g_iAmmoBoxIndex;
}

/**
 * Plays weapon's animation.
 *
 * @param iItem		Weapon's entity.
 * @param iAnim		Sequence number.
 *
 * native wpnmod_send_weapon_anim(const iItem, const iAnim);
*/
static cell AMX_NATIVE_CALL wpnmod_send_weapon_anim(AMX *amx, cell *params)
{
	CHECK_ENTITY(params[1])

	edict_t *pWeapon = INDEXENT(params[1]);
	edict_t *pPlayer = GetPrivateCbase(pWeapon, m_pPlayer);

	if (!IsValidPev(pPlayer))
	{
		return 0;
	}

	SendWeaponAnim(pPlayer, pWeapon, params[2]);
	return 1;
}

/**
 * Set the activity for player based on an event or current state.
 *
 * @param iPlayer		Player id.
 * @param iPlayerAnim	Animation (See PLAYER_ANIM constants).
 *
 * native wpnmod_set_player_anim(const iPlayer, const PLAYER_ANIM: iPlayerAnim);
*/
static cell AMX_NATIVE_CALL wpnmod_set_player_anim(AMX *amx, cell *params)
{
	int iPlayer = params[1];
	int iPlayerAnim = params[2];

	CHECK_ENTITY(iPlayer)

#ifdef _WIN32
	reinterpret_cast<void (__fastcall *)(void *, int, int)>(g_dllFuncs[Func_PlayerSetAnimation].address)((void*)INDEXENT2(iPlayer)->pvPrivateData, 0, iPlayerAnim);
#else
	reinterpret_cast<void (*)(void *, int)>(g_dllFuncs[Func_PlayerSetAnimation].address)((void*)INDEXENT2(iPlayer)->pvPrivateData, iPlayerAnim);
#endif

	return 1;
}

/**
* Get player's ammo inventory.
 *
 * @param iPlayer		Player id.
 * @param szAmmoName	Ammo type. ("9mm", "uranium", "MY_AMMO" etc..)
 *
 * @return				Amount of given ammo. (integer)
 *
 * native wpnmod_get_player_ammo(const iPlayer, const szAmmoName[]);
*/
static cell AMX_NATIVE_CALL wpnmod_get_player_ammo(AMX *amx, cell *params)
{
	int iPlayer = params[1];

	CHECK_ENTITY(iPlayer)

	int iAmmoIndex = GET_AMMO_INDEX(STRING(ALLOC_STRING(MF_GetAmxString(amx, params[2], 0, NULL))));

	if (iAmmoIndex != -1)
	{
		return (int)*((int *)INDEXENT2(iPlayer)->pvPrivateData + m_rgAmmo + iAmmoIndex - 1);
	}

	return 0;
}

/**
* Set player's ammo inventory.
 *
 * @param iPlayer		Player id.
 * @param szAmmoName	Ammo type. ("9mm", "uranium", "MY_AMMO" etc..)
 * @param iAmount		Ammo amount.
 *
 * native wpnmod_set_player_ammo(const iPlayer, const szAmmoName[], const iAmount);
*/
static cell AMX_NATIVE_CALL wpnmod_set_player_ammo(AMX *amx, cell *params)
{
	int iPlayer = params[1];

	CHECK_ENTITY(iPlayer)

	int iAmmoIndex = GET_AMMO_INDEX(STRING(ALLOC_STRING(MF_GetAmxString(amx, params[2], 0, NULL))));

	if (iAmmoIndex != -1)
	{
		*((int *)INDEXENT2(iPlayer)->pvPrivateData + m_rgAmmo + iAmmoIndex - 1) = params[3];
		return 1;
	}

	return 0;
}

/**
 * Sets an integer from private data.
 *
 * @param iEntity		Entity index.
 * @param iOffset		Offset (See e_Offsets constants).
 * @param iValue		Value.
 *
 * native wpnmod_set_offset_int(const iEntity, const e_Offsets: iOffset, const iValue);
*/
static cell AMX_NATIVE_CALL wpnmod_set_offset_int(AMX *amx, cell *params)
{
	int iEntity = params[1];
	int iOffset = params[2];
	int iValue = params[3];

	CHECK_ENTITY(iEntity)
	CHECK_OFFSET(iOffset)
	
	*((int *)INDEXENT2(iEntity)->pvPrivateData + PvDataOffsets[iOffset]) = iValue;
	return 1;
}

/**
 * Returns an integer from private data.
 *
 * @param iEntity		Entity index.
 * @param iOffset		Offset (See e_Offsets constants).
 * 
 * @return				Value from private data. (integer)
 *
 * native wpnmod_get_offset_int(const iEntity, const e_Offsets: iOffset);
*/
static cell AMX_NATIVE_CALL wpnmod_get_offset_int(AMX *amx, cell *params)
{
	int iEntity = params[1];
	int iOffset = params[2];

	CHECK_ENTITY(iEntity)
	CHECK_OFFSET(iOffset)

	return *((int *)INDEXENT2(iEntity)->pvPrivateData + PvDataOffsets[iOffset]);
}

/**
 * Sets a float from private data.
 *
 * @param iEntity		Entity index.
 * @param iOffset		Offset (See e_Offsets constants).
 * @param flValue		Value.
 *
 * native wpnmod_set_offset_float(const iEntity, const e_Offsets: iOffset, const Float: flValue);
*/
static cell AMX_NATIVE_CALL wpnmod_set_offset_float(AMX *amx, cell *params)
{
	int iEntity = params[1];
	int iOffset = params[2];

	float flValue = amx_ctof(params[3]);

	CHECK_ENTITY(iEntity)
	CHECK_OFFSET(iOffset)
	
	*((float *)INDEXENT2(iEntity)->pvPrivateData + PvDataOffsets[iOffset]) = flValue;
	return 1;
}

/**
 * Returns a float from private data.
 *
 * @param iEntity		Entity index.
 * @param iOffset		Offset (See e_Offsets constants).
 * 
 * @return				Value from private data. (float)
 *
 * native Float: wpnmod_get_offset_float(const iEntity, const e_Offsets: iOffset);
*/
static cell AMX_NATIVE_CALL wpnmod_get_offset_float(AMX *amx, cell *params)
{
	int iEntity = params[1];
	int iOffset = params[2];

	CHECK_ENTITY(iEntity)
	CHECK_OFFSET(iOffset)

	return amx_ftoc(*((float *)INDEXENT2(iEntity)->pvPrivateData + PvDataOffsets[iOffset]));
}

/**
 * Default deploy function.
 *
 * @param iItem				Weapon's entity index.
 * @param szViewModel		Weapon's view  model (V).
 * @param szWeaponModel		Weapon's player  model (P).
 * @param iAnim				Sequence number of deploy animation.
 * @param szAnimExt			Animation extension.
 *
 * native wpnmod_default_deploy(const iItem, const szViewModel[], const szWeaponModel[], const iAnim, const szAnimExt[]);
*/
static cell AMX_NATIVE_CALL wpnmod_default_deploy(AMX *amx, cell *params)
{
	int iEntity = params[1];
	int iAnim = params[4];

	const char *szViewModel = STRING(ALLOC_STRING(MF_GetAmxString(amx, params[2], 0, NULL))); 
	const char *szWeaponModel = STRING(ALLOC_STRING(MF_GetAmxString(amx, params[3], 0, NULL)));
	const char *szAnimExt = STRING(ALLOC_STRING(MF_GetAmxString(amx, params[5], 0, NULL)));

	CHECK_ENTITY(iEntity)

	edict_t* pItem = INDEXENT2(iEntity);
	edict_t* pPlayer = GetPrivateCbase(pItem, m_pPlayer);

	if (!IsValidPev(pPlayer))
	{
		return 0;
	}
	
	if (!Weapon_CanDeploy(pItem->pvPrivateData))
	{
		return 0;
	}

	pPlayer->v.viewmodel = MAKE_STRING(szViewModel);
	pPlayer->v.weaponmodel = MAKE_STRING(szWeaponModel);

	char *szData = (char *)pPlayer->pvPrivateData + m_szAnimExtention * 4;
		
	if (!IsBadWritePtr(szData, 1))
	{
		strcpy(szData, szAnimExt);
	}

	SendWeaponAnim(pPlayer, pItem, iAnim);

	*((float *)pPlayer->pvPrivateData + m_flNextAttack) = 0.5;
	*((float *)pItem->pvPrivateData + m_flTimeWeaponIdle) = 1.0;

	return 1;
}

/**
 * Default reload function.
 *
 * @param iItem				Weapon's entity index.
 * @param iClipSize			Maximum weapon's clip size.
 * @param iAnim				Sequence number of reload animation.
 * @param flDelay			Reload delay time.
 *
 * native wpnmod_default_reload(const iItem, const iClipSize, const iAnim, const Float: flDelay);
*/
static cell AMX_NATIVE_CALL wpnmod_default_reload(AMX *amx, cell *params)
{
	int iEntity = params[1];
	int iClipSize = params[2];
	int iAnim = params[3];

	float flDelay = amx_ctof(params[4]);

	CHECK_ENTITY(iEntity)

	edict_t* pItem = INDEXENT2(iEntity);
	edict_t* pPlayer = GetPrivateCbase(pItem, m_pPlayer);

	if (!IsValidPev(pPlayer))
	{
		return 0;
	}

	int iAmmo = Player_AmmoInventory(pPlayer, pItem, TRUE);

	if (!iAmmo)
	{
		return 0;
	}

	int j = min(iClipSize - *((int *)pItem->pvPrivateData + m_iClip), iAmmo);

	if (!j)
	{
		return 0;
	}

	*((int *)pItem->pvPrivateData + m_fInReload) = TRUE;	
	*((float *)pPlayer->pvPrivateData + m_flNextAttack) = flDelay;
	*((float *)pItem->pvPrivateData + m_flTimeWeaponIdle) = flDelay;

	SendWeaponAnim(pPlayer, pItem, iAnim);
	return 1;
}

/**
 * Sets weapon's think function. Analogue of set_task native.
 * 
 * Usage: 
 * 	wpnmod_set_think(iItem, "M249_CompleteReload");
 * 	set_pev(iItem, pev_nextthink, get_gametime() + 1.52);
 *
 * @param iItem				Weapon's entity index.
 * @param szCallBack		The forward to call.
 *
 * native wpnmod_set_think(const iItem, const szCallBack[]);
*/
static cell AMX_NATIVE_CALL wpnmod_set_think(AMX *amx, cell *params)
{
	int iEntity = params[1];

	CHECK_ENTITY(iEntity)

	char *funcname = MF_GetAmxString(amx, params[2], 0, NULL);

	int iForward = MF_RegisterSPForwardByName
	(
		amx, 
		funcname, 
		FP_CELL, 
		FP_CELL, 
		FP_CELL, 
		FP_CELL, 
		FP_CELL, 
		FP_DONE
	);

	if (iForward == -1)
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "Function not found (\"%s\").", funcname);
		return 0;
	}

	SetEntForward(INDEXENT2(iEntity), Think, (void*)Global_Think, iForward);
	return 1;
}

/**
 * Sets entity's touch function. 
 * 
 * @param iEntity			Entity index.
 * @param szCallBack		The forward to call.
 *
 * native wpnmod_set_touch(const iEntity, const szCallBack[]);
*/
static cell AMX_NATIVE_CALL wpnmod_set_touch(AMX *amx, cell *params)
{
	int iEntity = params[1];

	CHECK_ENTITY(iEntity)

	char *funcname = MF_GetAmxString(amx, params[2], 0, NULL);

	int iForward = MF_RegisterSPForwardByName
	(
		amx, 
		funcname, 
		FP_CELL, 
		FP_CELL, 
		FP_DONE
	);

	if (iForward == -1)
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "Function not found (\"%s\").", funcname);
		return 0;
	}

	SetEntForward(INDEXENT2(iEntity), Touch, (void*)Global_Touch, iForward);
	return 1;
}

/**
 * Fire bullets from player's weapon.
 *
 * @param iPlayer			Player index.
 * @param iAttacker			Attacker index (usualy it equal to previous param).
 * @param iShotsCount		Number of shots.
 * @param vecSpread			Spread.
 * @param flDistance		Max shot distance.
 * @param flDamage			Damage amount.
 * @param bitsDamageType	Damage type.
 * @param iTracerFreq		Tracer frequancy.
 *
 * native wpnmod_fire_bullets(const iPlayer, const iAttacker, const iShotsCount, const Float: vecSpread[3], const Float: flDistance, const Float: flDamage, const bitsDamageType, const iTracerFreq);
*/
static cell AMX_NATIVE_CALL wpnmod_fire_bullets(AMX *amx, cell *params)
{
	int iPlayer = params[1];
	int iAttacker = params[2];

	CHECK_ENTITY(iPlayer)
	CHECK_ENTITY(iAttacker)

	Vector vecSpread;

	cell *vSpread = MF_GetAmxAddr(amx, params[4]);

	vecSpread.x = amx_ctof(vSpread[0]);
	vecSpread.y = amx_ctof(vSpread[1]);
	vecSpread.z = amx_ctof(vSpread[2]);

	FireBulletsPlayer
	(
		INDEXENT2(iPlayer), 
		INDEXENT2(iAttacker), 
		params[3], 
		vecSpread, 
		amx_ctof(params[5]), 
		amx_ctof(params[6]), 
		params[7], 
		params[8]
	);

	return 1;
}

/**
 * Make damage upon entities within a certain range.
 * 	Only damage ents that can clearly be seen by the explosion.
 *
 * @param vecSrc			Origin of explosion.
 * @param iInflictor		Entity which causes the damage impact.
 * @param iAttacker			Attacker index.
 * @param flDamage			Damage amount.
 * @param flRadius			Damage radius.
 * @param iClassIgnore		Class to ignore.
 * @param bitsDamageType	Damage type (see CLASSIFY defines).
 *
 * native wpnmod_radius_damage(const Float: vecSrc[3], const iInflictor, const iAttacker, const Float: flDamage, const Float: flRadius, const iClassIgnore, const bitsDamageType);
*/
static cell AMX_NATIVE_CALL wpnmod_radius_damage(AMX *amx, cell *params)
{
	Vector vecSrc;
	cell *vSrc = MF_GetAmxAddr(amx, params[1]);

	vecSrc.x = amx_ctof(vSrc[0]);
	vecSrc.y = amx_ctof(vSrc[1]);
	vecSrc.z = amx_ctof(vSrc[2]);

	int iInflictor = params[2];
	int iAttacker = params[3];

	CHECK_ENTITY(iInflictor)
	CHECK_ENTITY(iAttacker)
	
#ifdef _WIN32
	reinterpret_cast<int (__cdecl *)(Vector, entvars_t*, entvars_t*, float, float, int, int)>(g_dllFuncs[Func_RadiusDamage].address)
	(vecSrc, &INDEXENT2(iInflictor)->v, &INDEXENT2(iAttacker)->v, amx_ctof(params[4]), amx_ctof(params[5]), params[6], params[7]);
#else
	reinterpret_cast<int (*)(Vector, entvars_t*, entvars_t*, float, float, int, int)>(g_dllFuncs[Func_RadiusDamage].address)
	(vecSrc, &INDEXENT2(iInflictor)->v, &INDEXENT2(iAttacker)->v, amx_ctof(params[4]), amx_ctof(params[5]), params[6], params[7]);
#endif

	return 1;
}

/**
 * Same as wpnmod_radius_damage, but blocks 'ghost mines' and 'ghost nades' 
 *  by fixing two bugs found in HLDM gamedll and HL engine.
 *
 * @param vecSrc			Origin of explosion.
 * @param iInflictor		Entity which causes the damage impact.
 * @param iAttacker			Attacker index.
 * @param flDamage			Damage amount.
 * @param flRadius			Damage radius.
 * @param iClassIgnore		Class to ignore.
 * @param bitsDamageType	Damage type (see CLASSIFY defines).
 *
 * native wpnmod_radius_damage2(const Float: vecSrc[3], const iInflictor, const iAttacker, const Float: flDamage, const Float: flRadius, const iClassIgnore, const bitsDamageType);
*/
static cell AMX_NATIVE_CALL wpnmod_radius_damage2(AMX *amx, cell *params)
{
	Vector vecSrc;
	cell *vSrc = MF_GetAmxAddr(amx, params[1]);

	vecSrc.x = amx_ctof(vSrc[0]);
	vecSrc.y = amx_ctof(vSrc[1]);
	vecSrc.z = amx_ctof(vSrc[2]);

	int iInflictor = params[2];
	int iAttacker = params[3];

	CHECK_ENTITY(iInflictor)
	CHECK_ENTITY(iAttacker)

	RadiusDamage2(vecSrc, INDEXENT2(iInflictor), INDEXENT2(iAttacker), amx_ctof(params[4]), amx_ctof(params[5]), params[6], params[7]);
	return 1;
}

/**
 * Eject a brass from player's weapon.
 *
 * @param iPlayer			Player index.
 * @param iShellModelIndex	Index of precached shell's model.
 * @param iSoundtype		Bounce sound type (see defines).
 * @param flForwardScale	Forward scale value.
 * @param flUpScale			Up scale value.
 * @param flRightScale		Right scale value.
 *
 * native wpnmod_eject_brass(const iPlayer, const iShellModelIndex, const iSoundtype, const Float: flForwardScale, const Float: flUpScale, const Float: flRightScale);
*/
static cell AMX_NATIVE_CALL wpnmod_eject_brass(AMX *amx, cell *params)
{
	int iPlayer = params[1];

	CHECK_ENTITY(iPlayer)

	edict_t* pPlayer = INDEXENT2(iPlayer);

	Vector vecShellVelocity = pPlayer->v.velocity + gpGlobals->v_right * RANDOM_FLOAT(50, 70) + gpGlobals->v_up * RANDOM_FLOAT(100, 150) + gpGlobals->v_forward * 25;
	
	UTIL_EjectBrass
	(
		pPlayer->v.origin + pPlayer->v.view_ofs + gpGlobals->v_up * amx_ctof(params[5]) + gpGlobals->v_forward * amx_ctof(params[4]) + gpGlobals->v_right * amx_ctof(params[6]), 
		vecShellVelocity, 
		pPlayer->v.angles.y, 
		params[2], 
		params[3]
	);

	return 1;
}

/**
 * Sets the weapon so that it can play empty sound again.
 *
 * @param iItem				Weapon's entity index.
 *
 * native wpnmod_reset_empty_sound(const iItem);
*/
static cell AMX_NATIVE_CALL wpnmod_reset_empty_sound(AMX *amx, cell *params)
{
	int iEntity = params[1];

	CHECK_ENTITY(iEntity)

	*((int *)INDEXENT2(iEntity)->pvPrivateData + m_iPlayEmptySound) = 1;
	return 1;
}

/**
 * Plays the weapon's empty sound.
 *
 * @param iItem				Weapon's entity index.
 *
 * native wpnmod_play_empty_sound(const iItem);
*/
static cell AMX_NATIVE_CALL wpnmod_play_empty_sound(AMX *amx, cell *params)
{
	int iEntity = params[1];

	CHECK_ENTITY(iEntity)

	if (*((int *)INDEXENT2(iEntity)->pvPrivateData + m_iPlayEmptySound))
	{
		edict_t* pPlayer = GetPrivateCbase(INDEXENT2(iEntity), m_pPlayer);

		if (IsValidPev(pPlayer))
		{
			EMIT_SOUND_DYN2(pPlayer, CHAN_WEAPON, "weapons/357_cock1.wav", 0.8, ATTN_NORM, 0, PITCH_NORM);
			*((int *)INDEXENT2(iEntity)->pvPrivateData + m_iPlayEmptySound) = 0;
			
			return 1;
		}
	}

	return 0;
}

/**
 * Spawn an item by name.
 *
 * @param szName			Item's name.
 * @param vecOrigin			Origin were to spawn.
 * @param vecAngles			Angles.
 *
 * @return					Item entity index or -1 on failure. (integer)
 *
 * native wpnmod_create_item(const szName[], const Float: vecOrigin[3] = {0.0, 0.0, 0.0}, const Float: vecAngles[3] = {0.0, 0.0, 0.0});
*/
static cell AMX_NATIVE_CALL wpnmod_create_item(AMX *amx, cell *params)
{
	char *itemname = MF_GetAmxString(amx, params[1], 0, NULL);

	Vector vecOrigin;
	cell *vOrigin = MF_GetAmxAddr(amx, params[2]);

	vecOrigin.x = amx_ctof(vOrigin[0]);
	vecOrigin.y = amx_ctof(vOrigin[1]);
	vecOrigin.z = amx_ctof(vOrigin[2]);

	Vector vecAngles;
	cell *vAngles = MF_GetAmxAddr(amx, params[3]);

	vecAngles.x = amx_ctof(vAngles[0]);
	vecAngles.y = amx_ctof(vAngles[1]);
	vecAngles.z = amx_ctof(vAngles[2]);

	edict_t* iItem = Weapon_Spawn(itemname, vecOrigin, vecAngles);

	if (IsValidPev(iItem))
	{
		return ENTINDEX(iItem);
	}
	else
	{
		edict_t* iItem = Ammo_Spawn(itemname, vecOrigin, vecAngles);

		if (IsValidPev(iItem))
		{
			return ENTINDEX(iItem);
		}	
	}

	return -1;
}

/**
 * Register new ammobox in module.
 *
 * @param szName			The ammobox classname.
 * 
 * @return					The ID of registerd ammobox or -1 on failure. (integer)
 *
 * native wpnmod_register_ammobox(const szClassname[]);
 */
static cell AMX_NATIVE_CALL wpnmod_register_ammobox(AMX *amx, cell *params)
{
	if (g_iAmmoBoxIndex >= MAX_WEAPONS - 1)
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "Ammobox limit reached.");
		return -1;
	}

	if (!g_AmmoBoxHooksEnabled)
	{
		g_AmmoBoxHooksEnabled = TRUE;
		SetHookVirt(&g_RpgAddAmmo_Hook);
	}

	const char *szAmmoboxName = MF_GetAmxString(amx, params[1], 0, NULL);

	for (int i = 1; i <= g_iAmmoBoxIndex; i++)
	{
		if (!_stricmp(AmmoBoxInfoArray[i].classname.c_str(), szAmmoboxName))
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Ammobox name is duplicated.");
			return -1;
		}
	}

	AmmoBoxInfoArray[++g_iAmmoBoxIndex].classname.assign(STRING(ALLOC_STRING(szAmmoboxName)));
	return g_iAmmoBoxIndex;
}

/**
 * Register ammobox's forward.
 *
 * @param iAmmoboxID		The ID of registered ammobox.
 * @param iForward			Forward type to register.
 * @param szCallBack		The forward to call.
 *
 * native wpnmod_register_ammobox_forward(const iWeaponID, const e_AmmoFwds: iForward, const szCallBack[]);
 */
static cell AMX_NATIVE_CALL wpnmod_register_ammobox_forward(AMX *amx, cell *params)
{
	int iId = params[1];

	if (iId <= 0 || iId >= MAX_WEAPONS)
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "Invalid ammobox id provided. Got: %d  Valid: 1 up to %d.", iId, MAX_WEAPONS - 1);
		return 0;
	}

	int Fwd = params[2];

	if (Fwd < 0 || Fwd >= Fwd_Ammo_End)
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "Function out of bounds. Got: %d  Max: %d.", iId, Fwd_Ammo_End - 1);
		return 0;
	}

	const char *funcname = MF_GetAmxString(amx, params[3], 0, NULL);

	AmmoBoxInfoArray[iId].iForward[Fwd] = MF_RegisterSPForwardByName
	(
		amx, 
		funcname, 
		FP_CELL,
		FP_CELL,
		FP_DONE
	);

	if (AmmoBoxInfoArray[iId].iForward[Fwd] == -1)
	{
		AmmoBoxInfoArray[iId].iForward[Fwd] = 0;
		MF_LogError(amx, AMX_ERR_NATIVE, "Function not found (%d, \"%s\").", Fwd, funcname);
		return 0;
	}

	return 1;
}

/**
 * Resets the global multi damage accumulator.
 *
 * native wpnmod_clear_multi_damage();
 */
static cell AMX_NATIVE_CALL wpnmod_clear_multi_damage(AMX *amx, cell *params)
{
	CLEAR_MULTI_DAMAGE();
	return 1;
}

/**
 * Inflicts contents of global multi damage register on entity.
 *
 * @param iInflictor		Entity which causes the damage impact.
 * @param iAttacker			Attacker index.
 *
 * native wpnmod_apply_multi_damage(const iInflictor, const iAttacker);
 */
static cell AMX_NATIVE_CALL wpnmod_apply_multi_damage(AMX *amx, cell *params)
{
	int iInflictor = params[1];
	int iAttacker = params[2];

	CHECK_ENTITY(iInflictor)
	CHECK_ENTITY(iAttacker)

	APPLY_MULTI_DAMAGE(INDEXENT2(iInflictor), INDEXENT2(iAttacker));
	return 1;
}


AMX_NATIVE_INFO Natives[] = 
{
	// Main
	{ "wpnmod_register_weapon", wpnmod_register_weapon},
	{ "wpnmod_register_weapon_forward", wpnmod_register_weapon_forward},
	{ "wpnmod_register_ammobox", wpnmod_register_ammobox},
	{ "wpnmod_register_ammobox_forward", wpnmod_register_ammobox_forward},
	{ "wpnmod_get_weapon_info", wpnmod_get_weapon_info},
	{ "wpnmod_get_ammobox_info", wpnmod_get_ammobox_info},
	{ "wpnmod_get_weapon_count", wpnmod_get_weapon_count},
	{ "wpnmod_get_ammobox_count", wpnmod_get_ammobox_count},
	{ "wpnmod_send_weapon_anim", wpnmod_send_weapon_anim},
	{ "wpnmod_set_player_anim", wpnmod_set_player_anim},
	{ "wpnmod_set_think", wpnmod_set_think},
	{ "wpnmod_set_touch", wpnmod_set_touch},
	{ "wpnmod_set_offset_int", wpnmod_set_offset_int},
	{ "wpnmod_set_offset_float", wpnmod_set_offset_float},
	{ "wpnmod_get_offset_int", wpnmod_get_offset_int},
	{ "wpnmod_get_offset_float", wpnmod_get_offset_float},
	{ "wpnmod_get_player_ammo", wpnmod_get_player_ammo},
	{ "wpnmod_set_player_ammo", wpnmod_set_player_ammo},
	{ "wpnmod_default_deploy", wpnmod_default_deploy},
	{ "wpnmod_default_reload", wpnmod_default_reload},
	{ "wpnmod_fire_bullets", wpnmod_fire_bullets},
	{ "wpnmod_radius_damage", wpnmod_radius_damage},
	{ "wpnmod_radius_damage2", wpnmod_radius_damage2},
	{ "wpnmod_clear_multi_damage", wpnmod_clear_multi_damage},
	{ "wpnmod_apply_multi_damage", wpnmod_apply_multi_damage},
	{ "wpnmod_eject_brass", wpnmod_eject_brass},
	{ "wpnmod_reset_empty_sound", wpnmod_reset_empty_sound},
	{ "wpnmod_play_empty_sound", wpnmod_play_empty_sound},
	{ "wpnmod_create_item", wpnmod_create_item},

	{ NULL, NULL }
};
