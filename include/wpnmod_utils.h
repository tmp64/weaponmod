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

#ifndef _UTILS_H
#define _UTILS_H

#include "amxxmodule.h"

#include "wpnmod_vtable.h"
#include "wpnmod_pvdata.h"


#define IsValidPev(entity)	(!FNullEnt(entity) && entity->pvPrivateData)

#define CBTEXTURENAMEMAX		13

#define CHAR_TEX_CONCRETE		'C'
#define CHAR_TEX_METAL			'M'
#define CHAR_TEX_DIRT			'D'
#define CHAR_TEX_VENT			'V'
#define CHAR_TEX_GRATE			'G'
#define CHAR_TEX_TILE			'T'
#define CHAR_TEX_SLOSH			'S'
#define CHAR_TEX_WOOD			'W'
#define CHAR_TEX_COMPUTER		'P'
#define CHAR_TEX_GLASS			'Y'
#define CHAR_TEX_FLESH			'F'

#define CLIENT_PRINT (*g_engfuncs.pfnClientPrintf)

inline int PrimaryAmmoIndex(edict_t *pEntity)
{
	return GetPrivateInt(pEntity, pvData_iPrimaryAmmoType);
}

inline int SecondaryAmmoIndex(edict_t *pEntity)
{
	return GetPrivateInt(pEntity, pvData_iSecondaryAmmoType);
}

extern edict_t*	INDEXENT2			(int iEdictNum);
extern edict_t* GiveNamedItem		(edict_t* pPlayer, const char* szName);

extern	bool	SwitchWeapon		(edict_t* pPlayer, edict_t* pWeapon);
extern	bool	Entity_IsInWorld	(edict_t* pEntity);
extern	bool	GetNextBestWeapon	(edict_t* pPlayer, edict_t* pCurrentWeapon);

extern	void	SelectItem			(edict_t* pPlayer, const char* pstr);
extern	void	RadiusDamage2		(Vector vecSrc, edict_t* pInflictor, edict_t* pAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType);
extern	void	SendWeaponAnim		(edict_t* pPlayer, edict_t* pWeapon, int iAnim);
extern	void	SelectLastItem		(edict_t* pPlayer);
extern	void	UTIL_EjectBrass		(const Vector &vecOrigin, const Vector &vecVelocity, float rotation, int model, int soundtype);
extern	void	UTIL_RemoveEntity	(edict_t* pEntity);
extern	void	UTIL_DecalGunshot	(TraceResult* pTrace);
extern	void	FireBulletsPlayer	(edict_t* pPlayer, edict_t* pAttacker, int iShotsCount, Vector vecSpread, float flDistance, float flDamage, int bitsDamageType, BOOL bTracers);

extern	int		GetAmmoInventory	(edict_t* pPlayer, int iAmmoIndex);
extern	int		SetAmmoInventory	(edict_t* pPlayer, int iAmmoIndex, int iAmount);

extern	bool	FileExists			(const char *dir);


#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#endif // _UTILS_H
