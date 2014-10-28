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

#define WEAPON_RESPAWN_TIME					20

#define ITEM_FLAG_SELECTONEMPTY				1
#define ITEM_FLAG_NOAUTORELOAD				2
#define ITEM_FLAG_NOAUTOSWITCHEMPTY			4
#define ITEM_FLAG_LIMITINWORLD				8
#define ITEM_FLAG_EXHAUSTIBLE				16

#define WEAPON_IS_CUSTOM			g_Config.WeaponIsCustom
#define WEAPON_IS_DEFAULT			g_Config.WeaponIsDefault
#define WEAPON_MAKE_CUSTOM			g_Config.WeaponMarkAsCustom
#define WEAPON_MAKE_DEFAULT			g_Config.WeaponMarkAsDefault

#define WEAPON_FORWARD_REGISTER		g_Config.WeaponRegisterForward
#define WEAPON_FORWARD_EXECUTE		g_Config.WeaponExecuteForward
#define WEAPON_FORWARD_EXISTS		g_Config.WeaponGetForward


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

#define GET_AMXX_PLUGIN_POINTER(amx)	(CPlugin*)amx->userdata[UD_FINDPLUGIN]
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

enum e_AmmoFwds
{
	Fwd_Ammo_Spawn,
	Fwd_Ammo_AddAmmo,

	Fwd_Ammo_End
};

enum e_WpnFwds
{
	Fwd_Wpn_Spawn,
	Fwd_Wpn_CanDeploy,
	Fwd_Wpn_Deploy,
	Fwd_Wpn_Idle,
	Fwd_Wpn_PrimaryAttack,
	Fwd_Wpn_SecondaryAttack,
	Fwd_Wpn_Reload,
	Fwd_Wpn_CanHolster,
	Fwd_Wpn_Holster,
	Fwd_Wpn_IsUseable,
	Fwd_Wpn_AddToPlayer,
	Fwd_Wpn_AddToPlayer2,
	Fwd_Wpn_ItemPostFrame,

	Fwd_Wpn_End
};

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

typedef struct
{
	std::string	classname;
	int iForward[Fwd_Ammo_End];
} AmmoBoxData;





typedef struct
{
	std::string title;
	std::string author;
	std::string version;

} WeaponData;






enum e_WpnType
{
	Wpn_None,
	Wpn_Default,
	Wpn_Custom,
	Wpn_End
};

class CWeaponInfo
{
public:
	CPlugin*	m_pAmxx;
	e_WpnType	m_WpnType;
	int			m_AmxxForwards[Fwd_Wpn_End];

	CWeaponInfo()
	{
		m_pAmxx = NULL;
		m_WpnType = Wpn_None;
		memset(m_AmxxForwards, 0, sizeof(m_AmxxForwards));
	}
};

class CConfig
{
private:
	bool	m_bInited;
	char	m_cfgpath[1024];
	SUBMOD	m_GameMod;

	CWeaponInfo m_WeaponsInfo[MAX_WEAPONS];

public:
	CConfig();

	int** m_pCurrentSlots;
	int m_iMaxWeaponSlots;
	int m_iMaxWeaponPositions;

	bool m_bCrowbarHooked;
	bool m_bAmmoBoxHooked;

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
	void	ServerShutDown		(void);
	void	ServerDeactivate	(void);
	void	ManageEquipment		(void);
	void	LoadBlackList		(void);

	void	DecalPushList		(const char *name);
	bool	IsItemBlocked		(const char *name);
	bool	CheckSlots			(int iWeaponID);

	static void ServerCommand	(void);
	static bool ClientCommand	(edict_t *pEntity);

	void WeaponMarkAsCustom		(int iId)	{ m_WeaponsInfo[iId].m_WpnType = Wpn_Custom; };
	void WeaponMarkAsDefault	(int iId)	{ m_WeaponsInfo[iId].m_WpnType = Wpn_Default; };

	bool WeaponIsCustom		(int iId)	{ return m_WeaponsInfo[iId].m_WpnType == Wpn_Custom; };
	bool WeaponIsDefault	(int iId)	{ return m_WeaponsInfo[iId].m_WpnType == Wpn_Default; };

	int WeaponRegisterForward(int iId, e_WpnFwds fwdType, AMX *amx, const char * pFuncName)
	{
		int iRegResult = MF_RegisterSPForwardByName(amx, pFuncName, FP_CELL, FP_CELL, FP_CELL, FP_CELL, FP_CELL, FP_DONE);

		if (iRegResult == -1)
		{
			return 0;
		}

		m_WeaponsInfo[iId].m_AmxxForwards[fwdType] = iRegResult;
		return iRegResult;
	}

	int WeaponExecuteForward(int iId, e_WpnFwds fwdType, edict_t* pWeapon, edict_t* pPlayer)
	{
		if (!m_WeaponsInfo[iId].m_AmxxForwards[fwdType])
		{
			return 0;
		}

		int iAmmo1 = 0;
		int iAmmo2 = 0;
		int iPlayer = 0;

		if (IsValidPev(pPlayer))
		{
			iPlayer = ENTINDEX(pPlayer);
			iAmmo1 = GetAmmoInventory(pPlayer, PrimaryAmmoIndex(pWeapon));
			iAmmo2 = GetAmmoInventory(pPlayer, SecondaryAmmoIndex(pWeapon));
		}

		return MF_ExecuteForward
		(
			m_WeaponsInfo[iId].m_AmxxForwards[fwdType],

			static_cast<cell>(ENTINDEX(pWeapon)),
			static_cast<cell>(iPlayer),
			static_cast<cell>(GetPrivateInt(pWeapon, pvData_iClip)),
			static_cast<cell>(iAmmo1),
			static_cast<cell>(iAmmo2)
		);
	}

	int WeaponGetForward(int iId, e_WpnFwds fwdType)
	{
		return m_WeaponsInfo[iId].m_AmxxForwards[fwdType];
	}

	//void		WeaponSetAmxxPlugin		(int iId, CPlugin* plugin);
	//CPlugin*	WeaponGetAmxxPlugin		(int iId);
};

extern CConfig g_Config;

extern int g_iAmmoBoxIndex;

extern WeaponData WeaponInfoArray[MAX_WEAPONS];
extern AmmoBoxData AmmoBoxInfoArray[MAX_WEAPONS];

extern const char* gWeaponReference;
extern const char* gAmmoBoxReference;

extern cvar_t *cvar_sv_cheats;
extern cvar_t *cvar_mp_weaponstay;

extern AMX_NATIVE_INFO Natives[];

extern void WpnMod_Init_GameMod(void);
extern void WpnMod_Precache(void);


#endif // _CONFIG_H
