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



BOOL g_IsBaseSet = FALSE;

void *pRadiusDamage = NULL;
void *pGetAmmoIndex = NULL;
void *pPlayerSetAnimation = NULL;
void *pPrecacheOtherWeapon = NULL;

unsigned char* hldll_base;

size_t hldll_base_len;



void* FindFunction(char* sig_str, char* sig_mask, size_t sig_len)
{
	unsigned char* pBuff = hldll_base;
	unsigned char* pEnd = hldll_base + hldll_base_len;

	unsigned long i;
	while (pBuff < pEnd)
	{
		for (i = 0; i < sig_len; i++) 
		{
			if ((sig_mask[i] != '?') && ((unsigned char)(sig_str[i]) != pBuff[i]))
				break;
		}

		if (i == sig_len)
			return (void*)pBuff;

		pBuff++;
	}

    return NULL;
}



#ifdef _WIN32
BOOL FindDllBase(void* func)
{
    MEMORY_BASIC_INFORMATION mem;
    VirtualQuery(func, &mem, sizeof(mem));
 
    IMAGE_DOS_HEADER *dos = (IMAGE_DOS_HEADER*)mem.AllocationBase;
    IMAGE_NT_HEADERS *pe = (IMAGE_NT_HEADERS*)((unsigned long)dos+(unsigned long)dos->e_lfanew);
 
    if (pe->Signature != IMAGE_NT_SIGNATURE) 
	{
        hldll_base = (unsigned char*)NULL;
		hldll_base_len = NULL;

		return (g_IsBaseSet = FALSE);

	} 
	else 
	{
		hldll_base = (unsigned char*)mem.AllocationBase;
		hldll_base_len = (size_t)pe->OptionalHeader.SizeOfImage;

		return (g_IsBaseSet = TRUE);
	}
}
#elif __linux__
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

		void *start=NULL;
		void *end=NULL;

		while (!feof(fp))
		{
			fgets(buffer, sizeof(buffer)-1, fp);			
#if defined AMD64
			sscanf(buffer, "%Lx-%Lx", &start, &end);
#else
			sscanf(buffer, "%lx-%lx", &start, &end);
#endif
			if(start == baseAddress)
			{
				length = (unsigned long)end  - (unsigned long)start;

				char ignore[100];
				int value;

				while(!feof(fp))
				{
					fgets(buffer, sizeof(buffer)-1, fp);
#if defined AMD64
					sscanf(buffer, "%Lx-%Lx %s %s %s %d", &start, &end, ignore, ignore, ignore, &value);
#else
    				sscanf(buffer, "%lx-%lx %s %s %s %d", &start, &end, ignore, ignore ,ignore, &value);
#endif
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



int FindDllBase(void* func)
{
	Dl_info info;

	if(!dladdr(func, &info) && !info.dli_fbase || !info.dli_fname)
	{
		hldll_base = NULL;
		hldll_base_len = (size_t)NULL;
		
		ALERT(at_console, "[RCONfig]: Base search failed.\n");

		return (g_IsBaseSet = 0);
	} else {
		hldll_base = (unsigned char*)info.dli_fbase;
		hldll_base_len = getBaseLen(hldll_base);

		return (g_IsBaseSet = 1);
	}
}
#endif