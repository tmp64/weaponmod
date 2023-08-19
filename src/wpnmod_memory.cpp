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
#include "wpnmod_items.h"

CMemory g_Memory;

function g_fh_GiveNamedItem			= HOOK_FUNC(GiveNamedItem_HookHandler);
function g_fh_FallThink				= HOOK_FUNC(CBasePlayerItem_FallThink_HookHandler);
function g_fh_AmmoSpawn				= HOOK_FUNC(CBasePlayerAmmoSpawn_HookHandler);
function g_fh_ItemSpawn				= HOOK_FUNC(CItemSpawn_HookHandler);
function g_fh_GetDispatch			= HOOK_FUNC(GetDispatch_HookHandler);
function g_fh_CallGameEntity		= HOOK_FUNC(CallGameEntity_HookHandler);
function g_fh_WorldPrecache			= HOOK_FUNC(WorldPrecache_HookHandler);

CMemory::CMemory()
{
	m_bIsNewGCC = false;

	m_pSubRemove = NULL;
	m_pClearMultiDamage = NULL;
	m_pApplyMultiDamage = NULL;
	m_pPlayerSetAnimation = NULL;
	m_pWorldPrecache = NULL;
}

bool CMemory::Init(void)
{
	char moduleNameBuf[512];

	if (!FindModuleByAddr((void*)g_engfuncs.pfnAlertMessage, &m_EngineModule))
	{
		WPNMOD_LOG("  Failed to locate game engine!\n");
		return false;
	}

	if (!FindModuleByAddr((void*)gpMetaUtilFuncs->pfnGetGameInfo, &m_MetamodModule))
	{
		WPNMOD_LOG("  Failed to locate metamod!\n");
		return false;
	}

	if (!FindModuleByAddr((void*)MDLL_FUNC->pfnGetGameDescription(), &m_GameDllModule))
	{
		WPNMOD_LOG("  Failed to locate gamedll!\n");
		return false;
	}

	WPNMOD_LOG("  Found %s at %p\n", GetDllNameByModule(m_EngineModule.base, moduleNameBuf, sizeof(moduleNameBuf)), m_EngineModule.base);
	WPNMOD_LOG("  Found %s at %p\n", GetDllNameByModule(m_MetamodModule.base, moduleNameBuf, sizeof(moduleNameBuf)), m_MetamodModule.base);
	WPNMOD_LOG("  Found %s at %p\n", GET_GAME_INFO(PLID, GINFO_DLL_FILENAME), m_GameDllModule.base);

	m_start_gamedll = (size_t)m_GameDllModule.base;
	m_end_gamedll = (size_t)m_GameDllModule.base + (size_t)m_GameDllModule.size;

	m_start_engine = (size_t)m_EngineModule.base;
	m_end_engine = (size_t)m_EngineModule.base + (size_t)m_EngineModule.size;

	m_bSuccess = true;

	Parse_GetDispatch();
	Parse_CallGameEntity();
	Parse_WorldPrecache();
	Parse_InfoArrays();
	Parse_ClearMultiDamage();
	Parse_ApplyMultiDamage();
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
	UnsetHook(&g_fh_FallThink);
	UnsetHook(&g_fh_AmmoSpawn);
	UnsetHook(&g_fh_ItemSpawn);
	UnsetHook(&g_fh_GetDispatch);
	UnsetHook(&g_fh_CallGameEntity);
	UnsetHook(&g_fh_WorldPrecache);
}

size_t Parse_StringInModule(module *lib, unsigned char* pattern, const char *string)
{
	int i = 0;
	unsigned char *ptr = &pattern[0];
	while (*(ptr++))
	{
		//++ptr;
		++i;
	}

//	printf2("!!!!!!!!!!!!!!!! Parse_StringInModule  %d\n", i);

	return NULL;
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
	if (!WEAPON_GET_ID("weapon_shield"))
	{
		return;
	}

#ifdef __linux__

	void* pAdress = (void*)FindAdressInDLL(&m_EngineModule, "g_bIsCStrike");

#else

	size_t pAdress = FindAdressInDLL(m_start_engine, m_end_gamedll,
		(unsigned char *)"\xC3\xE8\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x85\xC0\x75\x00\xA1", "xx????x????xxx?x");

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

char* CMemory::GetDllNameByModule(void* base, char* lp, size_t bufSize)
{
#ifdef __linux__

	Dl_info addrInfo;
	if (dladdr((void *)base, &addrInfo))
	{
		return (char *)addrInfo.dli_sname;
	}

#else

	GetModuleFileName((HMODULE)base, lp, bufSize);

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

