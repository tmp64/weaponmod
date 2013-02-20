/*
 * Half-Life Weapon Mod
 * Copyright (c) 2012 - 2013 AGHL.RU Dev Team
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

#ifndef _GREN_H
#define _GREN_H

#include "amxxmodule.h"
#include "wpnmod_hooks.h"
#include "wpnmod_utils.h"

// Grenade explosion flags.
#define SF_EXPLOSION_NODAMAGE	( 1 << 0 ) // when set, explosion will not actually inflict damage
#define SF_EXPLOSION_NOFIREBALL	( 1 << 1 ) // don't draw the fireball
#define SF_EXPLOSION_NOSMOKE	( 1 << 2 ) // don't draw the smoke
#define SF_EXPLOSION_NODECAL	( 1 << 3 ) // don't make a scorch mark
#define SF_EXPLOSION_NOSPARKS	( 1 << 4 ) // don't make a sparks
#define SF_EXPLOSION_NODEBRIS	( 1 << 5 ) // don't make a debris sound

#define SF_EXPLOSION_NOEFFECTS	( ~(1 << SF_EXPLOSION_NODEBRIS) ) // don't make any effects


#ifdef _WIN32
	extern void __fastcall Grenade_ThinkSmoke(void *pPrivate);
	extern void __fastcall Grenade_TumbleThink(void *pPrivate);
	extern void __fastcall Grenade_ThinkBeforeContact(void *pPrivate);
	extern void __fastcall Grenade_BounceTouch(void *pPrivate, int i, void *pPrivate2);
	extern void __fastcall Grenade_ExplodeTouch(void *pPrivate, int i, void *pPrivate2);
#else
	extern void Grenade_ThinkSmoke(void *pPrivate);
	extern void Grenade_TumbleThink(void *pPrivate);
	extern void Grenade_ThinkBeforeContact(void *pPrivate);
	extern void Grenade_BounceTouch(void *pPrivate, void *pPrivate2);
	extern void Grenade_ExplodeTouch(void *pPrivate, void *pPrivate2);
#endif

extern void Grenade_Explode(edict_t* pGrenade, int bitsDamageType);

extern edict_t* Grenade_ShootContact(edict_t *pOwner, Vector vecStart, Vector vecVelocity);
extern edict_t* Grenade_ShootTimed(edict_t *pOwner, Vector vecStart, Vector vecVelocity, float flTime);

#endif // _GREN_H
