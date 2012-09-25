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

#ifndef _WPNMOD_H
#define _WPNMOD_H

#include "amxxmodule.h"
#include "cbase.h"

#pragma warning( disable : 164 ) 

#define LIMITER_WEAPON						15

#define WEAPON_RESPAWN_TIME					20
#define AMMO_RESPAWN_TIME					20

#define ITEM_FLAG_SELECTONEMPTY				1
#define ITEM_FLAG_NOAUTORELOAD				2
#define ITEM_FLAG_NOAUTOSWITCHEMPTY			4
#define ITEM_FLAG_LIMITINWORLD				8
#define ITEM_FLAG_EXHAUSTIBLE				16

#define SetEntForward(ent, call, handler, forward) \
	g_Ents[ENTINDEX(ent)].i##call = forward; \
	Set##call##_(ent, handler);

#define GetEntForward(ent, call) g_Ents[ENTINDEX(ent)].i##call


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

	Fwd_Wpn_End
};

typedef enum
{
	PLAYER_IDLE,
	PLAYER_WALK,
	PLAYER_JUMP,
	PLAYER_SUPERJUMP,
	PLAYER_DIE,
	PLAYER_ATTACK1,
} PLAYER_ANIM;

typedef struct
{
	int		iSlot;
	int		iPosition;
	const char	*pszAmmo1;	// ammo 1 type
	int		iMaxAmmo1;		// max ammo 1
	const char	*pszAmmo2;	// ammo 2 type
	int		iMaxAmmo2;		// max ammo 2
	const char	*pszName;
	int		iMaxClip;
	int		iId;
	int		iFlags;
	int		iWeight;// this value used to determine this weapon's importance in autoselection.
} ItemInfo;

typedef struct
{
	int iThink;
	int iTouch;
} EntData;

typedef struct
{
	ItemInfo ItemData;
	int iForward[Fwd_Wpn_End];
} WeaponData;

typedef struct
{
	const char	*pszName;
	int iForward[Fwd_Ammo_End];
} AmmoBoxData;

extern int g_iWeaponIndex;
extern int g_iAmmoBoxIndex;

extern BOOL g_InitWeapon;
extern BOOL g_initialized;
extern BOOL g_CrowbarHooksEnabled;

extern EntData *g_Ents;

extern cvar_t *cvar_aghlru;
extern cvar_t *cvar_sv_cheats;
extern cvar_t *cvar_mp_weaponstay;

extern WeaponData WeaponInfoArray[MAX_WEAPONS];
extern AmmoBoxData AmmoBoxInfoArray[MAX_WEAPONS];

extern AMX_NATIVE_INFO Natives[];

extern void WpnModCommand(void);
extern int ParseBSPEntData(char *file);

inline int			iSlot(const int iId)			{ return WeaponInfoArray[iId].ItemData.iSlot; }
inline int			iItemPosition(const int iId)	{ return WeaponInfoArray[iId].ItemData.iPosition; }
inline const char	*pszAmmo1(const int iId)		{ return WeaponInfoArray[iId].ItemData.pszAmmo1; }
inline int			iMaxAmmo1(const int iId)		{ return WeaponInfoArray[iId].ItemData.iMaxAmmo1; }
inline const char	*pszAmmo2(const int iId)		{ return WeaponInfoArray[iId].ItemData.pszAmmo2; }
inline int			iMaxAmmo2(const int iId)		{ return WeaponInfoArray[iId].ItemData.iMaxAmmo2; }
inline const char	*pszName(const int iId)			{ return WeaponInfoArray[iId].ItemData.pszName; }
inline int			iMaxClip(const int iId)			{ return WeaponInfoArray[iId].ItemData.iMaxClip; }
inline int			iWeight(const int iId)			{ return WeaponInfoArray[iId].ItemData.iWeight; }
inline int			iFlags(const int iId)			{ return WeaponInfoArray[iId].ItemData.iFlags; }


#endif // _WPNMOD_H
