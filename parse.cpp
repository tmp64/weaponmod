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

		while (!feof(stream) && fgets(data, sizeof(data) - 1, stream))
		{
			char *b = &data[0];
			trim_line(b);

			if (*b && *b != ';')
			{
				if (!_strcmpi(pSection, b))
				{
					bFound = TRUE;
					continue;
				}

				else if (bFound)
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
	VirtHookData *p = new VirtHookData;
	
	p->done = NULL;
	p->handler = NULL;
	p->address = NULL;
	p->classname = STRING(ALLOC_STRING(szBlockItem));

	if (!_strcmpi(szBlockItem, "weapon_crowbar") || !_strcmpi(szBlockItem, "ammo_rpgclip"))
	{
		g_BlockedItems.push_back(p);
		return;
	}

	if (strstr(szBlockItem, "weapon_"))
	{
		p->offset = VOffset_AddToPlayer;
	}
	else if (strstr(szBlockItem, "ammo_"))
	{
		p->offset = VOffset_AddAmmo;
	}
	else
	{
		delete p;
		return;
	}

	p->handler = (void*)Item_Block;
	SetHookVirt(p);

	g_BlockedItems.push_back(p);
}

void ParseSpawnPoints_Handler(char* data)
{		
	char* arg;
	char szData[3][32];

	int state;

	for (int i = 0; i < 3; i++)
	{
		arg = parse_arg(&data, state);
		strcpy(szData[i], arg);
	}

	if (Weapon_Spawn(szData[0], strlen(szData[1]) ? ParseVec(szData[1]) : Vector(0, 0, 0), strlen(szData[2])  ? ParseVec(szData[2]) : Vector(0, 0, 0)))
	{
		g_SpawnedWpns++;
	}
	else if (Ammo_Spawn(szData[0], strlen(szData[1]) ? ParseVec(szData[1]) : Vector(0, 0, 0), strlen(szData[2]) ? ParseVec(szData[2]) : Vector(0, 0, 0)))
	{
		g_SpawnedAmmo++;
	}
}

void ParseEquipment_Handler(char* data)
{
	if (g_EquipEnt == NULL)
	{
		const char* equip_classname = "game_player_equip";

		edict_t* pFind = FIND_ENTITY_BY_CLASSNAME(NULL, equip_classname);

		while (!FNullEnt(pFind))
		{
			pFind->v.flags |= FL_KILLME;
			pFind = FIND_ENTITY_BY_CLASSNAME(pFind, equip_classname);
		}

		pFind = CREATE_NAMED_ENTITY(MAKE_STRING(equip_classname));

		if (IsValidPev(pFind))
		{
			MDLL_Spawn(pFind);

			g_EquipEnt = CREATE_NAMED_ENTITY(MAKE_STRING(equip_classname));

			if (IsValidPev(g_EquipEnt))
			{
				g_EquipEnt->v.classname = MAKE_STRING("weaponmod_equipment");
				MDLL_Spawn(g_EquipEnt);
			}
		}
	}

	char* arg;
	int i, state;
	char szData[2][32];

	KeyValueData kvd;

	for (i = 0; i < 2; i++)
	{
		arg = parse_arg(&data, state);
		
		trim_line(arg);
		strcpy(szData[i], arg);
	}

	kvd.szClassName = (char*)STRING(g_EquipEnt->v.classname);
	kvd.szKeyName = szData[0];
	kvd.szValue = szData[1];
	kvd.fHandled = 0;

	MDLL_KeyValue(g_EquipEnt, &kvd);
}

void ParseAmmo_Handler(char* data)
{
	char* arg;
	int i, state;
	char szData[2][32];

	for (i = 0; i < 2; i++)
	{
		arg = parse_arg(&data, state);
		
		trim_line(arg);
		strcpy(szData[i], arg);
	}

	StartAmmo *p = new StartAmmo;

	p->ammoname = STRING(ALLOC_STRING(szData[0]));
	p->count = max(min(atoi(szData[1]), 254 ), 0);

	g_StartAmmo.push_back(p);
}

// Thanks to Eg@r4$il{ and HLSDK.
void KeyValueFromBSP(char *pKey, char *pValue, int iNewent)
{
	static vec_t AngleY;
	static Vector vecOrigin;

	if (iNewent)
	{
		AngleY = vecOrigin.x = vecOrigin.y = vecOrigin.z = 0;
	}

	if (!strcmp(pKey, "angle"))
	{
		AngleY = atoi(pValue);
	}

	if (!strcmp(pKey, "origin"))
	{
		vecOrigin = ParseVec(pValue);
	}

	if (!strcmp(pKey, "classname") && !Ammo_Spawn(pValue, vecOrigin, Vector (0, AngleY, 0)))
	{
		Weapon_Spawn(pValue, vecOrigin, Vector (0, AngleY, 0));
	}
}

void ParseBSP()
{
	FILE *fp;

	int tmp, size;
	char key[512];
	char value[512];
	bool newent = false;

	char filepath[1024];
	MF_BuildPathnameR(filepath, sizeof(filepath) - 1, "maps/%s.bsp", STRING(gpGlobals->mapname));

	fp = fopen(filepath, "rb");
	
	if (!fp)
	{
		return;
	}

	fread(&tmp, 4, 1, fp);

	if (tmp != 30)
	{
		return;
	}

	fread(&tmp, 4, 1, fp);
	fread(&size, 4, 1, fp);
	
	char *data = (char*)malloc(size);
	
	if (!data)
	{
		return;
	}

	fseek(fp, tmp, SEEK_SET);
	fread(data, size, 1, fp);
	fclose(fp);

	char token[2048];

	while(( data = COM_ParseFile(data, token)) != NULL )
	{
		if (strcmp(token, "{"))
		{
			return;
		}

		newent = true;

		while (true)
		{
			if (!( data = COM_ParseFile(data, token)))
			{
				return;
			}

			if (!strcmp(token, "}"))
			{
				break;
			}

			strcpy(key, token);
			data = COM_ParseFile(data, token);
			strcpy(value, token);

			KeyValueFromBSP(key, value, newent);
			newent = false;
		}
	}

	free(data);
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

Vector ParseVec(char *pString)
{
	char *pValue;
	Vector vecResult;

	vecResult.x = atoi(pValue = pString);

	for (int i = 0; i < 2; i++)
	{
		pValue = strchr(pValue + i, ' ');
		vecResult[i + 1] = atoi(pValue);
	}

	return vecResult;
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
		
		if (**line == '"' || **line == ':')
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

char *COM_ParseFile( char *data, char *token )
{
	int	c, len;

	if( !token )
		return NULL;
	
	len = 0;
	token[0] = 0;
	
	if( !data )
		return NULL;
		
// skip whitespace
skipwhite:
	while(( c = ((byte)*data)) <= ' ' )
	{
		if( c == 0 )
			return NULL;	// end of file;
		data++;
	}
	
	// skip // comments
	if( c=='/' && data[1] == '/' )
	{
		while( *data && *data != '\n' )
			data++;
		goto skipwhite;
	}

	// handle quoted strings specially
	if( c == '\"' )
	{
		data++;
		while( 1 )
		{
			c = (byte)*data++;
			if( c == '\"' || !c )
			{
				token[len] = 0;
				return data;
			}
			token[len] = c;
			len++;
		}
	}

	// parse single characters
	if( c == '{' || c == '}' || c == ')' || c == '(' || c == '\'' || c == ',' )
	{
		token[len] = c;
		len++;
		token[len] = 0;
		return data + 1;
	}

	// parse a regular word
	do
	{
		token[len] = c;
		data++;
		len++;
		c = ((byte)*data);

		if( c == '{' || c == '}' || c == ')' || c == '(' || c == '\'' || c == ',' )
			break;
	} while( c > 32 );
	
	token[len] = 0;

	return data;
}
