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

#ifndef _VTABLE_H
#define _VTABLE_H

#include "amxxmodule.h"


#ifdef WIN32

	#define DUMMY_VAL 0
	#define GET_VTABLE_OFFSET(x) GameVirtualOffsets[x].iValue

	typedef int DUMMY;

#else

	#include <sys/types.h>
	#include <sys/stat.h>
	#include <sys/mman.h>
	#include <unistd.h>

	#define ALIGN(ar)				((intptr_t)ar & ~(sysconf(_SC_PAGESIZE) - 1))
	#define GET_VTABLE_OFFSET(x)	(GameVirtualOffsets[x].iValue + GameVirtualOffsets[x].iExtraOffset)

#endif

#define GET_VTABLE_ENT(e)	(*((void***)(((char*)e->pvPrivateData) + g_EntityVTableOffsetBase)))

enum VTableOffsets 
{
	VO_Spawn,
	VO_Precache,
	VO_Classify,
	VO_TraceAttack,
	VO_TakeDamage,
	VO_DamageDecal,
	VO_Respawn,
	VO_AddAmmo,
	VO_AddToPlayer,
	VO_CanDeploy,
	VO_Deploy,
	VO_CanHolster,
	VO_Holster,
	VO_ItemPostFrame,
	VO_ItemSlot,
	VO_IsUseable,
	VO_Player_PostThink,

	VO_End
};

struct GameOffset
{
	int	iValue;
	int	iExtraOffset;
};

struct VirtualHookData
{
	const char*	classname;

	int		offset;
	void*	handler;
	void*	address;
	bool		done;
};

extern GameOffset GameVirtualOffsets[VO_End];

extern int g_EntityVTableOffsetPev;
extern int g_EntityVTableOffsetBase;

extern void Vtable_Init			(void);

extern void SetVTableOffsetPev	(int iOffset);
extern void SetVTableOffsetBase	(int iOffset);

extern void SetHookVirtual		(VirtualHookData* hook);
extern void UnsetHookVirtual	(VirtualHookData* hook);
extern bool HandleHookVirtual	(VirtualHookData* hook, bool revert);

#endif  // _VTABLE_H
