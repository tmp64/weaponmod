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

#include "wpnmod_entity.h"
#include "wpnmod_parse.h"
#include "wpnmod_utils.h"
#include "wpnmod_hooks.h"


bool ParseSection(char* filePath, const char* section, void* handler, int separator)
{
	bool found = false;
	bool result = false;

	FILE* f = fopen(filePath, "r");

	if (f)
	{
		int pos;
		char buffer[512];
		std::string lineRead;

		while (!feof(f) && fgets(buffer, sizeof(buffer) - 1, f))
		{
			lineRead = buffer;
			//lineRead.trim();
			lineRead.erase(lineRead.find_last_not_of(" \n\r\t") + 1);

			if (lineRead.size() && lineRead.at(0) != ';')
			{
				if (!lineRead.compare(section))
				{
					found = true;
					continue;
				}
				else if (found)
				{
					if (lineRead.at(0) == '[')
					{
						break;
					}

					result = true;

					if (handler)
					{
						if (separator != -1 && (pos = lineRead.find(separator)) != -1)
						{
							std::string stringLeft = lineRead.substr(0, pos);
							std::string stringRight = lineRead.substr(pos + 1);

							if ((pos = stringRight.find(';')) != -1)
							{
								stringRight.erase(pos);
							}

							//stringLeft.trim();
							//stringRight.trim();
							stringLeft.erase(stringLeft.find_last_not_of(" \n\r\t") + 1);
							stringRight.erase(stringRight.find_last_not_of(" \n\r\t") + 1);

							reinterpret_cast<void(*)(std::string, std::string)>(handler)(stringLeft, stringRight);
						}
						else
						{
							reinterpret_cast<void(*)(std::string, std::string)>(handler)(NULL, lineRead);
						}
					}
				}
			}
		}

		fclose(f);
	}

	return result;
}

void OnParseStartAmmos(std::string item, std::string count)
{
	StartAmmo *p = new StartAmmo;

	p->ammoname = STRING(ALLOC_STRING(item.c_str()));
	p->count = max(min(atoi(count.c_str()), 254), 0);

	g_Config.m_pStartAmmoList.push_back(p);
}

void OnParseWeaponbox(std::string key, std::string value)
{
	if (!strcmp(key.c_str(), "setmodel"))
	{
		g_Config.m_bWpnBoxModels = atoi(value.c_str()) > 0 ? true : false;
	}
	else if (!strcmp(key.c_str(), "rendercolor"))
	{
		g_Config.m_iWpnBoxRenderColor = atoi(value.c_str());
	}
	else if (!strcmp(key.c_str(), "lifetime"))
	{
		g_Config.m_iWpnBoxLifeTime = atoi(value.c_str());
	}
}

void OnParseBlockedItems(std::string dummy, std::string BlockedItem)
{
	VirtualHookData *p = new VirtualHookData;

	p->done = false;
	p->handler = NULL;
	p->address = NULL;
	p->classname = STRING(ALLOC_STRING(BlockedItem.c_str()));

	if (!stricmp(BlockedItem.c_str(), "weapon_crowbar") || !stricmp(BlockedItem.c_str(), "ammo_rpgclip"))
	{
		g_Config.m_pBlockedItemsList.push_back(p);
		return;
	}

	if (strstr(BlockedItem.c_str(), "weapon_"))
	{
		p->offset = VO_AddToPlayer;
	}
	else if (strstr(BlockedItem.c_str(), "ammo_"))
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
		g_Config.m_pBlockedItemsList.push_back(p);
	}
}

void OnParseStartEquipments(std::string item, std::string count)
{
	if (!g_Config.m_pEquipEnt)
	{
		g_Config.ManageEquipment();
	}

	if (g_Config.m_pEquipEnt)
	{
		KeyValueData kvd;

		kvd.szClassName = (char*)STRING(g_Config.m_pEquipEnt->v.classname);
		kvd.szKeyName = (char*)item.c_str();
		kvd.szValue = (char*)count.c_str();
		kvd.fHandled = 0;

		MDLL_KeyValue(g_Config.m_pEquipEnt, &kvd);
	}
}

void ParseSpawnPoints()
{
	char filepath[1024];

	build_pathname_r(filepath, sizeof(filepath) - 1, "%s/weaponmod/spawnpoints/%s.ini", LOCALINFO((char*)"amxx_configsdir"), STRING(gpGlobals->mapname));
	FILE *stream = fopen(filepath, "r");

	if (stream)
	{
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
					arg = ParseArg(&b, state, '"');
					strcpy(szData[i], arg);
				}

				Vector vecOrigin = strlen(szData[1]) ? ParseVec(szData[1]) : Vector(0, 0, 0);
				Vector vecAngles = strlen(szData[2]) ? ParseVec(szData[2]) : Vector(0, 0, 0);

				if (Weapon_Spawn(szData[0], vecOrigin, vecAngles))
				{
					wpns++;
				}

				if (Ammo_Spawn(szData[0], vecOrigin, vecAngles))
				{
					ammoboxes++;
				}
			}
		}

		WPNMOD_LOG("Spawned: \"%s.ini\": %d weapons and %d ammoboxes.\n", STRING(gpGlobals->mapname), wpns, ammoboxes);
		fclose(stream);
	}
}

void ParseBSP()
{
	char filepath[1024];
	build_pathname_r(filepath, sizeof(filepath) - 1, "maps/%s.bsp", STRING(gpGlobals->mapname));

	FILE *fp = fopen(filepath, "rb");
	
	if (!fp)
	{
		return;
	}

	int tmp;
	int size;

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

	char key[2048];
	char value[2048];
	char token[2048];

	Vector vecOrigin;
	Vector vecAngles;

	std::string classname;

	while ((data = COM_ParseFile(data, token)) != NULL)
	{
		if (strcmp(token, "{"))
		{
			free(data);
			return;
		}

		while (true)
		{
			if (!(data = COM_ParseFile(data, token)))
			{
				free(data);
				return;
			}

			if (!strcmp(token, "}"))
			{
				if (classname.size())
				{
					Wpnmod_SpawnItem(classname.c_str(), vecOrigin, vecAngles);
				}

				classname.clear();
				vecOrigin = vecAngles = Vector(0, 0, 0);

				break;
			}

			strcpy(key, token);
			data = COM_ParseFile(data, token);
			strcpy(value, token);

			if (!strcmp(key, "origin"))
			{
				vecOrigin = ParseVec(value);
			}

			if (!strcmp(key, "classname"))
			{
				classname.append(value);
			}

			if (!strcmp(key, "angle"))
			{
				vecAngles.y = atoi(value);

				if (vecAngles.y == -1)
				{
					vecAngles.x = -90;
					vecAngles.y = 0;
				}
				else if (vecAngles.y == -2)
				{
					vecAngles.x = 90;
					vecAngles.y = 0;
				}
			}
		}
	}

	free(data);
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

char* ParseArg(char** line, int& state, char delimiter)
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

		if (**line == delimiter)
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

char *COM_ParseFile(char *data, char *token )
{
	int	c, len;

	if (!token)
	{
		return NULL;
	}

	len = 0;
	token[0] = 0;

	if (!data)
	{
		return NULL;
	}

// skip whitespace
skipwhite:
	while ((c = ((byte)*data)) <= ' ')
	{
		if (c == 0)
		{
			return NULL;	// end of file;
		}

		data++;
	}

	// skip // comments
	if (c=='/' && data[1] == '/')
	{
		while (*data && *data != '\n')
		{
			data++;
		}

		goto skipwhite;
	}

	// handle quoted strings specially
	if (c == '\"')
	{
		data++;

		while (1)
		{
			c = (byte)*data++;

			if (c == '\"' || !c)
			{
				token[len] = 0;
				return data;
			}

			token[len] = c;
			len++;
		}
	}

	// parse single characters
	if (c == '{' || c == '}' || c == ')' || c == '(' || c == '\'' || c == ',')
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

		if (c == '{' || c == '}' || c == ')' || c == '(' || c == '\'' || c == ',')
		{
			break;
		}

	} while (c > 32);

	token[len] = 0;

	return data;
}

