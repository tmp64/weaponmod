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

#include "wpnmod_log.h"


CLog::CLog()
{
	m_LogDir.clear();
}

void CLog::Init()
{
	m_LogDir.assign(LOCALINFO((char*)"amxx_logs"));
	m_LogDir.append("/weaponmod");

	char file[256];
#ifdef __linux
	mkdir(build_pathname_r(file, sizeof(file)-1, "%s", m_LogDir.c_str()), 0700);
#else
	mkdir(build_pathname_r(file, sizeof(file)-1, "%s", m_LogDir.c_str()));
#endif
}

void CLog::Log(const char *fmt, ...)
{
	time_t td;
	time(&td);
	tm *curTime = localtime(&td);

	char date[32];
	strftime(date, 31, "%m/%d/%Y - %H:%M:%S", curTime);

	static char msg[3072];

	va_list arglst;
	va_start(arglst, fmt);
	vsnprintf(msg, 3071, fmt, arglst);
	va_end(arglst);

	static char file[256];
	build_pathname_r(file, sizeof(file)-1, "%s/L%04d%02d%02d.log", m_LogDir.c_str(), (curTime->tm_year + 1900), curTime->tm_mon + 1, curTime->tm_mday);

	FILE *pF = fopen(file, "a+");

	if (pF)
	{
		fprintf(pF, "L %s: %s", date, msg);
		fclose(pF);
	}

	printf2("[%s]: %s", Plugin_info.logtag, msg);
}

char *build_pathname_r(char *buffer, size_t maxlen, char *fmt, ...)
{
	snprintf(buffer, maxlen, "%s%c", GET_GAME_INFO(PLID, GINFO_GAMEDIR), PATH_SEP_CHAR);

	size_t len = strlen(buffer);
	char *ptr = buffer + len;

	va_list argptr;
	va_start(argptr, fmt);
	vsnprintf (ptr, maxlen-len, fmt, argptr);
	va_end (argptr);

	while (*ptr) 
	{
		if (*ptr == ALT_SEP_CHAR)
		{
			*ptr = PATH_SEP_CHAR;
		}
		++ptr;
	}

	return buffer;
}

void printf2(const char* fmt, ...)
{
	va_list argptr;
	static char string[MAX_LOGMSG_LEN];
	va_start(argptr, fmt);
	vsnprintf(string, sizeof(string) - 1, fmt, argptr);
	string[sizeof(string) - 1] = '\0';
	va_end(argptr);

	SERVER_PRINT(string);
}

