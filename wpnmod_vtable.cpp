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

#include "wpnmod_vtable.h"
#include "wpnmod_config.h"


int g_EntityVTableOffsetPev = NULL;
int g_EntityVTableOffsetBase = NULL;

//
// Default vtbl offsets for Bugfixed and improved HL release.
//
GameOffset GameVirtualOffsets[VO_End] = 
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
	{128,	2}		// Player_PostThink
};

void Vtable_Init(void)
{
	// Bugfixed and improved HL release. Only set PEV and BASE offsets.
	if (g_GameMod == SUBMOD_AGHLRU)
	{
#ifdef _WIN32
		SetVTableOffsetPev(4);
		SetVTableOffsetBase(0x0);
#else
		SetVTableOffsetPev(0);
		SetVTableOffsetBase(0x60);
#endif 
	}
	else if (g_GameMod == SUBMOD_GEARBOX)
	{
		// More slots in OP4
		g_iMaxWeaponSlots = 7;
	}
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
		hook->done = HandleHookVirtual(hook, false);
	}
}

void UnsetHookVirtual(VirtualHookData* hook)
{
	if (hook && hook->done)
	{
		hook->done = HandleHookVirtual(hook, true);
	}
}

bool HandleHookVirtual(VirtualHookData* hook, bool bRevert)
{
	edict_t* pEdict = CREATE_ENTITY();

	CALL_GAME_ENTITY(PLID, hook->classname, &pEdict->v);

	if (pEdict->pvPrivateData == NULL)
	{
		REMOVE_ENTITY(pEdict);
		return FALSE;
	}

	void** vtable = GET_VTABLE_ENT(pEdict);

	if (vtable == NULL)
	{
		return FALSE;
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

		static DWORD oldProtection;

		FlushInstructionCache(GetCurrentProcess(), &ivtable[offset], sizeof(int*));
		VirtualProtect(&ivtable[offset], sizeof(int*), PAGE_READWRITE, &oldProtection);

	#endif

	if (bRevert)
	{
		ivtable[offset] = ( int* )hook->address;
	}
	else
	{
		ivtable[offset] = ( int* )hook->handler;
	}

	REMOVE_ENTITY( pEdict );
	return true;
}


