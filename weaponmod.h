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


#ifdef __linux__
	#include <sys/mman.h>
	#include <malloc.h>
	#include <dlfcn.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#define	PAGE_SIZE 4096
	#define Align(addr) (void*)((long)addr & ~(PAGE_SIZE-1))
#endif

#include "CEntity.h"
#include "CVirtHook.h"

#define WEAPON_RESPAWN_TIME						20
#define AMMO_RESPAWN_TIME						20

#define CBTEXTURENAMEMAX						13			// only load first n chars of name

#define CHAR_TEX_CONCRETE						'C'			// texture types
#define CHAR_TEX_METAL							'M'
#define CHAR_TEX_DIRT							'D'
#define CHAR_TEX_VENT							'V'
#define CHAR_TEX_GRATE							'G'
#define CHAR_TEX_TILE							'T'
#define CHAR_TEX_SLOSH							'S'
#define CHAR_TEX_WOOD							'W'
#define CHAR_TEX_COMPUTER						'P'
#define CHAR_TEX_GLASS							'Y'
#define CHAR_TEX_FLESH							'F'

#define LIMITER_WEAPON							15

#define ITEM_FLAG_SELECTONEMPTY					1
#define ITEM_FLAG_NOAUTORELOAD					2
#define ITEM_FLAG_NOAUTOSWITCHEMPTY				4
#define ITEM_FLAG_LIMITINWORLD					8
#define ITEM_FLAG_EXHAUSTIBLE					16

#define IsValidPev(pEntity) (!FNullEnt(pEntity) && pEntity->pvPrivateData)

#ifdef _WIN32
	#define XTRA_OFS_WEAPON						0
	#define XTRA_OFS_PLAYER						0
	
	#define PrivateToEdict(pPrivate) (*(entvars_t **)((char*)pPrivate + 4))->pContainingEntity
#elif __linux__
	#define XTRA_OFS_WEAPON						4
	#define XTRA_OFS_PLAYER						5
	
    #define PrivateToEdict(pPrivate) (*(entvars_t **)pPrivate)->pContainingEntity
#endif

#define CHECK_ENTITY(x) \
	if (x != 0 && (FNullEnt(INDEXENT2(x)) || x < 0 || x > gpGlobals->maxEntities)) \
	{ \
		MF_LogError(amx, AMX_ERR_NATIVE, "Invalid entity."); \
		return 0; \
	}\

#define CLIENT_PRINT							(*g_engfuncs.pfnClientPrintf)

#define m_flStartThrow							XTRA_OFS_WEAPON + 16
#define m_flReleaseThrow						XTRA_OFS_WEAPON + 17
#define m_chargeReady							XTRA_OFS_WEAPON + 18
#define m_fInAttack								XTRA_OFS_WEAPON + 19
#define m_fireState								XTRA_OFS_WEAPON + 20
#define m_pPlayer								XTRA_OFS_WEAPON + 28
#define m_iId									XTRA_OFS_WEAPON + 30
#define m_iPlayEmptySound						XTRA_OFS_WEAPON + 31
#define m_fFireOnEmpty							XTRA_OFS_WEAPON + 32
#define m_flPumpTime							XTRA_OFS_WEAPON + 33
#define m_fInSpecialReload						XTRA_OFS_WEAPON + 34
#define m_flNextPrimaryAttack					XTRA_OFS_WEAPON + 35
#define m_flNextSecondaryAttack					XTRA_OFS_WEAPON + 36
#define m_flTimeWeaponIdle						XTRA_OFS_WEAPON + 37
#define m_iPrimaryAmmoType						XTRA_OFS_WEAPON + 38
#define m_iSecondaryAmmoType					XTRA_OFS_WEAPON + 39
#define m_iClip									XTRA_OFS_WEAPON + 40
#define m_fInReload								XTRA_OFS_WEAPON + 43
#define m_iDefaultAmmo							XTRA_OFS_WEAPON + 44
#define m_LastHitGroup							XTRA_OFS_PLAYER + 90
#define m_flNextAttack							XTRA_OFS_PLAYER + 148
#define m_iWeaponVolume							XTRA_OFS_PLAYER + 173
#define m_iWeaponFlash							XTRA_OFS_PLAYER + 175
#define m_iFOV									XTRA_OFS_PLAYER + 298
#define m_rgAmmo								XTRA_OFS_PLAYER + 310
#define m_szAnimExtention						XTRA_OFS_PLAYER + 387

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

enum e_VirtFuncs
{
	VirtFunc_Classify,
	VirtFunc_TakeDamage,
	VirtFunc_BloodColor,
	VirtFunc_TraceBleed,
	VirtFunc_Think,
	VirtFunc_Touch,
	VirtFunc_Respawn,
	VirtFunc_AddToPlayer,
	VirtFunc_GetItemInfo,
	VirtFunc_CanDeploy,
	VirtFunc_Deploy,
	VirtFunc_CanHolster,
	VirtFunc_Holster,
	VirtFunc_ItemPostFrame,
	VirtFunc_Drop,
	VirtFunc_ItemSlot,
	VirtFunc_IsUseable,

	VirtFunc_End
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
	int iTrampoline;
	void *pOrigFunc;
} HookData;

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
	int iForward[Fwd_Wpn_End];
} AmmoBoxData;

typedef int (*FN_GetAmmoIndex)(const char *psz);
typedef void (*FN_PrecacheOtherWeapon)(const char *szClassname);
typedef void (*FN_RadiusDamage)(Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType);

extern void *pRadiusDamage;
extern void *pGetAmmoIndex;
extern void *pPlayerSetAnimation;
extern void *pPrecacheOtherWeapon;

extern int g_iWeaponIndex;
extern int g_iAmmoBoxIndex;
extern int VirtualFunction[VirtFunc_End];

extern CEntity g_EntData;
extern CVirtHook g_VirtHook_InfoTarget;
extern CVirtHook g_VirtHook_Crowbar;

extern WeaponData WeaponInfoArray[MAX_WEAPONS];
extern AmmoBoxData AmmoBoxInfoArray[MAX_WEAPONS];

extern AMX_NATIVE_INFO Natives_Ammo[];
extern AMX_NATIVE_INFO Natives_Weapon[];

extern unsigned char* hldll_base;

extern BOOL g_CrowbarHooksEnabled;
extern BOOL g_InfoTargetHooksEnabled;
extern BOOL g_InitWeapon;
extern BOOL g_initialized;
extern BOOL	g_IsBaseSet;

extern short g_sModelIndexBloodDrop;
extern short g_sModelIndexBloodSpray;
extern short g_sModelIndexFireball;
extern short g_sModelIndexWExplosion;

extern void MakeVirtualHooks(void);
extern void WpnModCommand(void);
extern void* FindFunction(char* sig_str, char* sig_mask, size_t sig_len);

extern BOOL FindDllBase(void* func);
extern int ParseBSPEntData(char *file);

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

inline edict_t* INDEXENT2( int iEdictNum )
{ 
	if (iEdictNum >= 1 && iEdictNum <= gpGlobals->maxClients)
		return MF_GetPlayerEdict(iEdictNum);
	else
		return (*g_engfuncs.pfnPEntityOfEntIndex)(iEdictNum); 
}

inline edict_t *GetPrivateCbase(edict_t *pEntity, int iOffset)
{
    void *pPrivate=*((void **)((int *)(edict_t *)(INDEXENT(0) + ENTINDEX(pEntity))->pvPrivateData + iOffset));

    if (!pPrivate)
    {
        return NULL;
    }

    return PrivateToEdict(pPrivate);	
}

inline int Player_AmmoInventory(edict_t* pPlayer, edict_t* pWeapon, BOOL bPrimary)
{
	int iAmmoIndex = (int)*((int *)pWeapon->pvPrivateData + (bPrimary ? m_iPrimaryAmmoType : m_iSecondaryAmmoType));

	if (iAmmoIndex == -1)
	{
		return -1;
	}

	return (int)*((int *)pPlayer->pvPrivateData + m_rgAmmo + iAmmoIndex - 1);
}

inline int Player_Set_AmmoInventory(edict_t* pPlayer, edict_t* pWeapon, BOOL bPrimary, int Amount)
{
	int iAmmoIndex = (int)*((int *)pWeapon->pvPrivateData + (bPrimary ? m_iPrimaryAmmoType : m_iSecondaryAmmoType));

	if (iAmmoIndex == -1)
	{
		return 0;
	}

	*((int *)pPlayer->pvPrivateData + m_rgAmmo + iAmmoIndex - 1) = Amount;

	return 1;
}

inline void print_srvconsole(char *fmt, ...)
{
	va_list argptr;
	static char string[384];
	va_start(argptr, fmt);
	vsnprintf(string, sizeof(string) - 1, fmt, argptr);
	string[sizeof(string) - 1] = '\0';
	va_end(argptr);
       
	SERVER_PRINT(string);
}

#ifdef _WIN32
	typedef void (*FN_SetAnimation)(void *Private, int i, PLAYER_ANIM playerAnim);

	inline int CLASSIFY(edict_t* pEntity)
	{
		return reinterpret_cast<int (__fastcall *)(void *, int)>((*((void***)((char*)pEntity->pvPrivateData)))[VirtualFunction[VirtFunc_Classify]])(pEntity->pvPrivateData, 0);
	}

	inline void TRACE_BLEED(edict_t* pEntity, float flDamage, Vector vecDir, TraceResult tr, int bitsDamageType)
	{
		reinterpret_cast<int (__fastcall *)(void *, int, float, Vector, TraceResult *, int)>((*((void***)((char*)pEntity->pvPrivateData)))[VirtualFunction[VirtFunc_TraceBleed]])(pEntity->pvPrivateData, 0, flDamage, vecDir, &tr, bitsDamageType);
	}

	inline int BLOOD_COLOR(edict_t* pEntity)
	{
		return reinterpret_cast<int (__fastcall *)(void *, int)>((*((void***)((char*)pEntity->pvPrivateData)))[VirtualFunction[VirtFunc_BloodColor]])(pEntity->pvPrivateData, 0);
	}

	inline void TAKE_DAMAGE(edict_t* pEntity, edict_t* pInflictor, edict_t* pAttacker, float flDamage, int bitsDamageType)
	{
		reinterpret_cast<int (__fastcall *)(void *, int, entvars_t *, entvars_t *, float, int)>((*((void***)((char*)pEntity->pvPrivateData)))[VirtualFunction[VirtFunc_TakeDamage]])(pEntity->pvPrivateData, 0, &(pInflictor->v), &(pAttacker->v), flDamage, bitsDamageType);
	}
#elif __linux__
	typedef void (*FN_SetAnimation)(void *Private, PLAYER_ANIM playerAnim);

	inline int CLASSIFY(edict_t* pEntity)
	{
		return reinterpret_cast<int (*)(void *)>((*((void***)(((char*)pEntity->pvPrivateData) + 0x60)))[VirtualFunction[VirtFunc_Classify]])(pEntity->pvPrivateData);
	}

	inline void TRACE_BLEED(edict_t* pEntity, float flDamage, Vector vecDir, TraceResult tr, int bitsDamageType)
	{
		reinterpret_cast<int (*)(void *, float, Vector, TraceResult *, int)>((*((void***)(((char*)tr.pHit->pvPrivateData) + 0x60)))[VirtualFunction[VirtFunc_TraceBleed]])(tr.pHit->pvPrivateData, flDamage, vecDir, &tr, bitsDamageType);
	}

	inline int BLOOD_COLOR(edict_t* pEntity)
	{
		return reinterpret_cast<int (*)(void *)>((*((void***)(((char*)pEntity->pvPrivateData) + 0x60)))[VirtualFunction[VirtFunc_BloodColor]])(pEntity->pvPrivateData);
	}

	inline void TAKE_DAMAGE(edict_t* pEntity, edict_t* pInflictor, edict_t* pAttacker, float flDamage, int bitsDamageType)
	{
		reinterpret_cast<int (*)(void *, entvars_t *, entvars_t *, float, int)>((*((void***)(((char*)pEntity->pvPrivateData) + 0x60)))[VirtualFunction[VirtFunc_TakeDamage]])(pEntity->pvPrivateData, &(pInflictor->v), &(pAttacker->v), flDamage, bitsDamageType);
	}
#endif