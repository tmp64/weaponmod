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

#include "amxxmodule.h"
#include "cbase.h"
#include "utils.h"

#ifdef __linux__
	#include <sys/mman.h>
	#include <malloc.h>
	#include <dlfcn.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>

	typedef unsigned long DWORD;
	typedef unsigned short WORD;
	typedef unsigned int UNINT32;
#endif

#define LIMITER_WEAPON						15

#define WEAPON_RESPAWN_TIME					20
#define AMMO_RESPAWN_TIME					20

#define ITEM_FLAG_SELECTONEMPTY				1
#define ITEM_FLAG_NOAUTORELOAD				2
#define ITEM_FLAG_NOAUTOSWITCHEMPTY			4
#define ITEM_FLAG_LIMITINWORLD				8
#define ITEM_FLAG_EXHAUSTIBLE				16

#ifdef _WIN32
#define XTRA_OFS_WEAPON						0
#define XTRA_OFS_PLAYER						0
#elif __linux__
#define XTRA_OFS_WEAPON						4
#define XTRA_OFS_PLAYER						5
#endif

#define m_flStartThrow						XTRA_OFS_WEAPON + 16
#define m_flReleaseThrow					XTRA_OFS_WEAPON + 17
#define m_chargeReady						XTRA_OFS_WEAPON + 18
#define m_fInAttack							XTRA_OFS_WEAPON + 19
#define m_fireState							XTRA_OFS_WEAPON + 20
#define m_pPlayer							XTRA_OFS_WEAPON + 28
#define m_iId								XTRA_OFS_WEAPON + 30
#define m_iPlayEmptySound					XTRA_OFS_WEAPON + 31
#define m_fFireOnEmpty						XTRA_OFS_WEAPON + 32
#define m_flPumpTime						XTRA_OFS_WEAPON + 33
#define m_fInSpecialReload					XTRA_OFS_WEAPON + 34
#define m_flNextPrimaryAttack				XTRA_OFS_WEAPON + 35
#define m_flNextSecondaryAttack				XTRA_OFS_WEAPON + 36
#define m_flTimeWeaponIdle					XTRA_OFS_WEAPON + 37
#define m_iPrimaryAmmoType					XTRA_OFS_WEAPON + 38
#define m_iSecondaryAmmoType				XTRA_OFS_WEAPON + 39
#define m_iClip								XTRA_OFS_WEAPON + 40
#define m_fInReload							XTRA_OFS_WEAPON + 43
#define m_iDefaultAmmo						XTRA_OFS_WEAPON + 44
#define m_LastHitGroup						XTRA_OFS_PLAYER + 90
#define m_flNextAttack						XTRA_OFS_PLAYER + 148
#define m_iWeaponVolume						XTRA_OFS_PLAYER + 173
#define m_iWeaponFlash						XTRA_OFS_PLAYER + 175
#define m_iFOV								XTRA_OFS_PLAYER + 298
#define m_rgAmmo							XTRA_OFS_PLAYER + 310
#define m_szAnimExtention					XTRA_OFS_PLAYER + 387


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

typedef struct usercmd_s
{
	short	lerp_msec;      // Interpolation time on client
	byte	msec;           // Duration in ms of command
	vec3_t	viewangles;     // Command view angles.

	// intended velocities
	float	forwardmove;    // Forward velocity.
	float	sidemove;       // Sideways velocity.
	float	upmove;         // Upward velocity.
	byte	lightlevel;     // Light level at spot where we are standing.
	unsigned short  buttons;  // Attack buttons
	byte    impulse;          // Impulse command issued.
	byte	weaponselect;	// Current weapon id

	// Experimental player impact stuff.
	int		impact_index;
	vec3_t	impact_position;
} usercmd_t;

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

extern WeaponData WeaponInfoArray[MAX_WEAPONS];
extern AmmoBoxData AmmoBoxInfoArray[MAX_WEAPONS];

extern AMX_NATIVE_INFO Natives[];

extern BOOL g_CrowbarHooksEnabled;
extern BOOL g_InfoTargetHooksEnabled;
extern BOOL g_InitWeapon;
extern BOOL g_initialized;

extern short g_sModelIndexBloodDrop;
extern short g_sModelIndexBloodSpray;

extern void WpnModCommand(void);

extern void ActivateCrowbarHooks();
extern void ActivateInfoTargetHooks();

extern Vector ParseVec(char *pString);
extern int ParseBSPEntData(char *file);
extern char* parse_arg(char** line, int& state);

extern edict_t* Ammo_Spawn(int iId, Vector vecOrigin, Vector vecAngles);
extern edict_t* Weapon_Spawn(int iId, Vector vecOrigin, Vector vecAngles);

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
