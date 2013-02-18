
#ifndef VIRTUAL_HOOKER_H
#define VIRTUAL_HOOKER_H

#include "amxxmodule.h"

struct VirtualHookData
{
	const char*	classname;

	int			offset;

	void*		handler;
	void*		address;
	int			done;
};

extern int g_EntityVTableOffsetPev;
extern int g_EntityVTableOffsetBase;



extern void SetVTableOffsetPev	(int iOffset);
extern void SetVTableOffsetBase	(int iOffset);

extern void SetHookVirtual		(VirtualHookData* hook);
extern void UnsetHookVirtual	(VirtualHookData* hook);
extern bool HandleHookVirtual	(VirtualHookData* hook, bool revert);

#ifdef _WIN32
	#define GET_VTABLE(e)	(*((void***)(((char*)e->pvPrivateData))))
#else
	#define ALIGN(ar)		((intptr_t)ar & ~(sysconf(_SC_PAGESIZE) - 1))
	#define GET_VTABLE(e)	(*((void***)(((char*)e->pvPrivateData) + g_EntityVTableOffsetBase)))
#endif

#endif  // VIRTUAL_HOOKER_H
