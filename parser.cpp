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
#include "parser.h"
#include "utils.h"
#include "hooks.h"


char g_ConfigFilepath[1024];


BOOL ParseConfigSection(char *pSection, void *pHandler)
{
	BOOL bFound = FALSE;
	BOOL bResult = FALSE;
	FILE *stream = fopen(g_ConfigFilepath, "r");

	if (stream)
	{
		char data[2048];

		while (!feof(stream))
		{
			fgets(data, sizeof(data) - 1, stream);
			
			char *b = &data[0];

			trim_line(b);

			if (*b && *b != ';')
			{
				if (!_strcmpi(pSection, b))
				{
					bFound = TRUE;
					continue;
				}

				if (bFound)
				{
					if (*b == '[')
					{
						break;
					}

					bResult = TRUE;
					reinterpret_cast<void (*)(char*)>(pHandler)(b);
				}
			}
		}

		fclose(stream);
	}

	return bResult;
}

void ParseBlockItems_Handler(char* szBlockItem)
{
	CBlockItem *p = new CBlockItem;
	p->strName.assign(szBlockItem);

	if (strstr(szBlockItem, "weapon_"))
	{
		if (!_strcmpi(szBlockItem, "weapon_crowbar"))
		{
			g_BlockedItems.push_back(p);
			return;
		}

		p->VHook.offset = VOffset_AddToPlayer;
	}
	else if (strstr(szBlockItem, "ammo_"))
	{
		if (!_strcmpi(szBlockItem, "ammo_rpgclip"))
		{
			g_BlockedItems.push_back(p);
			return;
		}

		p->VHook.offset = VOffset_AddAmmo;
	}
	else
	{
		return;
	}

	p->VHook.handler = (void*)Item_Block;
	SetHookVirt(p->strName.c_str(), &p->VHook);

	g_BlockedItems.push_back(p);
}

void ParseSpawnPoints_Handler(char* data)
{		
	char* arg;
	char szData[3][32];

	int i, state;

	for (i = 0; i < 3; i++)
	{
		arg = parse_arg(&data, state);
		strcpy(szData[i], arg);
	}
				
	for (i = 1; i <= g_iWeaponsCount; i++)
	{
		if (WeaponInfoArray[i].iType == Wpn_Custom && !_stricmp(GetWeapon_pszName(i), szData[0]))
		{
			Weapon_Spawn(i, strlen(szData[1]) ? ParseVec(szData[1]) : Vector(0, 0, 0), strlen(szData[2])  ? ParseVec(szData[2]) : Vector(0, 0, 0));
			g_SpawnedWpns++;
		}
	}

	for (i = 0; i < g_iAmmoBoxIndex; i++)
	{
		if (!_stricmp(AmmoBoxInfoArray[i].classname.c_str(), szData[0]))
		{
			Ammo_Spawn(i, strlen(szData[1]) ? ParseVec(szData[1]) : Vector(0, 0, 0), strlen(szData[2]) ? ParseVec(szData[2]) : Vector(0, 0, 0));
			g_SpawnedAmmo++;
		}
	}
}

void SetConfigFile()
{
	MF_BuildPathnameR(g_ConfigFilepath, sizeof(g_ConfigFilepath) - 1, "%s/weaponmod/maps/%s.ini", get_localinfo("amxx_configsdir", "addons/amxmodx/configs"), STRING(gpGlobals->mapname));
	
	if (!FileExists(g_ConfigFilepath))
	{
		MF_BuildPathnameR(g_ConfigFilepath, sizeof(g_ConfigFilepath) - 1, "%s/weaponmod/weaponmod.ini", get_localinfo("amxx_configsdir", "addons/amxmodx/configs"));
	}
}

bool FileExists(const char *file)
{
#if defined WIN32 || defined _WIN32
	DWORD attr = GetFileAttributes(file);
	
	if (attr == INVALID_FILE_ATTRIBUTES)
		return 0;
	
	if (attr == FILE_ATTRIBUTE_DIRECTORY)
		return 0;
	
	return 1;
#else
	struct stat s;
	
	if (stat(file, &s) != 0)
		return 0;
	
	if (S_ISDIR(s.st_mode))
		return 0;
	
	return 1;
#endif
}

void trim_line(char *input)
{
	char *oldinput=input;
	char *start=input;

	while ( *start==' ' ||
			*start=='\t' ||
			*start=='\r' ||
			*start=='\n')
	{
		start++;
	}

	// Overwrite the whitespace

	if (start != input)
	{
		while ((*input++=*start++)!='\0')
			/* do nothing */ ;
	}

	start=oldinput;

	start+=strlen(start) - 1;

	while ( start >= oldinput &&
			( *start == '\0' ||
			  *start == ' '  ||
			  *start == '\r' ||
			  *start == '\n' ||
			  *start == '\t'))
	{
		start--;
	}
	start++;
	*start='\0';

	// Now find any comments and cut off at the start

	while (*start != '\0')
	{
		if (*start == ';')
		{
			*start='\0';
			break;
		}

		start++;
	}
}

char* parse_arg(char** line, int& state)
{
	static char arg[3072];
	char* dest = arg;
	state = 0;
	
	while (**line)
	{
		if (isspace(**line))
		{
			if (state == 1)
				break;
			else if (!state)
			{
				(*line)++;
				continue;
			}
		}
		else if (state != 2)
			state = 1;
		
		if (**line == '"')
		{
			(*line)++;
			
			if (state == 2)
				break;
			
			state = 2;
			continue;
		}
		
		*dest++ = *(*line)++;
	}
	
	*dest = '\0';
	return arg;
}
