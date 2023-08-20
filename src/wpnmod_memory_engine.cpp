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

#include <tier1/interface.h>
#include <rehlds/rehlds_api.h>
#include "wpnmod_memory.h"
#include "wpnmod_hooks.h"
#include "wpnmod_items.h"

void* WpnMod_GetDispatch(char* pname);

static DISPATCHFUNCTION ReHldsGetDispatch(IRehldsHook_GetDispatch* pHook, char* pszName)
{
	if (g_Config.IsInited())
	{
		auto pDispatch = (DISPATCHFUNCTION)WpnMod_GetDispatch(pszName);

		if (pDispatch)
			return pDispatch;
	}

	return pHook->callNext(pszName);
}

void CMemory::FindReHldsApi()
{
	CreateInterfaceFn pfnEngineFactory = Sys_GetFactory((CSysModule*)m_EngineModule.handler);
	if (!pfnEngineFactory)
		return;

	m_pRehldsApi = static_cast<IRehldsApi*>(pfnEngineFactory(VREHLDS_HLDS_API_VERSION, nullptr));
	if (!m_pRehldsApi)
		return;

	int major = m_pRehldsApi->GetMajorVersion();
	int minor = m_pRehldsApi->GetMinorVersion();
	WPNMOD_LOG("  Found ReHLDS API v%d.%d\n", m_pRehldsApi->GetMajorVersion(), m_pRehldsApi->GetMinorVersion());

	// Check version
	if (major != REHLDS_API_VERSION_MAJOR || minor < REHLDS_API_VERSION_MINOR)
	{
		WPNMOD_LOG("    Unsupported ReHLDS API version. Minimum: v%d.%d\n", REHLDS_API_VERSION_MAJOR, REHLDS_API_VERSION_MINOR);
		m_pRehldsApi = nullptr;
	}
}

void CMemory::UnsetReHldsHooks()
{
	if (m_pRehldsApi)
	{
		m_pRehldsApi->GetHookchains()->GetDispatch()->unregisterHook(ReHldsGetDispatch);
	}
}

void CMemory::Parse_GetDispatch(void)
{
	char* funcname = "GetDispatch (engine)";

	if (m_pRehldsApi)
	{
		// Use ReHLDS hookchain system
		m_pRehldsApi->GetHookchains()->GetDispatch()->registerHook(ReHldsGetDispatch);
		return;
	}

#ifdef __linux__

	size_t pAdress = FindAdressInDLL(&m_EngineModule, "GetDispatch");

	if (!pAdress)
	{
		//pAdress = FindAdressInDLL(&m_GameDllModule, "");
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

#endif

	WPNMOD_LOG_ONLY("   Found \"%s\" at %p\n", funcname, pAdress);

	g_fh_GetDispatch.address = (void*)pAdress;

	if (!CreateFunctionHook(&g_fh_GetDispatch))
	{
		WPNMOD_LOG("   Error: failed to hook \"%s\"\n", funcname);
		m_bSuccess = false;
		return;
	}

	SetHook(&g_fh_GetDispatch);
}

