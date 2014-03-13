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

#include "wpnmod_hooker.h"

#include <extdll.h>
#include <meta_api.h>


#if defined _WIN32
int FindModuleByAddr (void *addr, module *lib)
{
	MEMORY_BASIC_INFORMATION mem;
	VirtualQuery(addr, &mem, sizeof(mem));

	IMAGE_DOS_HEADER *dos = (IMAGE_DOS_HEADER*)mem.AllocationBase;
	IMAGE_NT_HEADERS *pe = (IMAGE_NT_HEADERS*)((unsigned long)dos+(unsigned long)dos->e_lfanew);

	if(pe->Signature != IMAGE_NT_SIGNATURE)
	{
		return FALSE;
	}

	lib->base = mem.AllocationBase;
	lib->size = (size_t)pe->OptionalHeader.SizeOfImage;
	lib->handler = lib->base;

	return TRUE;
}
#else
// Code derived from code from David Anderson
// You're going to help us, Mr. Anderson whether you want to or not. (c)
long getBaseLen(void *baseAddress)
{
	pid_t pid = getpid();
	char file[255];
	char buffer[2048];
	snprintf(file, sizeof(file)-1, "/proc/%d/maps", pid);
	FILE *fp = fopen(file, "rt");
	if (fp)
	{
		long length = 0;

		void *start = NULL;
		void *end = NULL;

		while (!feof(fp))
		{
			if (fgets(buffer, sizeof(buffer)-1, fp) == NULL)
				return 0;

			sscanf(buffer, "%lx-%lx", reinterpret_cast< long unsigned int * > (&start), reinterpret_cast< long unsigned int * > (&end));

			if(start == baseAddress)
			{
				length = (unsigned long)end  - (unsigned long)start;

				int value;

				while(!feof(fp))
				{
					if (fgets(buffer, sizeof(buffer)-1, fp) == NULL)
						return 0;

					sscanf
					(
						buffer, 
						"%lx-%lx %*s %*s %*s %d", 
						reinterpret_cast< long unsigned int * > (&start), 
						reinterpret_cast< long unsigned int * > (&end), 
						&value
					);

					if(!value)
					{		
						break;
					}
					else
					{
						length += (unsigned long)end  - (unsigned long)start;
					}
				}
				
				break;
			}
		}

		fclose(fp);

		return length;
	}

	return 0;
}

int FindModuleByAddr (void *addr, module *lib)
{
	if (!lib)
		return FALSE;

	Dl_info info;

	if (!dladdr(addr, &info) && !info.dli_fbase || !info.dli_fname)
	{
		return FALSE;
	}

	lib->base = info.dli_fbase;
	lib->size = (size_t)getBaseLen(lib->base);
	lib->handler = dlopen(info.dli_fname, RTLD_NOW);

	return TRUE;
}
#endif

void *FindFunction(module *lib, signature sig)
{
	if (!lib || !sig.text[0])
	{
		return NULL;
	}

	unsigned char *pBuff = (unsigned char *)lib->base;
	unsigned char *pEnd = (unsigned char *)lib->base+lib->size-sig.size;

	unsigned long i;

	while (pBuff < pEnd)
	{
		for (i = 0; i < sig.size; i++)
		{
			if ((sig.mask[i] != '?') && ((unsigned char)(sig.text[i]) != pBuff[i]))
				break;
		}

		if (i == sig.size)
		{
			return (void*)pBuff;
		}

		pBuff++;
	}
	
	return NULL;
}

size_t FindFunction(module *lib, const unsigned char *pattern, const char *mask)
{
	if (!lib)
	{
		return NULL;
	}

	size_t pattern_len = strlen(mask);

	unsigned char *pBuff = (unsigned char *)lib->base;
	unsigned char *pEnd = (unsigned char *)lib->base + lib->size - pattern_len;

	unsigned long i;

	while (pBuff < pEnd)
	{
		for (i = 0; i < pattern_len; i++)
		{
			if ((mask[i] != '?') && ((unsigned char)(pattern[i]) != pBuff[i]))
			{
				break;
			}
		}

		if (i == pattern_len)
		{
			return (size_t)(void*)pBuff;
		}

		pBuff++;
	}

	return NULL;
}

void *FindFunction(module *lib, const char *name)
{
	if (!lib)
	{
		return NULL;
	}

	return DLSYM((DLHANDLE)lib->handler, name);
}

void *FindFunction(function *func)
{
	if (!func)
	{
		return NULL;
	}

	void *address = NULL;
	if (NULL == (address = FindFunction(func->lib, func->name)))
	{
		return FindFunction(func->lib, func->sig);
	}

	return address;
}

bool SetHook(function *func)
{
	if (!AllowWriteToMemory(func->address))
	{
		return false;
	}

	memcpy(func->address, func->patch, 5);
	return true;
}

bool UnsetHook(function *func)
{
	if (!AllowWriteToMemory(func->address))
	{
		return false;
	}

	memcpy(func->address, func->origin, 5);
	return true;
}

int CreateFunctionHook(function *func)
{
	if (!func)
	{
		return 0;
	}

	if (!func->address)
	{
		func->address = (unsigned char*)FindFunction(func);
	}

	if (func->address && func->handler)
	{
		memcpy(func->origin, func->address, 5);
		
		func->patch[0]=0xE9;
		*(unsigned long *)&func->patch[1] = (unsigned long)func->handler-(unsigned long)func->address-5;
		
		return (func->done = TRUE);
	}

	return (func->done = FALSE);
}

bool AllowWriteToMemory(void *address)
{
#if defined _WIN32
	DWORD OldProtection, NewProtection = PAGE_EXECUTE_READWRITE;
	if (VirtualProtect(address, 5, NewProtection, &OldProtection))
#else
	void* alignedAddress = Align(address);
	if (!mprotect(alignedAddress, sysconf(_SC_PAGESIZE), (PROT_READ | PROT_WRITE | PROT_EXEC)))
#endif
		return true;
	return false;
}

size_t FindStringInDLL(size_t start, size_t end, const char *string)
{
	size_t pattern_len = strlen(string);

	unsigned char *current = (unsigned char*)(start);
	unsigned char *cend = (unsigned char*)(end - pattern_len);

	size_t i;

	while (current < cend)
	{
		for (i = 0; i < pattern_len; i++)
		{
			if (current[i] != string[i])
			{
				break;
			}
		}

		if (i == pattern_len)
		{
			return (size_t)(void*)current;
		}

		current++;
	}

	return NULL;
}

size_t FindAdressInDLL(size_t start, size_t end, unsigned char *pattern, char *mask)
{
	size_t pattern_len = strlen(mask);

	unsigned char *current = (unsigned char*)(start);
	unsigned char *cend = (unsigned char*)(end - pattern_len);

	size_t i = 0;

	while (current < cend)
	{
		for (i = 0; i < pattern_len; i++)
		{
			if ((mask[i] != '?') && ((unsigned char)(pattern[i]) != current[i]))
			{
				break;
			}
		}

		if (i == pattern_len)
		{
			return (size_t)(void*)current;
		}

		current++;
	}
	
	return NULL;
}

