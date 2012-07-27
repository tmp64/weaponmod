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

#include "amxxmodule.h"


class CVirtHook
{
	struct Obj 
	{
		int iVirtFunc;
		void *pOrigFunc;
		
		Obj* next;
	} *head;

public:
	CVirtHook() { head = 0; }
	~CVirtHook() { clear(); }

	void SetHook(edict_t *pEdict, int iVirtFunc, int iTrampoline);
	void* GetOrigFunc(int iVirtFunc);

	void clear();
};


typedef struct
{
	int iTrampoline;
	void *pOrigFunc;
} HookData;

enum e_VirtFuncs
{
	VirtFunc_Classify,
	VirtFunc_TraceAttack,
	VirtFunc_Think,
	VirtFunc_Touch,
	VirtFunc_Respawn,
	VirtFunc_AddToPlayer,
	VirtFunc_GetItemInfo,
	VirtFunc_CanDeploy,
	VirtFunc_Deploy,
	VirtFunc_CanHolster,
	VirtFunc_Holster,
	VirtFunc_ItemPostFrame,
	VirtFunc_Drop,
	VirtFunc_ItemSlot,
	VirtFunc_IsUseable,

	VirtFunc_End
};

extern int VirtualFunction[VirtFunc_End];

#ifdef _WIN32
inline int CLASSIFY(edict_t* pEntity)
{
	return reinterpret_cast<int (__fastcall *)(void *, int)>((*((void***)((char*)pEntity->pvPrivateData)))[VirtualFunction[VirtFunc_Classify]])(pEntity->pvPrivateData, 0);
}

inline void TRACE_ATTACK(edict_t* pEntity, edict_t* pAttacker, float flDamage, Vector vecDir, TraceResult tr, int bitsDamageType)
{
	reinterpret_cast<int (__fastcall *)(void *, int, entvars_t *, float, Vector, TraceResult *, int)>((*((void***)((char*)pEntity->pvPrivateData)))[VirtualFunction[VirtFunc_TraceAttack]])(pEntity->pvPrivateData, 0, &(pAttacker->v), flDamage, vecDir,  &tr, bitsDamageType);
}
#elif __linux__
inline int CLASSIFY(edict_t* pEntity)
{
	return reinterpret_cast<int (*)(void *)>((*((void***)(((char*)pEntity->pvPrivateData) + 0x60)))[VirtualFunction[VirtFunc_Classify]])(pEntity->pvPrivateData);
}

inline void TRACE_ATTACK(edict_t* pEntity, edict_t* pAttacker, float flDamage, Vector vecDir, TraceResult tr, int bitsDamageType)
{
	reinterpret_cast<int (*)(void *, int, entvars_t *, float, Vector, TraceResult *, int)>((*((void***)(((char*)pEntity->pvPrivateData) + 0x60)))[VirtualFunction[VirtFunc_TraceAttack]])(pEntity->pvPrivateData, 0, &(pAttacker->v), flDamage, vecDir,  &tr, bitsDamageType);
}
#endif
