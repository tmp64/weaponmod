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

#include "wpnmod_vhooker.h"
#include "wpnmod_parse.h"
#include "wpnmod_utils.h"
#include "wpnmod_hooks.h"
#include "utils.h"


BOOL ParseConfigSection(char *Filepath, char *pSection, void *pHandler)
{
	BOOL bFound = FALSE;
	BOOL bResult = FALSE;

	FILE *stream = fopen(Filepath, "r");

	if (stream)
	{
		char data[2048];

		while (!feof(stream) && fgets(data, sizeof(data) - 1, stream))
		{
			char *b = &data[0];
			Util::TrimLine(b);

			if (*b && *b != ';')
			{
				if (!strcmp(pSection, b))
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
	VirtualHookData *p = new VirtualHookData;
	
	p->done = NULL;
	p->handler = NULL;
	p->address = NULL;
	p->classname = STRING(ALLOC_STRING(szBlockItem));

	if (!stricmp(szBlockItem, "weapon_crowbar") || !stricmp(szBlockItem, "ammo_rpgclip"))
	{
		g_BlockedItems.push_back(p);
		return;
	}

	if (strstr(szBlockItem, "weapon_"))
	{
		p->offset = VO_AddToPlayer;
	}
	else if (strstr(szBlockItem, "ammo_"))
	{
		p->offset = VO_AddAmmo;
	}
	
	p->handler = (void*)Item_Block;
	SetHookVirtual(p);

	if (!p->done)
	{
		delete p;
	}
	else
	{
		g_BlockedItems.push_back(p);
	}
}

void ParseSpawnPoints()
{		
	char filepath[1024];

	MF_BuildPathnameR(filepath, sizeof(filepath) - 1, "%s/weaponmod/spawnpoints/%s.ini", MF_GetLocalInfo("amxx_configsdir", "addons/amxmodx/configs"), STRING(gpGlobals->mapname));
	FILE *stream = fopen(filepath, "r");

	if (stream)
	{
		//printf2("[%s]: Spawned custom items from \"%s.ini\":\n", Plugin_info.logtag, STRING(gpGlobals->mapname));

		char data[2048];
		int wpns = 0, ammoboxes = 0;

		while (!feof(stream))
		{
			fgets(data, sizeof(data) - 1, stream);

			char *b = &data[0];

			if (*b != ';')
			{
				char* arg;
				char szData[3][32];

				for (int state, i = 0; i < 3; i++)
				{
					arg = Util::ParseArg(&b, state, '"');
					strcpy(szData[i], arg);
				}

				Vector vecOrigin = strlen(szData[1]) ? Util::ParseVec(szData[1]) : Vector(0, 0, 0);
				Vector vecAngles = strlen(szData[2]) ? Util::ParseVec(szData[2]) : Vector(0, 0, 0);

				if (Weapon_Spawn(szData[0], vecOrigin, vecAngles))
				{
					wpns++;
					//printf2("   %s at %f %f %f\n", szData[0], vecOrigin.x, vecOrigin.y, vecOrigin.z);
				}

				if (Ammo_Spawn(szData[0], vecOrigin, vecAngles))
				{
					ammoboxes++;
					//printf2("   %s at %f %f %f\n", szData[0], vecOrigin.x, vecOrigin.y, vecOrigin.z);
				}

			}
		}

		printf2("[%s]: Spawned %d weapons and %d ammoboxes from \"%s.ini\".\n", Plugin_info.logtag, wpns, ammoboxes, STRING(gpGlobals->mapname));
		fclose(stream);
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
		arg = Util::ParseArg(&data, state, ':');
		
		Util::TrimLine(arg);
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
		arg = Util::ParseArg(&data, state, ':');
		
		Util::TrimLine(arg);
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
	static vec_t AngleX;
	static vec_t AngleY;
	static Vector vecOrigin;

	if (iNewent)
	{
		AngleX = AngleY = vecOrigin.x = vecOrigin.y = vecOrigin.z = 0;
	}

	if (!strcmp(pKey, "angle"))
	{
		AngleY = atoi(pValue);

		if (AngleY == -1)
		{
			AngleX = -90;
			AngleY = 0;
		}
		else if (AngleY == -2)
		{
			AngleX = 90;
			AngleY = 0;
		}
	}

	if (!strcmp(pKey, "origin"))
	{
		vecOrigin = Util::ParseVec(pValue);
	}

	if (!strcmp(pKey, "classname") && !Ammo_Spawn(pValue, vecOrigin, Vector(AngleX, AngleY, 0)))
	{
		Weapon_Spawn(pValue, vecOrigin, Vector(AngleX, AngleY, 0));
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

	while(( data = Util::COM_ParseFile(data, token)) != NULL )
	{
		if (strcmp(token, "{"))
		{
			return;
		}

		newent = true;

		while (true)
		{
			if (!( data = Util::COM_ParseFile(data, token)))
			{
				return;
			}

			if (!strcmp(token, "}"))
			{
				break;
			}

			strcpy(key, token);
			data = Util::COM_ParseFile(data, token);
			strcpy(value, token);

			KeyValueFromBSP(key, value, newent);
			newent = false;
		}
	}

	free(data);
}
