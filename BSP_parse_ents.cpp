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

// Thanks to Eg@r4$il{ and HLSDK.

#include <weaponmod.h>


#define	MAX_INCLUDES	8
#define	MAXTOKEN		512


typedef struct
{
	char filename[1024];
	char *buffer,*script_p,*end_p;
	int line;
} script_t;


qboolean endofscript;
qboolean tokenready;  

script_t scriptstack[MAX_INCLUDES];
script_t *script;

int scriptline;
char token[MAXTOKEN];
char qdir[1024] = {'\0'};

qboolean GetToken(qboolean crossline);


void ParseFromMemory(char *buffer, int size)
{
	script = scriptstack;
	script++;

	strcpy(script->filename, "memory buffer");

	script->buffer = buffer;
	script->line = 1;
	script->script_p = script->buffer;
	script->end_p = script->buffer + size;

	endofscript = false;
	tokenready = false;
}


int filelength(FILE *f)
{
	int pos;
	int end;

	pos = ftell (f);
	fseek (f, 0, SEEK_END);
	end = ftell (f);
	fseek (f, pos, SEEK_SET);

	return end;
}


int LoadFile(char *filename, void **bufferptr)
{
	FILE *stream;
	
	void *buffer = NULL;
	int length = 0;

	stream = fopen(filename, "rb");

	if (stream)
	{
		length = filelength (stream);
		buffer = malloc (length+1);
		
		((char *)buffer)[length] = 0;

		fread(buffer, 1, length, stream);
		fclose (stream);
		
		*bufferptr = buffer;
	}

	return length;
}


char *ExpandPath(char *path)
{
	char *psz;
	static char full[1024];

	if (path[0] == '/' || path[0] == '\\' || path[1] == ':')
	{
		return path;
	}

	psz = strstr(path, qdir);
	
	if (psz)
	{
		strcpy(full, path);
	}
	else
	{
		sprintf(full, "%s%s", qdir, path);
	}

	return full;
}


void AddScriptToStack(char *filename)
{
	int size;
	script++;

	strcpy(script->filename, ExpandPath (filename) );
	size = LoadFile (script->filename, (void **)&script->buffer);
	script->line = 1;
	script->script_p = script->buffer;
	script->end_p = script->buffer + size;
}


qboolean EndOfScript(qboolean crossline)
{
	if (!strcmp(script->filename, "memory buffer"))
	{
		endofscript = true;
		return false;
	}

	free (script->buffer);

	if (script == scriptstack+1)
	{
		endofscript = true;
		return false;
	}

	script--;
	scriptline = script->line;

	return GetToken(crossline);
}


qboolean GetToken(qboolean crossline)
{
	char *token_p;

	if (tokenready)
	{
		tokenready = false;
		return true;
	}

	if (script->script_p >= script->end_p)
		return EndOfScript (crossline);

skipspace:
	while (*script->script_p <= 32)
	{
		if (script->script_p >= script->end_p)
		{
			return EndOfScript (crossline);
		}

		if (*script->script_p++ == '\n')
		{
			scriptline = script->line++;
		}
	}

	if (script->script_p >= script->end_p)
	{
		return EndOfScript (crossline);
	}

	if (*script->script_p == ';' || *script->script_p == '#' ||		 // semicolon and # is comment field
		(*script->script_p == '/' && *((script->script_p)+1) == '/')) // also make // a comment field
	{											
		while (*script->script_p++ != '\n')
			if (script->script_p >= script->end_p)
				return EndOfScript (crossline);
		goto skipspace;
	}

	token_p = token;

	if (*script->script_p == '"')
	{
		// quoted token
		script->script_p++;

		while (*script->script_p != '"')
		{
			*token_p++ = *script->script_p++;

			if (script->script_p == script->end_p)
			{
				break;
			}
		}

		script->script_p++;
	}
	else while (*script->script_p > 32 && *script->script_p != ';')
	{
		*token_p++ = *script->script_p++;

		if (script->script_p == script->end_p)
		{
			break;
		}
	}

	*token_p = 0;

	if (!strcmp (token, "$include"))
	{
		GetToken (false);
		AddScriptToStack (token);
		
		return GetToken (crossline);
	}

	return true;
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


void KeyValueFromBSP(char *pKey, char *pValue, int iNewent)
{
	static vec_t AngleY;
	static Vector vecOrigin;

	if (iNewent)
	{
		AngleY = 0;
		vecOrigin.x = vecOrigin.y = vecOrigin.z = 0;
	}

	if (!strcmp(pKey, "angle"))
	{
		AngleY = atoi(pValue);
	}

	if (!strcmp(pKey, "origin"))
	{
		vecOrigin = ParseVec(pValue);
	}

	if (!strcmp(pKey, "classname"))
	{
		int i;

		for (i = LIMITER_WEAPON + 1; i <= g_iWeaponIndex; i++)
		{
			if (!_stricmp(pszName(i), pValue))
			{
				Weapon_Spawn(i, vecOrigin, Vector (0, AngleY, 0));
			}
		}

		for (i = 0; i < g_iAmmoBoxIndex; i++)
		{
			if (!_stricmp(AmmoBoxInfoArray[i].pszName, pValue))
			{
				Ammo_Spawn(i, vecOrigin, Vector (0, AngleY, 0));
			}
		}
	}
}


int ParseBSPEntData(char *file)
{
	FILE *fp;

	int tmp, size;
	char key[512];
	char value[512];
	bool newent = false;

	fp = fopen(file, "rb");
	
	if (!fp)
	{
		return false;
	}

	fread(&tmp, 4, 1, fp);

	if (tmp != 30)
	{
		return false;
	}

	fread(&tmp, 4, 1, fp);
	fread(&size, 4, 1, fp);
	
	char *data = (char*)malloc(size);
	
	if (!data)
	{
		return false;
	}

	fseek(fp, tmp, SEEK_SET);
	fread(data, size, 1, fp);
	fclose(fp);

	ParseFromMemory(data, size);

	while (GetToken(true))
	{
		if (strcmp(token, "{"))
		{
			return false;
		}

		newent = true;

		while (true)
		{
			if (!GetToken(true))
			{
				return false;
			}

			if (!strcmp(token, "}"))
			{
				break;
			}

			strcpy(key, token);
			GetToken(false);
			strcpy(value, token);

			KeyValueFromBSP(key, value, newent);
			newent = false;
		}
	}

	free(data);
	return true;
}
