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
	(void*)&Parse_ClearMultiDamage,
	(void*)&Parse_ApplyMultiDamage,
	(void*)&Parse_PrecacheOtherWeapon,
	(void*)&Parse_GetAmmoIndex,
	(void*)&Parse_GiveNamedItem,
	(void*)&Parse_SetAnimation,
	NULL,
};

#ifdef __linux__
	bool g_bNewGCC = false;
#endif

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

	if (!bSuccess)
	{
		for (int i = 0; i < Func_End; i++)
		{
			if (g_dllFuncs[i].done)
			{
				UnsetHook(&g_dllFuncs[i]);
			}
		}
	}

	return bSuccess;
}

// 
// void ClearMultiDamage(void)
//

bool Parse_ClearMultiDamage(size_t start, size_t end)
{
	char funcname[] = "ClearMultiDamage";

#ifdef __linux__

	size_t pAdress	= (size_t)FindFunction(&g_GameDllModule, "ClearMultiDamage__Fv");

	if (!pAdress)
	{
		g_bNewGCC = true;
		pAdress	= (size_t)FindFunction(&g_GameDllModule, "_Z16ClearMultiDamagev");
	}

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found\n", funcname);
		return false;
	}

	WPNMOD_LOG("   Found \"%s\" at %p\n", funcname, pAdress);

#else

	size_t pAdress	= (size_t)FindFunction(&g_GameDllModule, "?SuperBounceTouch@CSqueakGrenade@@AAEXPAVCBaseEntity@@@Z");

	char				mask[]				= "x?????x?????x";
	unsigned char		pattern[]			= "\x3B\x00\x00\x00\x00\x00\x0F\x00\x00\x00\x00\x00\xE8";
	size_t				BytesOffset			= 13;

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found [0]\n", funcname);
		return false;
	}

	pAdress = ParseFunc(pAdress, pAdress + 300, funcname, pattern, mask, BytesOffset);

	if (!pAdress)
	{
		return false;
	}

#endif

	g_dllFuncs[Func_ClearMultiDamage].address = (void*)pAdress;
	return true;
}

// 
// void ApplyMultiDamage(entvars_t *pevInflictor, entvars_t *pevAttacker)
//

bool Parse_ApplyMultiDamage(size_t start, size_t end)
{
	char funcname[] = "ApplyMultiDamage";

#ifdef __linux__

	size_t pAdress	= (size_t)FindFunction(&g_GameDllModule, "ApplyMultiDamage__FP9entvars_sT0");

	if (!pAdress)
	{
		pAdress	= (size_t)FindFunction(&g_GameDllModule, "_Z16ApplyMultiDamageP9entvars_sS0_");
	}

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found\n", funcname);
		return false;
	}

	WPNMOD_LOG("   Found \"%s\" at %p\n", funcname, pAdress);

#else

	size_t pAdress	= (size_t)FindFunction(&g_GameDllModule, "?SuperBounceTouch@CSqueakGrenade@@AAEXPAVCBaseEntity@@@Z");

	char				mask[]				= "xxx????x";
	unsigned char		pattern[]			= "\x50\x50\xE8\x00\x00\x00\x00\x8B";
	size_t				BytesOffset			= 3;

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found [0]\n", funcname);
		return false;
	}

	pAdress = ParseFunc(pAdress, pAdress + 700, funcname, pattern, mask, BytesOffset);
	
	if (!pAdress)
	{
		return false;
	}

#endif

	g_dllFuncs[Func_ApplyMultiDamage].address = (void*)pAdress;
	return true;
}

// 
// void UTIL_PrecacheOtherWeapon(const char *szClassname)
//

bool Parse_PrecacheOtherWeapon(size_t start, size_t end)
{
	char funcname[] = "UTIL_PrecacheOtherWeapon";

#ifdef __linux__

	size_t pAdress	= (size_t)FindFunction(&g_GameDllModule, "UTIL_PrecacheOtherWeapon__FPCc");

	if (!pAdress)
	{
		pAdress	= (size_t)FindFunction(&g_GameDllModule, "_Z24UTIL_PrecacheOtherWeaponPKc");
	}

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found\n", funcname);
		return false;
	}

	WPNMOD_LOG("   Found \"%s\" at %p\n", funcname, pAdress);

#else

	char			string[]			= "weapon_rpg";
	char			mask[]				= "xxxxxx";
	unsigned char	pattern[]			= "\x68\x00\x00\x00\x00\xE8";
	size_t			BytesOffset			= 6;

	size_t pAdress = ParseFunc(start, end, funcname, string, pattern, mask, BytesOffset);

	if (!pAdress)
	{
		return false;
	}

#endif

	g_dllFuncs[Func_PrecacheOtherWeapon].address = (void*)pAdress;
	
	if (!CreateFunctionHook(&g_dllFuncs[Func_PrecacheOtherWeapon]))
	{
		WPNMOD_LOG("   Error: failed to hook \"%s\"\n", funcname);
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
	char funcname[] = "CBasePlayer::GetAmmoIndex";

#ifdef __linux__

	size_t pAdress	= (size_t)FindFunction(&g_GameDllModule, "GetAmmoIndex__11CBasePlayerPCc");

	if (!pAdress)
	{
		pAdress	= (size_t)FindFunction(&g_GameDllModule, "_ZN11CBasePlayer12GetAmmoIndexEPKc");
	}

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found\n", funcname);
		return false;
	}

	WPNMOD_LOG("   Found \"%s\" at %p\n", funcname, pAdress);

#else

	char			string[]			= "357";
	char			mask[]				= "xxxxxxx?x";
	unsigned char	pattern[]			= "\x68\x00\x00\x00\x00\x89\x46\x00\xE8";
	size_t			BytesOffset			= 9;

	size_t pAdress = ParseFunc(start, end, funcname, string, pattern, mask, BytesOffset);

	if (!pAdress)
	{
		return false;
	}

#endif

	g_dllFuncs[Func_GetAmmoIndex].address = (void*)pAdress;
	return true;
}

// 
// void CBasePlayer::GiveNamedItem(const char *pszName)
//

bool Parse_GiveNamedItem(size_t start, size_t end)
{
	char funcname[] = "CBasePlayer::GiveNamedItem";

#ifdef __linux__

	size_t pAdress	= (size_t)FindFunction(&g_GameDllModule, "GiveNamedItem__11CBasePlayerPCc");

	if (!pAdress)
	{
		pAdress	= (size_t)FindFunction(&g_GameDllModule, "_ZN11CBasePlayer13GiveNamedItemEPKc");
	}

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found\n", funcname);
		return false;
	}

	WPNMOD_LOG("   Found \"%s\" at %p\n", funcname, pAdress);

#else

	char			string[]		= "weapon_crowbar";
	char			mask[]			= "xxxxxx?x";
	unsigned char	pattern[]		= "\x68\x00\x00\x00\x00\x8B\x00\xE8";
	size_t			BytesOffset		= 8;

	size_t pAdress = ParseFunc(start, end, funcname, string, pattern, mask, BytesOffset);

	if (!pAdress)
	{
		return false;
	}

#endif

	g_dllFuncs[Func_GiveNamedItem].address = (void*)pAdress;

	if (!CreateFunctionHook(&g_dllFuncs[Func_GiveNamedItem]))
	{
		WPNMOD_LOG("   Error: failed to hook \"%s\"\n", funcname);
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
	char funcname[] = "CBasePlayer::SetAnimation";

#ifdef __linux__

	size_t pAdress	= (size_t)FindFunction(&g_GameDllModule, "SetAnimation__11CBasePlayer11PLAYER_ANIM");

	if (!pAdress)
	{
		pAdress	= (size_t)FindFunction(&g_GameDllModule, "_ZN11CBasePlayer12SetAnimationE11PLAYER_ANIM");
	}

	if (!pAdress)
	{
		WPNMOD_LOG("   Error: \"%s\" not found\n", funcname);
		return false;
	}

	WPNMOD_LOG("   Found \"%s\" at %p\n", funcname, pAdress);

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

	pCurrent = FindStringInDLL(start, end, string);

	while (pCurrent)
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
		WPNMOD_LOG("   Error: \"%s\" not found [0]\n", funcname);
		return false;
	}
	else if (count > 1)
	{
		WPNMOD_LOG("   Error: %d candidates found for \"%s\"\n", count, funcname);
		return false;
	}

	pAdress = ParseFunc(pAdress, pAdress + 150, funcname, pattern2, mask2, BytesOffset);
	
	if (!pAdress)
	{
		return false;
	}

#endif

	g_dllFuncs[Func_PlayerSetAnimation].address = (void*)pAdress;
	return true;
}

#ifdef WIN32

size_t ParseFunc(size_t start, size_t end, char* funcname, unsigned char* pattern, char* mask, size_t bytes)
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

	WPNMOD_LOG("   Found \"%s\" at %p\n", funcname, pAdress);

	return pAdress;
}

size_t ParseFunc(size_t start, size_t end, char* funcname, char* string, unsigned char* pattern, char* mask, size_t bytes)
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

	WPNMOD_LOG("   Found \"%s\" at %p\n", funcname, pAdress);

	return pAdress;
}

#endif

void EnableShieldHitboxTracing(void)
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

	module hEngine = { NULL, NULL, NULL };

	if (!FindModuleByAddr((void*)g_engfuncs.pfnAlertMessage, &hEngine))
	{
		WPNMOD_LOG("Failed to locate engine, shield hitbox tracing not active.\n");
		return;
	}

#ifdef __linux__

	void* pAdress = FindFunction(&hEngine, "g_bIsCStrike");

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
		WPNMOD_LOG("Failed to enable shield hitbox tracing.\n");
	}
	else
	{
#ifdef __linux__

		*(int*)pAdress = 1;
#else
		*(int*)*(size_t*)(pAdress) = 1;
#endif
		WPNMOD_LOG("Shield hitbox tracing enabled at %p\n", pAdress);
	}
}

void EnableWeaponboxModels(void)
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

	char* funcname = "CWeaponBox::PackWeapon";

#ifdef __linux__

	size_t pAdress	= (size_t)FindFunction(&g_GameDllModule, "PackWeapon__10CWeaponBoxP15CBasePlayerItem");

	if (!pAdress)
	{
		pAdress	= (size_t)FindFunction(&g_GameDllModule, "_ZN10CWeaponBox10PackWeaponEP15CBasePlayerItem");
	}

	if (!pAdress)
	{
		WPNMOD_LOG("Error: \"%s\" not found\n", funcname);
		return;
	}

#else

	int count = 0;

	size_t pAdress = NULL;
	size_t pCurrent = NULL;
	size_t pCandidate = NULL;

	size_t start = (size_t)g_GameDllModule.base;
	size_t end = (size_t)g_GameDllModule.base + (size_t)g_GameDllModule.size;

	//
	// 50					push eax
	// 68 EC 65 0C 10		push offset aWeaponbox_0 ; "weaponbox"
	//

	char			string[]		= "weaponbox";
	char			mask[]			= "xxxxxx";
	unsigned char	pattern[]		= "\x50\x68\x00\x00\x00\x00";

	pCurrent = FindStringInDLL(start, end, string);

	while (pCurrent)
	{
		*(size_t*)(pattern + 2) = (size_t)pCurrent;

		if ((pCandidate = FindAdressInDLL(start, end, pattern, mask)) != NULL)
		{
			count++;
			pAdress = pCandidate;
		}

		pCurrent = FindStringInDLL(pCurrent + 1, end, string);
	}

	if (!count)
	{
		WPNMOD_LOG("Error: \"%s\" not found [0]\n", funcname);
		return;
	}
	else if (count > 1)
	{
		WPNMOD_LOG("Error: %d candidates found for \"%s\"\n", count, funcname);
		return;
	}

	count = 0;

	//
	// E8 4A A3 FA FF		call	?Create@CBaseEntity@@SAPAV1@PADABVVector@@1PAUedict_s@@@Z
	// E8 DD DE 02 00		call	?PackAmmo@CWeaponBox@@QAEHHH@Z
	// E8 11 DF 02 00		call	?PackWeapon@CWeaponBox@@QAEHPAVCBasePlayerItem@@@Z
	//

	unsigned char opcode[] = "\xE8";

	end = pAdress + 300;
	pCurrent = FindAdressInDLL(pAdress, end, opcode, "x");

	// Find third call.
	while (pCurrent && count != 3)
	{
		count++;
		pAdress = pCurrent;
		pCurrent = FindAdressInDLL(pCurrent + 1, end, opcode, "x");
	}

	if (count != 3)
	{
		WPNMOD_LOG("Error: \"%s\" not found [1] (count %d)\n", funcname, count);
		return;
	}

	pAdress += 1;
	pAdress = *(size_t*)pAdress + pAdress + 4;
#endif

	WPNMOD_LOG_ONLY("Found \"%s\" at %p\n", funcname, pAdress);

	g_funcPackWeapon.address = (void*)pAdress;

	if (!CreateFunctionHook(&g_funcPackWeapon))
	{
		WPNMOD_LOG("Error: failed to hook \"%s\"\n", funcname);
		return;
	}

	SetHook(&g_funcPackWeapon);
}

