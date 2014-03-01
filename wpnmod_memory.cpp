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

#include "wpnmod_memory.h"


void *g_pParseDllFuncs[] =
{
	&Parse_ClearMultiDamage,
	&Parse_ApplyMultiDamage,
	&Parse_PrecacheOtherWeapon,
	&Parse_GetAmmoIndex,
	&Parse_GiveNamedItem,
	&Parse_SetAnimation,
	NULL,
};

bool FindFuncsInDll(size_t start, size_t end)
{
	bool bSuccess = true;

	void **func = g_pParseDllFuncs;

	while (*func != NULL)
	{
		if (!reinterpret_cast<bool (*)(size_t, size_t)>(*func)(start, end))
		{
			bSuccess = false;
		}

		func++;
	}
	
	return bSuccess;
}

// 
// void ClearMultiDamage(void)
//

bool Parse_ClearMultiDamage(size_t start, size_t end)
{
	size_t pAdress	= (size_t)FindFunction(&g_GameDllModule, "?SuperBounceTouch@CSqueakGrenade@@AAEXPAVCBaseEntity@@@Z");

	char				funcname[]			= "ClearMultiDamage";
	char				mask[]				= "x?????x?????x";
	unsigned char		pattern[]			= "\x3B\x00\x00\x00\x00\x00\x0F\x00\x00\x00\x00\x00\xE8";
	size_t				BytesOffset			= 13;

	if (!pAdress)
	{
		printf2("[%s]:   Error: \"%s\" not found [0]\n", Plugin_info.logtag, funcname);
		return false;
	}

	pAdress = ParseFunc(pAdress, pAdress + 300, funcname, pattern, mask, BytesOffset);
	
	if (pAdress == NULL)
	{
		return false;
	}

	g_dllFuncs[Func_ClearMultiDamage].address = (void*)pAdress;

	return true;
}

// 
// void ApplyMultiDamage(entvars_t *pevInflictor, entvars_t *pevAttacker)
//

bool Parse_ApplyMultiDamage(size_t start, size_t end)
{
	size_t pAdress	= (size_t)FindFunction(&g_GameDllModule, "?SuperBounceTouch@CSqueakGrenade@@AAEXPAVCBaseEntity@@@Z");

	char				funcname[]			= "ApplyMultiDamage";
	char				mask[]				= "xxx????x";
	unsigned char		pattern[]			= "\x50\x50\xE8\x00\x00\x00\x00\x8B";
	size_t				BytesOffset			= 3;

	if (!pAdress)
	{
		printf2("[%s]:   Error: \"%s\" not found [0]\n", Plugin_info.logtag, funcname);
		return false;
	}

	pAdress = ParseFunc(pAdress, pAdress + 700, funcname, pattern, mask, BytesOffset);
	
	if (pAdress == NULL)
	{
		return false;
	}

	g_dllFuncs[Func_ApplyMultiDamage].address = (void*)pAdress;

	return true;
}

// 
// void UTIL_PrecacheOtherWeapon(const char *szClassname)
//

bool Parse_PrecacheOtherWeapon(size_t start, size_t end)
{
	char			funcname[]			= "UTIL_PrecacheOtherWeapon";
	char			string[]			= "weapon_rpg";
	char			mask[]				= "xxxxxx";
	unsigned char	pattern[]			= "\x68\x00\x00\x00\x00\xE8";
	size_t			BytesOffset			= 6;

	size_t pAdress = ParseFunc(start, end, funcname, string, pattern, mask, BytesOffset);

	if (pAdress == NULL)
	{
		return false;
	}

	g_dllFuncs[Func_PrecacheOtherWeapon].address = (void*)pAdress;
	
	if (!CreateFunctionHook(&g_dllFuncs[Func_PrecacheOtherWeapon]))
	{
		printf2("[%s]:   Error: failed to hook \"%s\"\n", Plugin_info.logtag, funcname);
		return false;
	}

	SetHook(&g_dllFuncs[Func_PrecacheOtherWeapon]);
	return true;
}

// 
// int CBasePlayer::GetAmmoIndex(const char *psz)
//

bool Parse_GetAmmoIndex(size_t start, size_t end)
{
	char			funcname[]			= "CBasePlayer::GetAmmoIndex";
	char			string[]			= "357";
	char			mask[]				= "xxxxxxx?x";
	unsigned char	pattern[]			= "\x68\x00\x00\x00\x00\x89\x46\x00\xE8";
	size_t			BytesOffset			= 9;

	size_t pAdress = ParseFunc(start, end, funcname, string, pattern, mask, BytesOffset);

	if (!pAdress)
	{
		return false;
	}

	g_dllFuncs[Func_GetAmmoIndex].address = (void*)pAdress;

	return true;
}

// 
// void CBasePlayer::GiveNamedItem(const char *pszName)
//

bool Parse_GiveNamedItem(size_t start, size_t end)
{
	char			funcname[]		= "CBasePlayer::GiveNamedItem";
	char			string[]		= "weapon_crowbar";
	char			mask[]			= "xxxxxx?x";
	unsigned char	pattern[]		= "\x68\x00\x00\x00\x00\x8B\x00\xE8";
	size_t			BytesOffset		= 8;

	size_t pAdress = ParseFunc(start, end, funcname, string, pattern, mask, BytesOffset);

	if (pAdress == NULL)
	{
		return false;
	}

	g_dllFuncs[Func_GiveNamedItem].address = (void*)pAdress;

	if (!CreateFunctionHook(&g_dllFuncs[Func_GiveNamedItem]))
	{
		printf2("[%s]:   Error: failed to hook \"%s\"\n", Plugin_info.logtag, funcname);
		return false;
	}

	SetHook(&g_dllFuncs[Func_GiveNamedItem]);
	return true;
}

// 
// void CBasePlayer::SetAnimation(PLAYER_ANIM playerAnim)
//

bool Parse_SetAnimation(size_t start, size_t end)
{
	char			funcname[]		= "CBasePlayer::SetAnimation";
	
	char			string[]		= "models/v_satchel_radio.mdl";
	char			mask[]			= "x??xxxxx";
	unsigned char	pattern[]		= "\x8B\x00\x00\xB9\x00\x00\x00\x00";

	char			mask2[]			= "xx?xxx";
	unsigned char	pattern2[]		= "\x8B\x4E\x00\x6A\x05\xE8";
	size_t			BytesOffset		= 6;

	int count = 0;

	size_t pAdress = NULL;
	size_t pCurrent = NULL;
	size_t pCandidate = NULL;
	
	pCurrent = FindStringInDLL(start, end, string);

	while (pCurrent != NULL)
	{
		*(size_t*)(pattern + 4) = (size_t)pCurrent;

		if ((pCandidate = FindAdressInDLL(start, end, pattern, mask)) != NULL)
		{
			count++;
			pAdress = pCandidate;
		}

		pCurrent = FindStringInDLL(pCurrent + 1, end, string);
	}

	if (!count)
	{
		printf2("[%s]:   Error: \"%s\" not found [0]\n", Plugin_info.logtag, funcname);
		return false;
	}
	else if (count > 1)
	{
		printf2("[%s]:   Error: %d candidates found for \"%s\"\n", Plugin_info.logtag, count, funcname);
		return false;
	}

	pAdress = ParseFunc(pAdress, pAdress + 150, funcname, pattern2, mask2, BytesOffset);
	
	if (pAdress == NULL)
	{
		return false;
	}

	g_dllFuncs[Func_PlayerSetAnimation].address = (void*)pAdress;

	return true;
}

void EnableShieldHitboxTracing()
{
	bool bShieldRegistered = false;

	for (int i = 1; i <= g_iWeaponsCount; i++)
	{
		if (WeaponInfoArray[i].iType == Wpn_Custom && !stricmp(GetWeapon_pszName(i), "weapon_rpg7"))
		{
			bShieldRegistered = true;
			break;
		}
	}

	if (!bShieldRegistered)
	{
		return;
	}

	module hEngine = { NULL, NULL, NULL };

	if (!FindModuleByAddr((void*)g_engfuncs.pfnAlertMessage, &hEngine))
	{
		printf2("[%s]: Failed to locate engine, shield hitbox tracing not active.\n", Plugin_info.logtag);
		return;
	}

	#ifdef __linux__

		size_t pAdress = (size_t)FindFunction(&hEngine, "g_bIsCStrike");

	#else

		signature sig =
		{
			"\xC3\xE8\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x85\xC0\x75\x00\xA1",
			"xx????x????xxx?x", 16
		};

		size_t pAdress = (size_t)FindFunction(&hEngine, sig);

		if (pAdress)
		{
			pAdress += 7;
		}

	#endif 

	if (!pAdress)
	{
		printf2("[%s]: Failed to enable shield hitbox tracing.\n", Plugin_info.logtag);
	}
	else
	{
		*(int*)*(size_t*)(pAdress) = 1;
		printf2("[%s]: Shield hitbox tracing enabled at %p\n", Plugin_info.logtag, pAdress);
	}
}

size_t ParseFunc(size_t start, size_t end, char* funcname, unsigned char* pattern, char* mask, size_t bytes)
{
	int count = 0;

	size_t pAdress = NULL;
	size_t pCurrent = NULL;
	
	pCurrent = FindAdressInDLL(start, end, pattern, mask);

	while (pCurrent != NULL)
	{
		count++;
		pAdress = pCurrent;
		pCurrent = FindAdressInDLL(pCurrent + 1, end, pattern, mask);
	}

	if (!count)
	{
		printf2("[%s]:   Error: \"%s\" not found\n", Plugin_info.logtag, funcname);
		return false;
	}
	else if (count > 1)
	{
		printf2("[%s]:   Error: %d candidates found for \"%s\"\n", Plugin_info.logtag, count, funcname);
		return false;
	}

	pAdress += bytes;
	pAdress = *(size_t*)pAdress + pAdress + 4;

	printf2("[%s]:   Found \"%s\" at %p\n", Plugin_info.logtag, funcname, pAdress);
	
	return pAdress;
}

size_t ParseFunc(size_t start, size_t end, char* funcname, char* string, unsigned char* pattern, char* mask, size_t bytes)
{
	int count = 0;

	size_t pAdress = NULL;
	size_t pCurrent = NULL;
	size_t pCandidate = NULL;
	
	pCurrent = FindStringInDLL(start, end, string);

	while (pCurrent != NULL)
	{
		*(size_t*)(pattern + 1) = (size_t)pCurrent;

		if ((pCandidate = FindAdressInDLL(start, end, pattern, mask)) != NULL)
		{
			count++;
			pAdress = pCandidate;
		}

		pCurrent = FindStringInDLL(pCurrent + 1, end, string);
	}

	if (!count)
	{
		printf2("[%s]:   Error: \"%s\" not found\n", Plugin_info.logtag, funcname);
		return false;
	}
	else if (count > 1)
	{
		printf2("[%s]:   Error: %d candidates found for \"%s\"\n", Plugin_info.logtag, count, funcname);
		return false;
	}

	pAdress += bytes;
	pAdress = *(size_t*)pAdress + pAdress + 4;

	printf2("[%s]:   Found \"%s\" at %p\n", Plugin_info.logtag, funcname, pAdress);

	return pAdress;
}








typedef struct
{
	int	iValue;
	int	iExtraOffset;
} GameOffset;

//
// Default vtbl offsets for Bugfixed and improved HL release.
//
GameOffset g_vtbl_offsets[VO_End] = 
{
	{0,		2},		// Spawn
	{1,		2},		// Precache
	{8,		2},		// Classify
	{10,	2},		// TraceAttack
	{11,	2},		// TakeDamage
	{28,	2},		// DamageDecal
	{47,	2},		// Respawn
	{57,	2},		// AddAmmo
	{58,	2},		// AddToPlayer
	{60,	2},		// GetItemInfo
	{61,	2},		// CanDeploy
	{62,	2},		// Deploy
	{63,	2},		// CanHolster
	{64,	2},		// Holster
	{67,	2},		// ItemPostFrame
	{75,	2},		// ItemSlot
	{82,	2},		// IsUseable
	{0,		2}
};

//
// Default vtbl offsets for Bugfixed and improved HL release.
//
GameOffset g_pvData_offsets[pvData_End] = 
{
};







void Offsets_Init(void)
{
	//int extra_cbase = 0;
	int extra_vtable = 0;
	//int extra_cbase = 0;

	// if (g_GameMod == SUBMOD_AGHLRU)
	{
#ifdef _WIN32
		SetVTableOffsetPev(4);
		SetVTableOffsetBase(0x0);
#else
		SetVTableOffsetPev(0);
		SetVTableOffsetBase(0x60);

		extra_vtable = 2;

#endif
		g_vtblOffsets[VO_Spawn]				= extra_vtable +	0;
		g_vtblOffsets[VO_Precache]			= extra_vtable +	1;
		g_vtblOffsets[VO_Classify]			= extra_vtable +	8;
		g_vtblOffsets[VO_TraceAttack]		= extra_vtable +	10;
		g_vtblOffsets[VO_TakeDamage]		= extra_vtable +	11;
		g_vtblOffsets[VO_DamageDecal]		= extra_vtable +	28;
		g_vtblOffsets[VO_Respawn]			= extra_vtable +	47;
		g_vtblOffsets[VO_AddAmmo]			= extra_vtable +	57;
		g_vtblOffsets[VO_AddToPlayer]		= extra_vtable +	58;
		g_vtblOffsets[VO_GetItemInfo]		= extra_vtable +	60;
		g_vtblOffsets[VO_CanDeploy]			= extra_vtable +	61;
		g_vtblOffsets[VO_Deploy]			= extra_vtable +	62;
		g_vtblOffsets[VO_CanHolster]		= extra_vtable +	63;
		g_vtblOffsets[VO_Holster]			= extra_vtable +	64;
		g_vtblOffsets[VO_ItemPostFrame]		= extra_vtable +	67;
		g_vtblOffsets[VO_ItemSlot]			= extra_vtable +	75;
		g_vtblOffsets[VO_IsUseable]			= extra_vtable +	82;

		g_pvDataOffsets[pvData_pfnThink] = 4;
		g_pvDataOffsets[pvData_pfnTouch] = 5;


		
		int pvdata[pvData_End] =
		{
			4,		// m_pfnThink
			5,		// m_pfnTouch
			8,		// ammo_9mm
			9,		// ammo_357
			10,		// ammo_bolts
			11,		// ammo_buckshot
			12,		// ammo_rockets
			13,		// ammo_uranium
			14,		// ammo_hornets
			15,		// ammo_argrens
			16,		// m_flStartThrow
			17,		// m_flReleaseThrow
			18,		// m_chargeReady
			19,		// m_fInAttack
			20,		// m_fireState
			28,		// m_pPlayer
			29,		// m_pNext
			30,		// m_iId
			31,		// m_iPlayEmptySound
			32,		// m_fFireOnEmpty
			33,		// m_flPumpTime
			34,		// m_fInSpecialReload
			35,		// m_flNextPrimaryAttack
			36,		// m_flNextSecondaryAttack
			37,		// m_flTimeWeaponIdle
			38,		// m_iPrimaryAmmoType
			39,		// m_iSecondaryAmmoType
			40,		// m_iClip
			43,		// m_fInReload
			44,		// m_iDefaultAmmo
			90,		// m_LastHitGroup
			148,	// m_flNextAttack
			173,	// m_iWeaponVolume
			175,	// m_iWeaponFlash
			298,	// m_iFOV
			300,	// m_rgpPlayerItems
			306,	// m_pActiveItem
			308,	// m_pLastItem
			310,	// m_rgAmmo
			387		// m_szAnimExtention
		};

		memcpy(g_pvDataOffsets, pvdata, sizeof(int) * pvData_End);
	}
	if (g_GameMod == SUBMOD_AG || g_GameMod == SUBMOD_MINIAG)
	{
	}
	else if (g_GameMod == SUBMOD_GEARBOX)
	{
		// More slots in OP4
		g_iMaxWeaponSlots = 7;
	}
}

