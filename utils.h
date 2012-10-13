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

#ifndef _UTILS_H
#define _UTILS_H

#include "amxxmodule.h"


#define CBTEXTURENAMEMAX		13			// only load first n chars of name

#define CHAR_TEX_CONCRETE		'C'			// texture types
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

#define CHECK_ENTITY(x) \
	if (x != 0 && (FNullEnt(INDEXENT2(x)) || x < 0 || x > gpGlobals->maxEntities)) \
	{ \
		MF_LogError(amx, AMX_ERR_NATIVE, "Invalid entity."); \
		return 0; \
	}\

#define IsValidPev(pEntity) (!FNullEnt(pEntity) && pEntity->pvPrivateData)

#ifdef _WIN32
#define PrivateToEdict(pPrivate) (*(entvars_t **)((char*)pPrivate + 4))->pContainingEntity
#else
#define PrivateToEdict(pPrivate) (*(entvars_t **)pPrivate)->pContainingEntity
#endif


extern edict_t* INDEXENT2(int iEdictNum);
extern edict_t *GetPrivateCbase(edict_t *pEntity, int iOffset);

extern int Player_AmmoInventory(edict_t* pPlayer, edict_t* pWeapon, BOOL bPrimary);
extern int Player_Set_AmmoInventory(edict_t* pPlayer, edict_t* pWeapon, BOOL bPrimary, int Amount);

extern const char* get_localinfo(const char* name, const char* def);

extern void print_srvconsole(char *fmt, ...);
extern void GiveNamedItem(edict_t *pPlayer, const char *szName);
extern void SendWeaponAnim(edict_t* pPlayer, edict_t* pWeapon, int iAnim);

extern void UTIL_EjectBrass(const Vector &vecOrigin, const Vector &vecVelocity, float rotation, int model, int soundtype);

extern void FireBulletsPlayer(edict_t* pPlayer, edict_t* pAttacker, int iShotsCount, Vector vecSpread, float flDistance, float flDamage, int bitsDamageType, BOOL bTracers);
extern void RadiusDamage2(Vector vecSrc, edict_t* pInflictor, edict_t* pAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType);

#endif // _UTILS_H
