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

#ifndef _CONFIG_H
#define _CONFIG_H

#include "amxxmodule.h"

#include <map>
#include <string>
#include <vector>

#include "cbase.h"

#include "wpnmod_vtable.h"
#include "wpnmod_pvdata.h"
#include "wpnmod_memory.h"
#include "wpnmod_log.h"
#include "wpnmod_utils.h"

#ifdef __linux__
	#define stricmp	strcasecmp
#endif




//
// CPlugin.h AMXX
//

#include "CString.h"

#define PS_STOPPED		4
#define UD_FINDPLUGIN	3

class CPlugin
{
public:
	AMX amx;
	void* code;

	String	name;
	String	version;
	String	title;
	String	author;
	String	errorMsg;

	char padding[0x10]; // + 16

	int status;
};

#define GET_AMXX_PLUGIN_POINTER(amx)	((CPlugin*)amx->userdata[UD_FINDPLUGIN])
#define STOP_AMXX_PLUGIN(amx)			((CPlugin*)amx->userdata[UD_FINDPLUGIN])->status = PS_STOPPED


typedef enum
{
	SUBMOD_UNKNOWN = 0,

	SUBMOD_AG,
	SUBMOD_VALVE,
	SUBMOD_GEARBOX,

	SUBMOD_AGHLRU,
	SUBMOD_MINIAG,
} SUBMOD;










typedef struct
{
	std::string title;
	std::string author;
	std::string version;

} WeaponData;











typedef struct
{
	const char*	name;
	int			index;
} DecalList;

typedef struct
{
	const char*	ammoname;
	int count;
} StartAmmo;

class CConfig
{
private:
	bool	m_bInited;
	char	m_cfgpath[1024];
	SUBMOD	m_GameMod;

public:
	CConfig()
	{
		m_bInited = false;
		m_GameMod = SUBMOD_UNKNOWN;
		m_pEquipEnt = NULL;
	};

	edict_t* m_pEquipEnt;

	std::vector <DecalList*> m_pDecalList;
	std::vector <StartAmmo*> m_pStartAmmoList;
	std::vector <VirtualHookData*>	m_pBlockedItemsList;

	void	InitGameMod			(void);
	void	SetConfigFile		(void);
	char*	GetConfigFile		(void)	{ return &m_cfgpath[0]; };
	bool	IsInited			(void)	{ return m_bInited; };

	SUBMOD	GetSubMod			(void)	{ return m_GameMod; };
	SUBMOD	CheckSubMod			(const char* game);

	void	ServerActivate		(void);
	void	ServerDeactivate	(void);
	void	ManageEquipment		(void);
	void	LoadBlackList		(void);

	void	DecalPushList		(const char *name);
	bool	IsItemBlocked		(const char *name);

	static void ServerCommand	(void);
	static bool ClientCommand	(edict_t *pEntity);
};

extern CConfig g_Config;

extern WeaponData WeaponInfoArray[MAX_WEAPONS];

extern cvar_t *cvar_sv_cheats;
extern cvar_t *cvar_mp_weaponstay;

extern AMX_NATIVE_INFO Natives[];

extern void WpnMod_Init_GameMod(void);
extern void WpnMod_Precache(void);


#endif // _CONFIG_H
