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
	VirtFunc_TakeDamage,
	VirtFunc_BloodColor,
	VirtFunc_TraceBleed,
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

inline void TRACE_BLEED(edict_t* pEntity, float flDamage, Vector vecDir, TraceResult tr, int bitsDamageType)
{
	reinterpret_cast<int (__fastcall *)(void *, int, float, Vector, TraceResult *, int)>((*((void***)((char*)pEntity->pvPrivateData)))[VirtualFunction[VirtFunc_TraceBleed]])(pEntity->pvPrivateData, 0, flDamage, vecDir, &tr, bitsDamageType);
}

inline int BLOOD_COLOR(edict_t* pEntity)
{
	return reinterpret_cast<int (__fastcall *)(void *, int)>((*((void***)((char*)pEntity->pvPrivateData)))[VirtualFunction[VirtFunc_BloodColor]])(pEntity->pvPrivateData, 0);
}

inline void TAKE_DAMAGE(edict_t* pEntity, edict_t* pInflictor, edict_t* pAttacker, float flDamage, int bitsDamageType)
{
	reinterpret_cast<int (__fastcall *)(void *, int, entvars_t *, entvars_t *, float, int)>((*((void***)((char*)pEntity->pvPrivateData)))[VirtualFunction[VirtFunc_TakeDamage]])(pEntity->pvPrivateData, 0, &(pInflictor->v), &(pAttacker->v), flDamage, bitsDamageType);
}
#elif __linux__
inline int CLASSIFY(edict_t* pEntity)
{
	return reinterpret_cast<int (*)(void *)>((*((void***)(((char*)pEntity->pvPrivateData) + 0x60)))[VirtualFunction[VirtFunc_Classify]])(pEntity->pvPrivateData);
}

inline void TRACE_BLEED(edict_t* pEntity, float flDamage, Vector vecDir, TraceResult tr, int bitsDamageType)
{
	reinterpret_cast<int (*)(void *, float, Vector, TraceResult *, int)>((*((void***)(((char*)tr.pHit->pvPrivateData) + 0x60)))[VirtualFunction[VirtFunc_TraceBleed]])(tr.pHit->pvPrivateData, flDamage, vecDir, &tr, bitsDamageType);
}

inline int BLOOD_COLOR(edict_t* pEntity)
{
	return reinterpret_cast<int (*)(void *)>((*((void***)(((char*)pEntity->pvPrivateData) + 0x60)))[VirtualFunction[VirtFunc_BloodColor]])(pEntity->pvPrivateData);
}

inline void TAKE_DAMAGE(edict_t* pEntity, edict_t* pInflictor, edict_t* pAttacker, float flDamage, int bitsDamageType)
{
	reinterpret_cast<int (*)(void *, entvars_t *, entvars_t *, float, int)>((*((void***)(((char*)pEntity->pvPrivateData) + 0x60)))[VirtualFunction[VirtFunc_TakeDamage]])(pEntity->pvPrivateData, &(pInflictor->v), &(pAttacker->v), flDamage, bitsDamageType);
}
#endif
