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

#include "effects.h"


edict_t* Beam_Create( const char *pSpriteName, int width)
{
	edict_t* pBeam = NULL;

	static int iszAllocStringCached;

	if (iszAllocStringCached || (iszAllocStringCached = MAKE_STRING("beam")))
	{
		pBeam = CREATE_NAMED_ENTITY(iszAllocStringCached);
	}

	if (!FNullEnt(pBeam) && pBeam->pvPrivateData)
	{
		BeamInit(pBeam, pSpriteName, width);
	}

	return pBeam;
}

void BeamInit(edict_t* pBeam, const char *pSpriteName, int width)
{
	pBeam->v.flags |= FL_CUSTOMENTITY;
	Beam_SetColor(pBeam, 255, 255, 255);
	Beam_SetBrightness(pBeam, 255);
	Beam_SetNoise(pBeam, 0);
	Beam_SetFrame(pBeam, 0);
	Beam_SetScrollRate(pBeam, 0);
	pBeam->v.model = MAKE_STRING(pSpriteName);
	Beam_SetTexture(pBeam, MODEL_INDEX((char *)pSpriteName));
	Beam_SetWidth(pBeam, width);
	pBeam->v.skin = 0;
	pBeam->v.sequence = 0;
	pBeam->v.rendermode = 0;
}

void Beam_PointsInit(edict_t* pBeam, const Vector &start, const Vector &end)
{
	Beam_SetType(pBeam, BEAM_POINTS);
	Beam_SetStartPos(pBeam, start);
	Beam_SetEndPos(pBeam, end);
	Beam_SetStartAttachment(pBeam, 0);
	Beam_SetEndAttachment(pBeam, 0);
	Beam_RelinkBeam(pBeam);
}

void Beam_PointEntInit(edict_t* pBeam, const Vector &start, int endIndex)
{
	Beam_SetType(pBeam, BEAM_ENTPOINT);
	Beam_SetStartPos(pBeam, start);
	Beam_SetEndEntity(pBeam, endIndex);
	Beam_SetStartAttachment(pBeam, 0);
	Beam_SetEndAttachment(pBeam, 0);
	Beam_RelinkBeam(pBeam);
}

void Beam_EntsInit(edict_t* pBeam, int startIndex, int endIndex)
{
	Beam_SetType(pBeam, BEAM_ENTS);
	Beam_SetStartEntity(pBeam, startIndex);
	Beam_SetEndEntity(pBeam, endIndex);
	Beam_SetStartAttachment(pBeam, 0);
	Beam_SetEndAttachment(pBeam, 0);
	Beam_RelinkBeam(pBeam);
}

void Beam_HoseInit(edict_t* pBeam, const Vector &start, const Vector &direction)
{
	Beam_SetType(pBeam, BEAM_HOSE);
	Beam_SetStartPos(pBeam, start);
	Beam_SetEndPos(pBeam, direction);
	Beam_SetStartAttachment(pBeam, 0);
	Beam_SetEndAttachment(pBeam, 0);
	Beam_RelinkBeam(pBeam);
}

void Beam_SetStartEntity(edict_t* pBeam, int entityIndex) 
{ 
	pBeam->v.sequence = (entityIndex & 0x0FFF) | ((pBeam->v.sequence&0xF000)<<12); 
	pBeam->v.owner = g_engfuncs.pfnPEntityOfEntIndex( entityIndex );
}

void Beam_SetEndEntity(edict_t* pBeam, int entityIndex) 
{ 
	pBeam->v.skin = (entityIndex & 0x0FFF) | ((pBeam->v.skin&0xF000)<<12); 
	pBeam->v.aiment = g_engfuncs.pfnPEntityOfEntIndex( entityIndex );
}

void Beam_RelinkBeam(edict_t* pBeam)
{
	const Vector &startPos = Beam_GetStartPos(pBeam), &endPos = Beam_GetEndPos(pBeam);

	pBeam->v.mins.x = min(startPos.x, endPos.x);
	pBeam->v.mins.y = min(startPos.y, endPos.y);
	pBeam->v.mins.z = min(startPos.z, endPos.z);
	pBeam->v.maxs.x = max(startPos.x, endPos.x);
	pBeam->v.maxs.y = max(startPos.y, endPos.y);
	pBeam->v.maxs.z = max(startPos.z, endPos.z);
	pBeam->v.mins = pBeam->v.mins - pBeam->v.origin;
	pBeam->v.maxs = pBeam->v.maxs - pBeam->v.origin;

	SET_SIZE(pBeam, pBeam->v.mins, pBeam->v.maxs);
	SET_ORIGIN(pBeam, pBeam->v.origin);
}

const Vector &Beam_GetStartPos(edict_t* pBeam)
{
	if (Beam_GetType(pBeam) == BEAM_ENTS)
	{
		edict_t *pent =  g_engfuncs.pfnPEntityOfEntIndex(Beam_GetStartEntity(pBeam));
		return pent->v.origin;
	}
	return pBeam->v.origin;
}

const Vector &Beam_GetEndPos(edict_t* pBeam)
{
	int type = Beam_GetType(pBeam);
	if (type == BEAM_POINTS || type == BEAM_HOSE)
	{
		return pBeam->v.angles;
	}

	edict_t *pent =  g_engfuncs.pfnPEntityOfEntIndex(Beam_GetEndEntity(pBeam));
	if ( pent )
		return pent->v.origin;
	return pBeam->v.angles;
}
