
#include "amxxmodule.h"
#include "virtual_hooker.h"
//#include "wpnmod_memory.h"
#include "wpnmod_utils.h"

int g_EntityVTableOffsetPev;
int g_EntityVTableOffsetBase;

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

	void** vtable = GET_VTABLE(pEdict);

	if (vtable == NULL)
	{
		return FALSE;
	}

	int** ivtable = (int**)vtable;

	int offset = g_vtblOffsets[hook->offset];

	if (!bRevert)
	{
		hook->address = (void*)ivtable[offset];
	}

	#ifdef __linux__

		void* alignedAddress = (void *)ALIGN(&ivtable[offset]);
		mprotect(alignedAddress, sysconf(_SC_PAGESIZE), PROT_READ | PROT_WRITE);

	#else

		FlushInstructionCache(GetCurrentProcess(), &ivtable[offset], sizeof(int*));

		static DWORD oldProtection;
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