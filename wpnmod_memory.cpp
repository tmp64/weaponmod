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
#include "wpnmod_hooks.h"

CMemory g_Memory;

function g_fh_GiveNamedItem = HOOK_FUNC_DLL(GiveNamedItem_HookHandler);
function g_fh_funcPackWeapon = HOOK_FUNC_DLL(PackWeapon_HookHandler);
function g_fh_PrecacheOtherWeapon = HOOK_FUNC_DLL(PrecacheOtherWeapon_HookHandler);
function g_fh_FallThink = HOOK_FUNC_DLL(CBasePlayerItem_FallThink_HookHandler);
function g_fh_AmmoSpawn = HOOK_FUNC_DLL(CBasePlayerAmmoSpawn_HookHandler);
function g_fh_ItemSpawn = HOOK_FUNC_DLL(CItemSpawn_HookHandler);
function g_fh_GetDispatch = HOOK_FUNC_ENGINE(GetDispatch_HookHandler);
function g_fh_CallGameEntity = HOOK_FUNC_METAMOD(CallGameEntity_HookHandler);

CMemory::CMemory()
{
	m_bIsNewGCC = false;

	m_pSubRemove = NULL;
	m_pWpnBoxKillThink = NULL;
	m_pGetAmmoIndex = NULL;
	m_pClearMultiDamage = NULL;
	m_pApplyMultiDamage = NULL;
	m_pPlayerSetAnimation = NULL;
}

bool CMemory::Init(void)
{
	if (!FindModuleByAddr((void*)g_engfuncs.pfnAlertMessage, &m_EngineModule))
	{
		WPNMOD_LOG("  Failed to locate game engine!\n"); // But how? o_O
		return false;
	}

	if (!FindModuleByAddr((void*)gpMetaUtilFuncs->pfnGetGameInfo, &m_MetamodModule))
	{
		WPNMOD_LOG("  Failed to locate metamod!\n"); // But how? o_O
		return false;
	}

	if (!FindModuleByAddr((void*)MDLL_FUNC->pfnGetGameDescription(), &m_GameDllModule))
	{
		WPNMOD_LOG("  Failed to locate %s\n", GET_GAME_INFO(PLID, GINFO_DLL_FILENAME));
		return false;
	}

	WPNMOD_LOG("  Found %s at %p\n", GetDllNameByModule(m_EngineModule.base), m_EngineModule.base);
	WPNMOD_LOG("  Found %s at %p\n", GetDllNameByModule(m_MetamodModule.base), m_MetamodModule.base);
	WPNMOD_LOG("  Found %s at %p\n", GET_GAME_INFO(PLID, GINFO_DLL_FILENAME), m_GameDllModule.base);

	m_start_gamedll = (size_t)m_GameDllModule.base;
	m_end_gamedll = (size_t)m_GameDllModule.base + (size_t)m_GameDllModule.size;

	m_start_engine = (size_t)m_EngineModule.base;
	m_end_engine = (size_t)m_EngineModule.base + (size_t)m_EngineModule.size;

	m_bSuccess = true;

	Parse_GetDispatch();
	Parse_CallGameEntity();
	Parse_ClearMultiDamage();
	Parse_ApplyMultiDamage();
	Parse_PrecacheOtherWeapon();
	Parse_GetAmmoIndex();
	Parse_GiveNamedItem();
	Parse_SetAnimation();
	Parse_SubRemove();
	Parse_FallThink();
	Parse_AmmoSpawn();
	Parse_ItemSpawn();

	if (!m_bSuccess)
	{
		UnsetHooks();
	}
	else
	{
		WPNMOD_LOG(" Memory initialization ended.\n");
	}

	return m_bSuccess;
}

void CMemory::UnsetHooks(void)
{
	UnsetHook(&g_fh_GiveNamedItem);
	UnsetHook(&g_fh_PrecacheOtherWeapon);
	UnsetHook(&g_fh_FallThink);
	UnsetHook(&g_fh_AmmoSpawn);
	UnsetHook(&g_fh_ItemSpawn);
	UnsetHook(&g_fh_GetDispatch);
	UnsetHook(&g_fh_CallGameEntity);
}

void CMemory::Parse_ClearMultiDamage(void)
{
	char funcname[] = "ClearMultiDamage (gamedll)";

#ifdef __linux__

	size_t pAdress	= (size_t)FindFunction(&m_GameDllModule, "ClearMultiDamage__Fv");

	if (!pAdress)
	{
		m_bIsNewGCC = true;
		pAdress	= (size_t)FindFunction(&m_GameDllModule, "_Z16ClearMultiDamagev");
	}

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found\n", funcname);
		m_bSuccess = false;
		return;
	}

	WPNMOD_LOG_ONLY("   Found \"%s\" at %p\n", funcname, pAdress);

#else

	size_t pAdress = (size_t)FindFunction(&m_GameDllModule, "?SuperBounceTouch@CSqueakGrenade@@AAEXPAVCBaseEntity@@@Z");

	char			mask[]			= "x?????x?????x";
	unsigned char	pattern[]		= "\x3B\x00\x00\x00\x00\x00\x0F\x00\x00\x00\x00\x00\xE8";
	size_t			BytesOffset		= 13;

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found [0]\n", funcname);
		m_bSuccess = false;
		return;
	}

	pAdress = ParseFunc(pAdress, pAdress + 300, funcname, pattern, mask, BytesOffset);

	if (!pAdress)
	{
		m_bSuccess = false;
		return;
	}

#endif

	m_pClearMultiDamage = (void*)pAdress;
}

void CMemory::Parse_ApplyMultiDamage(void)
{
	char funcname[] = "ApplyMultiDamage (gamedll)";

#ifdef __linux__

	size_t pAdress	= (size_t)FindFunction(&m_GameDllModule, "ApplyMultiDamage__FP9entvars_sT0");

	if (!pAdress)
	{
		pAdress	= (size_t)FindFunction(&m_GameDllModule, "_Z16ApplyMultiDamageP9entvars_sS0_");
	}

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found\n", funcname);
		m_bSuccess = false;
		return;
	}

	WPNMOD_LOG_ONLY("   Found \"%s\" at %p\n", funcname, pAdress);

#else

	size_t pAdress	= (size_t)FindFunction(&m_GameDllModule, "?SuperBounceTouch@CSqueakGrenade@@AAEXPAVCBaseEntity@@@Z");

	char				mask[]				= "xxx????x";
	unsigned char		pattern[]			= "\x50\x50\xE8\x00\x00\x00\x00\x8B";
	size_t				BytesOffset			= 3;

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found [0]\n", funcname);
		m_bSuccess = false;
		return;
	}

	pAdress = ParseFunc(pAdress, pAdress + 700, funcname, pattern, mask, BytesOffset);
	
	if (!pAdress)
	{
		m_bSuccess = false;
		return;
	}

#endif

	m_pApplyMultiDamage = (void*)pAdress;
}

void CMemory::Parse_PrecacheOtherWeapon(void)
{
	char funcname[] = "UTIL_PrecacheOtherWeapon (gamedll)";

#ifdef __linux__

	size_t pAdress	= (size_t)FindFunction(&m_GameDllModule, "UTIL_PrecacheOtherWeapon__FPCc");

	if (!pAdress)
	{
		pAdress	= (size_t)FindFunction(&m_GameDllModule, "_Z24UTIL_PrecacheOtherWeaponPKc");
	}

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found\n", funcname);
		m_bSuccess = false;
		return;
	}

	WPNMOD_LOG_ONLY("   Found \"%s\" at %p\n", funcname, pAdress);

#else

	char			string[]			= "weapon_rpg";
	char			mask[]				= "xxxxxx";
	unsigned char	pattern[]			= "\x68\x00\x00\x00\x00\xE8";
	size_t			BytesOffset			= 6;

	size_t pAdress = ParseFunc(m_start_gamedll, m_end_gamedll, funcname, string, pattern, mask, BytesOffset);

	if (!pAdress)
	{
		m_bSuccess = false;
		return;
	}

#endif

	g_fh_PrecacheOtherWeapon.address = (void*)pAdress;

	if (!CreateFunctionHook(&g_fh_PrecacheOtherWeapon))
	{
		WPNMOD_LOG("   Error: failed to hook \"%s\"\n", funcname);
		m_bSuccess = false;
		return;
	}

	SetHook(&g_fh_PrecacheOtherWeapon);
}

void CMemory::Parse_GetAmmoIndex(void)
{
	char funcname[] = "CBasePlayer::GetAmmoIndex (gamedll)";

#ifdef __linux__

	size_t pAdress = (size_t)FindFunction(&m_GameDllModule, "GetAmmoIndex__11CBasePlayerPCc");

	if (!pAdress)
	{
		pAdress = (size_t)FindFunction(&m_GameDllModule, "_ZN11CBasePlayer12GetAmmoIndexEPKc");
	}

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found\n", funcname);
		m_bSuccess = false;
		return;
	}

	WPNMOD_LOG_ONLY("   Found \"%s\" at %p\n", funcname, pAdress);

#else

	char			string[]			= "357";
	char			mask[]				= "xxxxxxx?x";
	unsigned char	pattern[]			= "\x68\x00\x00\x00\x00\x89\x46\x00\xE8";
	size_t			BytesOffset			= 9;

	size_t pAdress = ParseFunc(m_start_gamedll, m_end_gamedll, funcname, string, pattern, mask, BytesOffset);

	if (!pAdress)
	{
		m_bSuccess = false;
		return;
	}

#endif

	m_pGetAmmoIndex = (void*)pAdress;
}

void CMemory::Parse_GiveNamedItem(void)
{
	char funcname[] = "CBasePlayer::GiveNamedItem (gamedll)";

#ifdef __linux__

	size_t pAdress	= (size_t)FindFunction(&m_GameDllModule, "GiveNamedItem__11CBasePlayerPCc");

	if (!pAdress)
	{
		pAdress	= (size_t)FindFunction(&m_GameDllModule, "_ZN11CBasePlayer13GiveNamedItemEPKc");
	}

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found\n", funcname);
		m_bSuccess = false;
		return;
	}

	WPNMOD_LOG_ONLY("   Found \"%s\" at %p\n", funcname, pAdress);

#else

	char			string[]		= "weapon_crowbar";
	char			mask[]			= "xxxxxx?x";
	unsigned char	pattern[]		= "\x68\x00\x00\x00\x00\x8B\x00\xE8";
	size_t			BytesOffset		= 8;

	size_t pAdress = ParseFunc(m_start_gamedll, m_end_gamedll, funcname, string, pattern, mask, BytesOffset);

	if (!pAdress)
	{
		m_bSuccess = false;
		return;
	}

#endif

	g_fh_GiveNamedItem.address = (void*)pAdress;

	if (!CreateFunctionHook(&g_fh_GiveNamedItem))
	{
		WPNMOD_LOG("   Error: failed to hook \"%s\"\n", funcname);
		m_bSuccess = false;
		return;
	}

	SetHook(&g_fh_GiveNamedItem);
}

void CMemory::Parse_SetAnimation(void)
{
	char funcname[] = "CBasePlayer::SetAnimation (gamedll)";

#ifdef __linux__

	size_t pAdress	= (size_t)FindFunction(&m_GameDllModule, "SetAnimation__11CBasePlayer11PLAYER_ANIM");

	if (!pAdress)
	{
		pAdress	= (size_t)FindFunction(&m_GameDllModule, "_ZN11CBasePlayer12SetAnimationE11PLAYER_ANIM");
	}

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found\n", funcname);
		m_bSuccess = false;
		return;
	}

	WPNMOD_LOG_ONLY("   Found \"%s\" at %p\n", funcname, pAdress);

#else

	char			string[]		= "models/v_satchel_radio.mdl";
	char			mask[]			= "xxxxx";
	unsigned char	pattern[]		= "\xB9\x00\x00\x00\x00";

	char			mask2[]			= "xx?xxx";
	unsigned char	pattern2[]		= "\x8B\x4E\x00\x6A\x05\xE8";
	size_t			BytesOffset		= 6;

	int count = 0;

	size_t pAdress = NULL;
	size_t pCurrent = NULL;
	size_t pCandidate = NULL;

	pCurrent = FindStringInDLL(m_start_gamedll, m_end_gamedll, string);

	while (pCurrent)
	{
		*(size_t*)(pattern + 1) = (size_t)pCurrent;

		if ((pCandidate = FindAdressInDLL(m_start_gamedll, m_end_gamedll, pattern, mask)) != NULL)
		{
			count++;
			pAdress = pCandidate;
		}

		pCurrent = FindStringInDLL(pCurrent + 1, m_end_gamedll, string);
	}

	if (!count)
	{
		WPNMOD_LOG("   Error: \"%s\" not found [0]\n", funcname);
		m_bSuccess = false;
		return;
	}
	else if (count > 1)
	{
		WPNMOD_LOG("   Error: \"%s\" not found [1]\n", funcname);
		m_bSuccess = false;
		return;
	}

	pAdress = ParseFunc(pAdress, pAdress + 150, funcname, pattern2, mask2, BytesOffset);
	
	if (!pAdress)
	{
		m_bSuccess = false;
		return;
	}

#endif

	m_pPlayerSetAnimation = (void*)pAdress;
}

void CMemory::Parse_SubRemove(void)
{
	char funcname[] = "CBaseEntity::SUB_Remove (gamedll)";

#ifdef WIN32

	void* pAdress = (void*)FindFunction(&m_GameDllModule, "?SUB_Remove@CBaseEntity@@QAEXXZ");

#else

	void* pAdress = (void*)FindFunction(&m_GameDllModule, "SUB_Remove__11CBaseEntity");

	if (!pAdress)
	{
		pAdress = (void*)FindFunction(&m_GameDllModule, "_ZN11CBaseEntity10SUB_RemoveEv");
	}

#endif

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found\n", funcname);
		m_bSuccess = false;
		return;
	}

	m_pSubRemove = pAdress;

	WPNMOD_LOG_ONLY("   Found \"%s\" at %p\n", funcname, pAdress);
}

void CMemory::Parse_FallThink(void)
{
	char funcname[] = "CBasePlayerItem::FallThink (gamedll)";

#ifdef WIN32

	void* pAdress = (void*)FindFunction(&m_GameDllModule, "?FallThink@CBasePlayerItem@@QAEXXZ");

#else

	void* pAdress = (void*)FindFunction(&m_GameDllModule, "FallThink__15CBasePlayerItem");

	if (!pAdress)
	{
		pAdress = (void*)FindFunction(&m_GameDllModule, "_ZN15CBasePlayerItem9FallThinkEv");
	}

#endif

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found\n", funcname);
		m_bSuccess = false;
		return;
	}

	WPNMOD_LOG_ONLY("   Found \"%s\" at %p\n", funcname, pAdress);

	g_fh_FallThink.address = (void*)pAdress;

	if (!CreateFunctionHook(&g_fh_FallThink))
	{
		WPNMOD_LOG("   Error: failed to hook \"%s\"\n", funcname);
		m_bSuccess = false;
		return;
	}

	SetHook(&g_fh_FallThink);
}

void CMemory::Parse_AmmoSpawn(void)
{
	char* funcname = "CBasePlayerAmmo::Spawn (gamedll)";

#ifdef __linux__

	size_t pAdress = (size_t)FindFunction(&m_GameDllModule, "Spawn__15CBasePlayerAmmo");

	if (!pAdress)
	{
		pAdress = (size_t)FindFunction(&m_GameDllModule, "_ZN15CBasePlayerAmmo5SpawnEv");
	}

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found\n", funcname);
		m_bSuccess = false;
		return;
	}

#else

	int count = 0;

	size_t pAdress = NULL;
	size_t pCurrent = NULL;
	size_t pCandidate = NULL;

	//
	// 68 10 88 0C 10	push offset aModelsW_argren ; "models/w_ARgrenade.mdl"
	//

	char			string[] = "models/w_ARgrenade.mdl";
	char			mask[] = "xxxxxx";
	unsigned char	pattern[] = "\x04\x68\x00\x00\x00\x00";

	pCurrent = FindStringInDLL(m_start_gamedll, m_end_gamedll, string);

	while (pCurrent)
	{
		*(size_t*)(pattern + 2) = (size_t)pCurrent;

		if ((pCandidate = FindAdressInDLL(m_start_gamedll, m_end_gamedll, pattern, mask)))
		{
			count++;
			pAdress = pCandidate;
		}

		pCurrent = FindStringInDLL(pCurrent + 1, m_end_gamedll, string);
	}

	if (!count)
	{
		WPNMOD_LOG("   Error: \"%s\" not found [0]\n", funcname);
		m_bSuccess = false;
		return;
	}
	else if (count > 1)
	{
		WPNMOD_LOG("   Error: \"%s\" not found [1]\n", funcname);
		m_bSuccess = false;
		return;
	}

	// Find first call.
	size_t end = pAdress + 32;
	unsigned char opcode[] = "\xE9";

	pAdress = FindAdressInDLL(pAdress, end, opcode, "x");

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found [2]\n", funcname);
		m_bSuccess = false;
		return;
	}

	pAdress += 1;
	pAdress = *(size_t*)pAdress + pAdress + 4;

	WPNMOD_LOG_ONLY("   Found \"%s\" at %p\n", funcname, pAdress);

#endif

	g_fh_AmmoSpawn.address = (void*)pAdress;

	if (!CreateFunctionHook(&g_fh_AmmoSpawn))
	{
		WPNMOD_LOG("   Error: failed to hook \"%s\"\n", funcname);
		m_bSuccess = false;
		return;
	}

	SetHook(&g_fh_AmmoSpawn);
}

void CMemory::Parse_ItemSpawn(void)
{
	char* funcname = "CItem::Spawn (gamedll)";

#ifdef __linux__

	size_t pAdress = (size_t)FindFunction(&m_GameDllModule, "Spawn__5CItem");

	if (!pAdress)
	{
		pAdress = (size_t)FindFunction(&m_GameDllModule, "_ZN5CItem5SpawnEv");
	}

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found\n", funcname);
		m_bSuccess = false;
		return;
	}

#else

	int count = 0;

	size_t pAdress = NULL;
	size_t pCurrent = NULL;
	size_t pCandidate = NULL;

	//
	// 8B 46 04				mov eax, [esi+4]
	// 68 A4 62 0C 10		push offset aModelsW_batter; "models/w_battery.mdl"
	//

	char			string[] = "models/w_battery.mdl";
	char			mask[] = "xxxxxx";
	unsigned char	pattern[] = "\x04\x68\x00\x00\x00\x00";

	pCurrent = FindStringInDLL(m_start_gamedll, m_end_gamedll, string);

	while (pCurrent)
	{
		*(size_t*)(pattern + 2) = (size_t)pCurrent;

		if ((pCandidate = FindAdressInDLL(m_start_gamedll, m_end_gamedll, pattern, mask)))
		{
			count++;
			pAdress = pCandidate;
		}

		pCurrent = FindStringInDLL(pCurrent + 1, m_end_gamedll, string);
	}

	if (!count)
	{
		WPNMOD_LOG("   Error: \"%s\" not found [0]\n", funcname);
		m_bSuccess = false;
		return;
	}
	else if (count > 1)
	{
		WPNMOD_LOG("   Error: \"%s\" not found [1]\n", funcname);
		m_bSuccess = false;
		return;
	}

	// Find first call.
	size_t end = pAdress + 32;
	unsigned char opcode[] = "\xE9";

	pAdress = FindAdressInDLL(pAdress, end, opcode, "x");

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found [2]\n", funcname);
		m_bSuccess = false;
		return;
	}

	pAdress += 1;
	pAdress = *(size_t*)pAdress + pAdress + 4;

	WPNMOD_LOG_ONLY("   Found \"%s\" at %p\n", funcname, pAdress);

#endif
	
	g_fh_ItemSpawn.address = (void*)pAdress;

	if (!CreateFunctionHook(&g_fh_ItemSpawn))
	{
		WPNMOD_LOG("   Error: failed to hook \"%s\"\n", funcname);
		m_bSuccess = false;
		return;
	}

	SetHook(&g_fh_ItemSpawn);
}

void CMemory::Parse_GameRules(void)
{
#ifdef __linux__

	void* pAdress = FindFunction(&m_GameDllModule, "g_pGameRules");

	WPNMOD_LOG("   Found \"g_pGameRules\" at %p\n", pAdress);

	#define ObjectVTableOffsetBase	0x0
	#define GET_VTABLE_OBJECT(e) (*((void***)(((char*)e) + ObjectVTableOffsetBase)))

	typedef const char* (*FuncGetGameDescription) (void*);
	const char* GameDescription = reinterpret_cast<FuncGetGameDescription>(GET_VTABLE_OBJECT(pAdress)[12])(pAdress);

	printf2(" *GameDescription is \"%s\"\n", GameDescription);

#else

	char* funcname = "g_pGameRules (gamedll)";

	int count = 0;

	size_t pAdress = NULL;
	size_t pCurrent = NULL;
	size_t pCandidate = NULL;

	char			string[]		= "items/smallmedkit1.wav";
	char			mask[]			= "xxxxxx";
	unsigned char	pattern[]		= "\x68\x00\x00\x00\x00\x6A";

	pCurrent = FindStringInDLL(m_start_gamedll, m_end_gamedll, string);

	while (pCurrent)
	{
		*(size_t*)(pattern + 1) = (size_t)pCurrent;

		if ((pCandidate = FindAdressInDLL(m_start_gamedll, m_end_gamedll, pattern, mask)))
		{
			count++;
			pAdress = pCandidate;
		}

		pCurrent = FindStringInDLL(pCurrent + 1, m_end_gamedll, string);
	}

	if (!count)
	{
		WPNMOD_LOG("   Error: \"%s\" not found [0]\n", funcname);
		return;
	}
	else if (count > 1)
	{
		WPNMOD_LOG("   Error: \"%s\" not found [1]\n", funcname);
		return;
	}

	count = 0;

	size_t end = pAdress + 30;
	unsigned char opcode[] = "\x8B\x0D";

	pCurrent = FindAdressInDLL(pAdress, end, opcode, "xx");

	// Find first mov.
	while (pCurrent && count != 1)
	{
		count++;
		pAdress = pCurrent;
		pCurrent = FindAdressInDLL(pCurrent + 1, end, opcode, "xx");
	}

	if (count != 1)
	{
		WPNMOD_LOG("Error: \"%s\" not found [2]\n", funcname);
		return;
	}

	pAdress += 2;

	void* pGameRules = (void*) **((size_t **)pAdress);

	WPNMOD_LOG("   Found \"%s\" at %p %p\n", funcname, pAdress, pGameRules);


	#define ObjectVTableOffsetBase	0x0
	#define GET_VTABLE_OBJECT(e) (*((void***)(((char*)e) + ObjectVTableOffsetBase)))

	typedef const char* (__fastcall *FuncGetGameDescription) (void*, DUMMY);
	const char* GameDescription = reinterpret_cast<FuncGetGameDescription>(GET_VTABLE_OBJECT(pGameRules)[10])(pGameRules, DUMMY_VAL);

	printf2(" *GameDescription is \"%s\"\n", GameDescription);
#endif
}

void CMemory::Parse_GetDispatch(void)
{
	char* funcname = "GetDispatch (engine)";

#ifdef __linux__

	size_t pAdress = (size_t)FindFunction(&m_EngineModule, "GetDispatch");

	if (!pAdress)
	{
		//pAdress = (size_t)FindFunction(&m_GameDllModule, "");
	}

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found\n", funcname);
		m_bSuccess = false;
		return;
	}

#else

	int count = 0;

	size_t pAdress = NULL;
	size_t pCurrent = NULL;
	size_t pCandidate = NULL;

	//
	// 68 30 B1 E4 01		push	offset aGetentityapi2; "GetEntityAPI2"
	// E8 09 F7 FF FF		call	GetDispatch
	//

	char			string[] = "GetEntityAPI2";
	char			mask[] = "xxxxx";
	unsigned char	pattern[] = "\x68\x00\x00\x00\x00";

	pCurrent = FindStringInDLL(m_start_engine, m_end_engine, string);

	while (pCurrent)
	{
		*(size_t*)(pattern + 1) = (size_t)pCurrent;

		if ((pCandidate = FindAdressInDLL(m_start_engine, m_end_engine, pattern, mask)))
		{
			count++;
			pAdress = pCandidate;
		}

		pCurrent = FindStringInDLL(pCurrent + 1, m_end_engine, string);
	}

	if (!count)
	{
		WPNMOD_LOG("   Error: \"%s\" not found [0]\n", funcname);
		m_bSuccess = false;
		return;
	}
	else if (count > 1)
	{
		WPNMOD_LOG("   Error: \"%s\" not found [1]\n", funcname);
		m_bSuccess = false;
		return;
	}
	
	// Find first call.
	size_t end = pAdress + 10;
	unsigned char opcode[] = "\xE8";

	pAdress = FindAdressInDLL(pAdress, end, opcode, "x");

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found [2]\n", funcname);
		m_bSuccess = false;
		return;
	}

	pAdress += 1;
	pAdress = *(size_t*)pAdress + pAdress + 4;

	WPNMOD_LOG_ONLY("   Found \"%s\" at %p\n", funcname, pAdress);

#endif
	
	g_fh_GetDispatch.address = (void*)pAdress;

	if (!CreateFunctionHook(&g_fh_GetDispatch))
	{
		WPNMOD_LOG("   Error: failed to hook \"%s\"\n", funcname);
		m_bSuccess = false;
		return;
	}

	SetHook(&g_fh_GetDispatch);
}

void CMemory::Parse_CallGameEntity(void)
{
	char* funcname = "mutil_CallGameEntity (metamod)";

	size_t pAdress = (size_t)gpMetaUtilFuncs->pfnCallGameEntity;

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found\n", funcname);
		m_bSuccess = false;
		return;
	}

	WPNMOD_LOG_ONLY("   Found \"%s\" at %p\n", funcname, pAdress);

	g_fh_CallGameEntity.address = (void*)pAdress;

	if (!CreateFunctionHook(&g_fh_CallGameEntity))
	{
		WPNMOD_LOG("   Error: failed to hook \"%s\"\n", funcname);
		m_bSuccess = false;
		return;
	}

	SetHook(&g_fh_CallGameEntity);
}

void CMemory::EnableShieldHitboxTracing(void)
{
	bool bShieldRegistered = false;

	for (int i = 1; i <= g_iWeaponsCount; i++)
	{
		if (WeaponInfoArray[i].iType == Wpn_Custom && !stricmp(GetWeapon_pszName(i), "weapon_shield"))
		{
			bShieldRegistered = true;
			break;
		}
	}

	if (!bShieldRegistered)
	{
		return;
	}

#ifdef __linux__

	void* pAdress = FindFunction(&m_EngineModule, "g_bIsCStrike");

#else

	signature sig =
	{
		"\xC3\xE8\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x85\xC0\x75\x00\xA1",
		"xx????x????xxx?x", 16
	};

	size_t pAdress = (size_t)FindFunction(&m_EngineModule, sig);

	if (pAdress)
	{
		pAdress += 7;
	}

#endif 

	if (!pAdress)
	{
		WPNMOD_LOG("Warning: failed to enable hitbox tracing for \"weapon_shield\".\n");
	}
	else
	{

#ifdef __linux__

		*(int*)pAdress = 1;

#else

		*(int*)*(size_t*)(pAdress) = 1;

#endif

		WPNMOD_LOG_ONLY("Shield hitbox tracing enabled at %p\n", pAdress);
	}
}

void CMemory::EnableWeaponboxModels(void)
{
	//
	// Check for running amxx plugin, stop if exists.
	//

	char* plugin_amxx = "weaponbox_models.amxx";
	int iAmxxScript = MF_FindScriptByName(MF_BuildPathname("%s/%s", LOCALINFO((char*)"amxx_pluginsdir"), plugin_amxx));

	if (iAmxxScript != -1)
	{
		((CPlugin*)MF_GetScriptAmx(iAmxxScript)->userdata[UD_FINDPLUGIN])->status = PS_STOPPED;
		WPNMOD_LOG("Warning: amxx plugin \"%s\" stopped.\n", plugin_amxx);
	}

	//
	// Check for running meta plugin, unload if exists.
	//

#ifdef WIN32
	char* plugin_meta = "wpnbox_models_mm.dll";
#else
	char* plugin_meta = "wpnbox_models_mm_i386.so";
#endif

	void* pMetaPlugin = DLOPEN(plugin_meta);

	if (pMetaPlugin)
	{
		UNLOAD_PLUGIN_BY_HANDLE(PLID, pMetaPlugin, PT_NEVER, PNL_PLG_FORCED);
		WPNMOD_LOG("Warning: meta plugin \"%s\" unloaded.\n", plugin_meta);
	}

	//
	// Let's find adress of "CWeaponBox::PackWeapon" in game dll.
	//

	char* funcname = "CWeaponBox::PackWeapon (gamedll)";

#ifdef __linux__

	size_t pAdress = (size_t)FindFunction(&m_GameDllModule, "PackWeapon__10CWeaponBoxP15CBasePlayerItem");

	if (!pAdress)
	{
		pAdress = (size_t)FindFunction(&m_GameDllModule, "_ZN10CWeaponBox10PackWeaponEP15CBasePlayerItem");
	}

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found\n", funcname);
		return;
	}

	m_pWpnBoxKillThink = (void*)FindFunction(&m_GameDllModule, "Kill__10CWeaponBox");

	if (!m_pWpnBoxKillThink)
	{
		m_pWpnBoxKillThink = (void*)FindFunction(&m_GameDllModule, "_ZN10CWeaponBox4KillEv");
	}

#else

	int count = 0;

	size_t pAdress = NULL;
	size_t pCurrent = NULL;
	size_t pCandidate = NULL;

	//
	// 50					push eax
	// 68 EC 65 0C 10		push offset aWeaponbox_0 ; "weaponbox"
	//

	char			string[]		= "weaponbox";
	char			mask[]			= "xxxxxx";
	unsigned char	pattern[]		= "\x50\x68\x00\x00\x00\x00";

	pCurrent = FindStringInDLL(m_start_gamedll, m_end_gamedll, string);

	while (pCurrent)
	{
		*(size_t*)(pattern + 2) = (size_t)pCurrent;

		if ((pCandidate = FindAdressInDLL(m_start_gamedll, m_end_gamedll, pattern, mask)))
		{
			count++;
			pAdress = pCandidate;
		}

		pCurrent = FindStringInDLL(pCurrent + 1, m_end_gamedll, string);
	}

	if (!count)
	{
		WPNMOD_LOG("   Error: \"%s\" not found [0]\n", funcname);
		return;
	}
	else if (count > 1)
	{
		WPNMOD_LOG("   Error: \"%s\" not found [1]\n", funcname);
		return;
	}

	count = 0;

	//
	// E8 4A A3 FA FF		call	?Create@CBaseEntity@@SAPAV1@PADABVVector@@1PAUedict_s@@@Z
	// E8 DD DE 02 00		call	?PackAmmo@CWeaponBox@@QAEHHH@Z
	// E8 11 DF 02 00		call	?PackWeapon@CWeaponBox@@QAEHPAVCBasePlayerItem@@@Z
	//

	size_t end = pAdress + 300;
	unsigned char opcode[] = "\xE8";

	pCurrent = FindAdressInDLL(pAdress + 4, end, opcode, "x");

	// Find third call.
	while (pCurrent && count != 3)
	{
		count++;
		pAdress = pCurrent;
		pCurrent = FindAdressInDLL(pCurrent + 1, end, opcode, "x");
	}

	if (count != 3)
	{
		WPNMOD_LOG("   Error: \"%s\" not found [2]\n", funcname);
		return;
	}

	pAdress += 1;
	pAdress = *(size_t*)pAdress + pAdress + 4;

	m_pWpnBoxKillThink = (void*)FindFunction(&m_GameDllModule, "?Kill@CWeaponBox@@QAEXXZ");

#endif

	if (!m_pWpnBoxKillThink)
	{
		WPNMOD_LOG("   Error: \"%s\" not found [3]\n", funcname);
		return;
	}

	g_fh_funcPackWeapon.address = (void*)pAdress;

	if (!CreateFunctionHook(&g_fh_funcPackWeapon))
	{
		WPNMOD_LOG("   Error: failed to hook \"%s\"\n", funcname);
		return;
	}

	WPNMOD_LOG_ONLY("   Function \"%s\" successfully hooked at %p\n", funcname, pAdress);
	SetHook(&g_fh_funcPackWeapon);
}

char* CMemory::GetDllNameByModule(void* base)
{
	char lp[1536];

#ifdef __linux__

	Dl_info addrInfo;
	if (dladdr((void *)base, &addrInfo))
	{
		return (char *)addrInfo.dli_sname;
	}

#else

	GetModuleFileName((HMODULE)base, lp, 1536);

#endif

	char *ptr = lp;
	while (*ptr)
	{
		if (*ptr == ALT_SEP_CHAR)
		{
			*ptr = PATH_SEP_CHAR;
		}
		++ptr;
	}

	ptr = strrchr(lp, PATH_SEP_CHAR);
	return (ptr ? ptr + 1 : lp);
}

size_t CMemory::ParseFunc(size_t start, size_t end, char* funcname, unsigned char* pattern, char* mask, size_t bytes)
{
	int count = 0;

	size_t pAdress = NULL;
	size_t pCurrent = NULL;

	pCurrent = FindAdressInDLL(start, end, pattern, mask);

	while (pCurrent)
	{
		count++;
		pAdress = pCurrent;
		pCurrent = FindAdressInDLL(pCurrent + 1, end, pattern, mask);
	}

	if (!count)
	{
		WPNMOD_LOG("   Error: \"%s\" not found\n", funcname);
		return false;
	}
	else if (count > 1)
	{
		WPNMOD_LOG("   Error: %d candidates found for \"%s\"\n", count, funcname);
		return false;
	}

	pAdress += bytes;
	pAdress = *(size_t*)pAdress + pAdress + 4;

	WPNMOD_LOG_ONLY("   Found \"%s\" at %p\n", funcname, pAdress);

	return pAdress;
}

size_t CMemory::ParseFunc(size_t start, size_t end, char* funcname, char* string, unsigned char* pattern, char* mask, size_t bytes)
{
	int count = 0;

	size_t pAdress = NULL;
	size_t pCurrent = NULL;
	size_t pCandidate = NULL;
	
	pCurrent = FindStringInDLL(start, end, string);

	while (pCurrent)
	{
		*(size_t*)(pattern + 1) = (size_t)pCurrent;

		if ((pCandidate = FindAdressInDLL(start, end, pattern, mask)))
		{
			count++;
			pAdress = pCandidate;
		}

		pCurrent = FindStringInDLL(pCurrent + 1, end, string);
	}

	if (!count)
	{
		WPNMOD_LOG("   Error: \"%s\" not found\n", funcname);
		return false;
	}
	else if (count > 1)
	{
		WPNMOD_LOG("   Error: %d candidates found for \"%s\"\n", count, funcname);
		return false;
	}

	pAdress += bytes;
	pAdress = *(size_t*)pAdress + pAdress + 4;

	WPNMOD_LOG_ONLY("   Found \"%s\" at %p\n", funcname, pAdress);

	return pAdress;
}

