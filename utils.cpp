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

#include "utils.h"

namespace Util
{
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

	void TrimLine(char *input)
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
}
