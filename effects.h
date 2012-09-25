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

#ifndef _EFFECTS_H
#define _EFFECTS_H

#include <extdll.h>
#include <meta_api.h>


#define SF_BEAM_STARTON			0x0001
#define SF_BEAM_TOGGLE			0x0002
#define SF_BEAM_RANDOM			0x0004
#define SF_BEAM_RING			0x0008
#define SF_BEAM_SPARKSTART		0x0010
#define SF_BEAM_SPARKEND		0x0020
#define SF_BEAM_DECALS			0x0040
#define SF_BEAM_SHADEIN			0x0080
#define SF_BEAM_SHADEOUT		0x0100
#define SF_BEAM_TEMPORARY		0x8000

#define SF_SPRITE_STARTON		0x0001
#define SF_SPRITE_ONCE			0x0002
#define SF_SPRITE_TEMPORARY		0x8000

// Beam types, encoded as a byte
enum 
{
	BEAM_POINTS = 0,
	BEAM_ENTPOINT,
	BEAM_ENTS,
	BEAM_HOSE,
};

inline void	Beam_SetType(edict_t* pBeam, int type) { pBeam->v.rendermode = (pBeam->v.rendermode & 0xF0) | (type&0x0F); }
inline void	Beam_SetFlags(edict_t* pBeam, int flags) { pBeam->v.rendermode = (pBeam->v.rendermode & 0x0F) | (flags&0xF0); }
inline void Beam_SetStartPos(edict_t* pBeam, const Vector& pos) { pBeam->v.origin = pos; }
inline void Beam_SetEndPos(edict_t* pBeam, const Vector& pos) { pBeam->v.angles = pos; }

void Beam_SetStartEntity(edict_t* pBeam, int entityIndex);
void Beam_SetEndEntity(edict_t* pBeam, int entityIndex);

inline void Beam_SetStartAttachment(edict_t* pBeam, int attachment) { pBeam->v.sequence = (pBeam->v.sequence & 0x0FFF) | ((attachment&0xF)<<12); }
inline void Beam_SetEndAttachment(edict_t* pBeam, int attachment) { pBeam->v.skin = (pBeam->v.skin & 0x0FFF) | ((attachment&0xF)<<12); }

inline void Beam_SetTexture(edict_t* pBeam, int spriteIndex) { pBeam->v.modelindex = spriteIndex; }
inline void Beam_SetWidth(edict_t* pBeam, int width) { pBeam->v.scale = width; }
inline void Beam_SetNoise(edict_t* pBeam, int amplitude) { pBeam->v.body = amplitude; }
inline void Beam_SetColor(edict_t* pBeam, int r, int g, int b) { pBeam->v.rendercolor.x = r; pBeam->v.rendercolor.y = g; pBeam->v.rendercolor.z = b; }
inline void Beam_SetBrightness(edict_t* pBeam, int brightness) { pBeam->v.renderamt = brightness; }
inline void Beam_SetFrame(edict_t* pBeam, float frame) { pBeam->v.frame = frame; }
inline void Beam_SetScrollRate(edict_t* pBeam, int speed) { pBeam->v.animtime = speed; }

inline int Beam_GetType(edict_t* pBeam) { return pBeam->v.rendermode & 0x0F; }
inline int Beam_GetFlags(edict_t* pBeam) { return pBeam->v.rendermode & 0xF0; }
inline int Beam_GetStartEntity(edict_t* pBeam) { return pBeam->v.sequence & 0xFFF; }
inline int Beam_GetEndEntity(edict_t* pBeam) { return pBeam->v.skin & 0xFFF; }

const Vector &Beam_GetStartPos(edict_t* pBeam);
const Vector &Beam_GetEndPos(edict_t* pBeam);

inline Vector Beam_Center(edict_t* pBeam) { return (Beam_GetStartPos(pBeam) + Beam_GetEndPos(pBeam)) * 0.5; }; // center point of beam

inline int Beam_GetTexture(edict_t* pBeam) { return pBeam->v.modelindex; }
inline int Beam_GetWidth(edict_t* pBeam) { return pBeam->v.scale; }
inline int Beam_GetNoise(edict_t* pBeam) { return pBeam->v.body; }
inline int Beam_GetBrightness(edict_t* pBeam) { return pBeam->v.renderamt; }
inline int Beam_GetFrame(edict_t* pBeam) { return pBeam->v.frame; }
inline int Beam_GetScrollRate(edict_t* pBeam) { return pBeam->v.animtime; }

void Beam_RelinkBeam(edict_t* pBeam);

void BeamInit(edict_t* pBeam, const char *pSpriteName, int width);
void Beam_PointsInit(edict_t* pBeam, const Vector &start, const Vector &end);
void Beam_PointEntInit(edict_t* pBeam, const Vector &start, int endIndex);
void Beam_EntsInit(edict_t* pBeam, int startIndex, int endIndex);
void Beam_HoseInit(edict_t* pBeam, const Vector &start, const Vector &direction);

edict_t* Beam_Create( const char *pSpriteName, int width);

// inline void LiveForTime( float time ) { SetThink(&CSprite::SUB_Remove); pev->nextthink = gpGlobals->time + time; }


#endif // _EFFECTS_H
