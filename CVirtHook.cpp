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

#include "weaponmod.h"


void *pDbase = NULL;

int VirtualFunction[VirtFunc_End];


void CVirtHook::SetHook(edict_t *pEdict, int iVirtFunc, int iTrampoline)
{
	Obj* a = head;

	while (a)
	{
		if (a->iVirtFunc == iVirtFunc)
		{
			return;
		}

		a = a->next;
	}

	a = new Obj;

	if (a == 0) 
		return;
	
#ifdef _WIN32
	DWORD OldFlags;

    void **vtable = *((void***)((char*)pEdict->pvPrivateData));
#elif __linux__
    void **vtable = *((void***)(((char*)pEdict->pvPrivateData) + 0x60));
#endif

    if (vtable == NULL)
	{
		delete a;
		print_srvconsole("[WEAPONMOD] Failed to retrieve vtable for \"%s\", hooks not active.\n", STRING(pEdict->v.classname));
        return;
	}

	int **ivtable = (int **)vtable;

	a->iVirtFunc = iVirtFunc;
	a->pOrigFunc = (void *)ivtable[VirtualFunction[iVirtFunc]];
	
#ifdef _WIN32
	VirtualProtect(&ivtable[VirtualFunction[iVirtFunc]], sizeof(int *), PAGE_READWRITE, &OldFlags);
#elif __linux__
	mprotect(&ivtable[VirtualFunction[iVirtFunc]], sizeof(int*), PROT_READ | PROT_WRITE);
#endif
	ivtable[VirtualFunction[iVirtFunc]] = (int *)iTrampoline;

	a->next = head;
	head = a;
}



void* CVirtHook::GetOrigFunc(int iVirtFunc)
{
	Obj* a = head;

	while ( a )
	{
		if (a->iVirtFunc == iVirtFunc)
		{
			return a->pOrigFunc;
		}
		a = a->next;
	}

	return NULL;
}



void CVirtHook::clear()
{
	while(head)
	{
		Obj* a = head->next;
		delete head;
		head = a;
	}
}