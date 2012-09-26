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
#include <hooks.h>
#include <utils.h>

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

	if (!strcmp(pKey, "classname"))
	{
		int i;

		for (i = LIMITER_WEAPON + 1; i <= g_iWeaponIndex; i++)
		{
			if (!_stricmp(GetWeapon_pszName(i), pValue))
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

	char token[2048];

	while(( data = COM_ParseFile(data, token)) != NULL )
	{
		if (strcmp(token, "{"))
		{
			return false;
		}

		newent = true;

		while (true)
		{
			if (!( data = COM_ParseFile(data, token)))
			{
				return false;
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
	return true;
}
