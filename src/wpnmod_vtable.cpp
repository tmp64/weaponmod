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

#include <string>
#include "sdk/amxx_gameconfigs.h"
#include "wpnmod_vtable.h"
#include "wpnmod_memory.h"
#include "wpnmod_config.h"


int g_EntityVTableOffsetPev = NULL;
int g_EntityVTableOffsetBase = NULL;

//! List of VTable offsets.
TypeDescription GameVirtualOffsets[VO_End];

namespace
{

//! Method name which a VTable offset must be loaded.
struct OffsetInitializer
{
	const char* szClassName = nullptr;
	const char* szMethodName = nullptr;
	const char* szAmxxMethodName = nullptr;
};

//! List class and field name for each offset.
OffsetInitializer g_OffsetInitializers[VO_End];

} // namespace

void Vtable_Init(void)
{
	// CBaseEntity
	g_OffsetInitializers[VO_Spawn] =			{ "CBaseEntity",		"Spawn",		"spawn" };
	g_OffsetInitializers[VO_Precache] =			{ "CBaseEntity",		"Precache",		"precache" };
	g_OffsetInitializers[VO_Classify] =			{ "CBaseEntity",		"Classify",		"classify" };
	g_OffsetInitializers[VO_TraceAttack] =		{ "CBaseEntity",		"TraceAttack",	"traceattack" };
	g_OffsetInitializers[VO_TakeDamage] =		{ "CBaseEntity",		"TakeDamage",	"takedamage" };
	g_OffsetInitializers[VO_DamageDecal] =		{ "CBaseEntity",		"DamageDecal",	"damagedecal" };
	g_OffsetInitializers[VO_Respawn] =			{ "CBaseEntity",		"Respawn",		"respawn" };

	// CBasePlayer
	g_OffsetInitializers[VO_Player_PostThink] = { "CBasePlayer",		"PostThink",	"player_postthink" };

	// CBasePlayerItem
	g_OffsetInitializers[VO_AddToPlayer] =		{ "CBasePlayerItem",	"AddToPlayer",	"item_addtoplayer" };
	g_OffsetInitializers[VO_CanDeploy] =		{ "CBasePlayerItem",	"CanDeploy",	"item_candeploy" };
	g_OffsetInitializers[VO_Deploy] =			{ "CBasePlayerItem",	"Deploy",		"item_deploy" };
	g_OffsetInitializers[VO_CanHolster] =		{ "CBasePlayerItem",	"CanHolster",	"item_canholster" };
	g_OffsetInitializers[VO_Holster] =			{ "CBasePlayerItem",	"Holster",		"item_holster" };
	g_OffsetInitializers[VO_ItemSlot] =			{ "CBasePlayerItem",	"ItemSlot",		"item_itemslot" };

	// CBasePlayerWeapon
	g_OffsetInitializers[VO_ItemPostFrame] =	{ "CBasePlayerWeapon",	"ItemPostFrame" };
	g_OffsetInitializers[VO_IsUseable] =		{ "CBasePlayerWeapon",	"IsUseable" };

	// CBasePlayerAmmo
	g_OffsetInitializers[VO_AddAmmo] =			{ "CBasePlayerAmmo",	"AddAmmo" };

	// Load offsets
	IGameConfigPtr pWpnModCfg = WpnMod_LoadGameConfigFile("weaponmod.games");
	IGameConfigPtr pAmxxCfg = WpnMod_LoadGameConfigFile("common.games");
	bool anyNotFound = false;

	for (int i = 0; i < std::size(g_OffsetInitializers); i++)
	{
		const OffsetInitializer& init = g_OffsetInitializers[i];
		TypeDescription& offset = GameVirtualOffsets[i];
		bool isFound = false;

		// Try to load from WpnMod config
		isFound = pWpnModCfg->GetOffsetByClass(init.szClassName, init.szMethodName, &offset);

		if (!isFound)
		{
			// Try to load from AMXX config
			std::string amxxName;

			if (init.szAmxxMethodName)
			{
				amxxName = init.szAmxxMethodName;
			}
			else
			{
				// Convert name to lower case
				amxxName = init.szMethodName;
				std::for_each(amxxName.begin(), amxxName.end(), [](char& c) { c = tolower(c); });
			}

			isFound = pAmxxCfg->GetOffset(amxxName.c_str(), &offset);
		}

		if (!isFound)
		{
			WPNMOD_LOG("VTable Offset not found: %s::%s (amxx: %s)\n", init.szClassName, init.szMethodName, init.szAmxxMethodName);
			anyNotFound = true;
			continue;
		}
	}

	if (anyNotFound)
	{
		WPNMOD_LOG("Failed to find some VTable offsets. The server will now crash. Goodbye.\n");
		std::abort();
	}

	// Load pev offset
	IGameConfigPtr pEntityCfg = WpnMod_LoadGameConfigFile("common.games");
	TypeDescription pevOffset;

	if (!pEntityCfg->GetOffsetByClass("CBaseEntity", "pev", &pevOffset))
	{
		WPNMOD_LOG("CBaseEntity::pev offset not found\n");
		std::abort();
	}

	// Default PEV and BASE offsets.
	SetVTableOffsetPev(pevOffset.fieldOffset);
	SetVTableOffsetBase(0x0);
}

void SetVTableOffsetPev(int iOffset)
{
	g_EntityVTableOffsetPev = iOffset;
}

void SetVTableOffsetBase(int iOffset)
{
	g_EntityVTableOffsetBase = iOffset;
}

void SetHookVirtual(VirtualHookData* hook)
{
	if (hook)
	{
		HandleHookVirtual(hook, false);
	}
}

void UnsetHookVirtual(VirtualHookData* hook)
{
	if (hook && hook->done)
	{
		HandleHookVirtual(hook, true);
	}
}

bool HandleHookVirtual(VirtualHookData* hook, bool bRevert)
{
	edict_t* pEdict = CREATE_ENTITY();

	CALL_GAME_ENTITY(PLID, hook->classname, &pEdict->v);

	if (pEdict->pvPrivateData == NULL)
	{
		REMOVE_ENTITY(pEdict);
		return (hook->done = false);
	}

	void** vtable = GetEntityVTable(pEdict);

	if (vtable == NULL)
	{
		return (hook->done = false);
	}

	int** ivtable = (int**)vtable;

	int offset = GET_VTABLE_OFFSET(hook->offset);

	if (!bRevert)
	{
		hook->address = (void*)ivtable[offset];
	}

	#ifdef __linux__

		void* alignedAddress = (void *)ALIGN(&ivtable[offset]);
		mprotect(alignedAddress, sysconf(_SC_PAGESIZE), PROT_READ | PROT_WRITE);

	#else

		DWORD oldProtection;

		FlushInstructionCache(GetCurrentProcess(), &ivtable[offset], sizeof(int*));
		VirtualProtect(&ivtable[offset], sizeof(int*), PAGE_READWRITE, &oldProtection);

	#endif

	if (bRevert)
	{
		ivtable[offset] = (int*)hook->address;
	}
	else
	{
		ivtable[offset] = (int*)hook->handler;
	}

	REMOVE_ENTITY(pEdict);
	return (hook->done = true);
}

