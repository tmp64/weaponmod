/* AMX Mod X
*	Beam entities include by KORD_12.7.
*
* Version 1.2 (last update: 16, november (11), 2012)
*
* http://aghl.ru/forum/ - Russian Half-Life and Adrenaline Gamer Community
*
* This file is provided as is (no warranties)
*/

#if defined _beams_included
	#endinput
#endif
#define _beams_included

#include <fakemeta>
#include <xs>


// These functions are here to show the way beams are encoded as entities.
// Encoding beams as entities simplifies their management in the client/server architecture.


// Beam types
enum _:Beam_Types
{
	BEAM_POINTS,
	BEAM_ENTPOINT,
	BEAM_ENTS,
	BEAM_HOSE
};

// Beam flags
#define BEAM_FSINE	0x10
#define BEAM_FSOLID	0x20
#define BEAM_FSHADEIN	0x40
#define BEAM_FSHADEOUT	0x80


#define Beam_SetType(%0,%1) set_pev(%0, pev_rendermode, (pev(%0, pev_rendermode) & 0xF0) | %1 & 0x0F)
/* stock Beam_SetType(const iBeamEntity, const iType)
	return set_pev(iBeamEntity, pev_rendermode, (pev(iBeamEntity, pev_rendermode) & 0xF0) | iType & 0x0F); */
	
#define Beam_SetFlags(%0,%1) set_pev(%0, pev_rendermode, (pev(%0, pev_rendermode) & 0x0F) | %1 & 0xF0)
/* stock Beam_SetFlags(const iBeamEntity, const iType)
	return set_pev(iBeamEntity, pev_rendermode, (pev(iBeamEntity, pev_rendermode) & 0x0F) | iType & 0xF0); */	

#define Beam_SetStartPos(%0,%1) set_pev(%0, pev_origin, %1)
/* stock Beam_SetStartPos(const iBeamEntity, const Float: flVecStart[3])
	return set_pev(iBeamEntity, pev_origin, flVecStart); */

#define Beam_SetEndPos(%0,%1) set_pev(%0, pev_angles, %1)
/* stock Beam_SetEndPos(const iBeamEntity, const Float: flVecEnd[3]) 
	return set_pev(iBeamEntity, pev_angles, flVecEnd); */

#define Beam_SetStartEntity(%0,%1) \
	set_pev(%0, pev_sequence, (%1 & 0x0FFF) | ((pev(%0, pev_sequence) & 0xF000) << 12)); \
	set_pev(%0, pev_owner, %1) \
/* stock Beam_SetStartEntity(const iBeamEntity, const iEntityIndex) */

#define Beam_SetEndEntity(%0,%1) \
	set_pev(%0, pev_skin, (%1 & 0x0FFF) | ((pev(%0, pev_skin) & 0xF000) << 12)); \
	set_pev(%0, pev_aiment, %1) \
/* stock Beam_SetEndEntity(const iBeamEntity, const iEntityIndex) */

#define Beam_SetStartAttachment(%0,%1) set_pev(%0, pev_sequence, (pev(%0, pev_sequence) & 0x0FFF) | ((%1 & 0xF) << 12))
/* stock Beam_SetStartAttachment(const iBeamEntity, const iAttachment)
	return set_pev(iBeamEntity, pev_sequence, (pev(iBeamEntity, pev_sequence) & 0x0FFF) | ((iAttachment & 0xF) << 12)); */

#define Beam_SetEndAttachment(%0,%1) set_pev(%0, pev_skin, (pev(%0, pev_skin) & 0x0FFF) | ((%1 & 0xF) << 12))
/* stock Beam_SetEndAttachment(const iBeamEntity, const iAttachment)
	return set_pev(iBeamEntity, pev_skin, (pev(iBeamEntity, pev_skin) & 0x0FFF) | ((iAttachment & 0xF) << 12)); */

#define Beam_SetTexture(%0,%1) set_pev(%0, pev_modelindex, %1)
/* stock Beam_SetTexture(const iBeamEntity, const iSpriteIndex)
	return set_pev(iBeamEntity, pev_modelindex, iSpriteIndex); */

#define Beam_SetWidth(%0,%1) set_pev(%0, pev_scale, %1)
/* stock Beam_SetWidth(const iBeamEntity, const Float: flWidth)
	return set_pev(iBeamEntity, pev_scale, flWidth); */

#define Beam_SetNoise(%0,%1) set_pev(%0, pev_body, %1)
/* stock Beam_SetNoise(const iBeamEntity, const iNoise)
	return set_pev(iBeamEntity, pev_body, iNoise); */	
	
#define Beam_SetColor(%0,%1) set_pev(%0, pev_rendercolor, %1)
/* stock Beam_SetColor(const iBeamEntity, const Float: flColor[3])
	return set_pev(iBeamEntity, pev_rendercolor, flColor); */	
		
#define Beam_SetBrightness(%0,%1) set_pev(%0, pev_renderamt, %1)
/* stock Beam_SetBrightness(const iBeamEntity, const Float: flBrightness)
	return set_pev(iBeamEntity, pev_renderamt, flBrightness); */

#define Beam_SetFrame(%0,%1) set_pev(%0, pev_frame, %1)
/* stock Beam_SetFrame(const iBeamEntity, const Float: flFrame)
	return set_pev(iBeamEntity, pev_frame, flFrame); */

#define Beam_SetScrollRate(%0,%1) set_pev(%0, pev_animtime, %1)
/* stock Beam_SetScrollRate(const iBeamEntity, const Float: flSpeed)
	return set_pev(iBeamEntity, pev_animtime, flSpeed); */

#define Beam_GetType(%0) (pev(%0, pev_rendermode) & 0x0F)
/* stock Beam_GetType(const iBeamEntity)
	return pev(iBeamEntity, pev_rendermode) & 0x0F; */
	
#define Beam_GetFlags(%0) (pev(%0, pev_rendermode) & 0xF0)
/* stock Beam_GetFlags(const iBeamEntity)
	return pev(iBeamEntity, pev_rendermode) & 0xF0; */	

#define Beam_GetStartEntity(%0) (pev(%0, pev_sequence) & 0xFFF)
/* stock Beam_GetStartEntity(const iBeamEntity)
	return pev(iBeamEntity, pev_sequence) & 0xFFF; */

#define Beam_GetEndEntity(%0) (pev(%0, pev_skin) & 0xFFF)
/* stock Beam_GetEndEntity(const iBeamEntity)
	return pev(iBeamEntity, pev_skin) & 0xFFF; */

stock Beam_GetStartPos(const iBeamEntity, Float: flStartPos[3])
{
	if (Beam_GetType(iBeamEntity) == BEAM_ENTS)
	{
		new iEntity = Beam_GetStartEntity(iBeamEntity);
		
		if (pev_valid(iEntity))
		{
			pev(iEntity, pev_origin, flStartPos);
			return;
		}
	}
	
	pev(iBeamEntity, pev_origin, flStartPos);
}

stock Beam_GetEndPos(const iBeamEntity, Float: flEndPos[3])
{
	new iType = Beam_GetType(iBeamEntity);
	
	if (iType == BEAM_POINTS || iType == BEAM_HOSE)
	{
		pev(iBeamEntity, pev_angles, flEndPos);
		return;
	}
	
	new iEntity = Beam_GetEndEntity(iBeamEntity);
	
	if (pev_valid(iEntity))
	{
		pev(iEntity, pev_origin, flEndPos);
		return;
	}
	
	pev(iBeamEntity, pev_angles, flEndPos);
}

#define Beam_GetTexture(%0) pev(%0, pev_modelindex)
/* stock Beam_GetTexture(const iBeamEntity)
	return pev(iBeamEntity, pev_modelindex); */
	
stock Float: Beam_GetWidth(const iBeamEntity)
{
	new Float: flScale;
	pev(iBeamEntity, pev_scale, flScale);
	
	return flScale;
}

#define Beam_GetNoise(%0) pev(%0, pev_body)
/* stock Beam_GetNoise(const iBeamEntity)
	return pev(iBeamEntity, pev_body); */
	
#define Beam_GetColor(%0,%1) pev(%0, pev_rendercolor,%1)
/* stock Beam_GetColor(const iBeamEntity, Float: flRGB[3])
	return pev(iBeamEntity, pev_rendercolor, flRGB); */

stock Float: Beam_GetBrightness(const iBeamEntity)
{
	new Float: flBrightness;
	pev(iBeamEntity, pev_renderamt, flBrightness);
	
	return flBrightness;
}

stock Float: Beam_GetFrame(const iBeamEntity)
{
	new Float: flFrame;
	pev(iBeamEntity, pev_frame, flFrame);
	
	return flFrame;
}

stock Float: Beam_GetScrollRate(const iBeamEntity)
{
	new Float: flScrollRate;
	pev(iBeamEntity, pev_animtime, flScrollRate);
	
	return flScrollRate;
}

stock Beam_Create(const szSpriteName[], const Float: flWidth)
{
	new iBeamEntity;
	static iszAllocStringCached;
	
	if (iszAllocStringCached || (iszAllocStringCached = engfunc(EngFunc_AllocString, "beam")))
	{
		iBeamEntity = engfunc(EngFunc_CreateNamedEntity, iszAllocStringCached);
	}
	
	if (pev_valid(iBeamEntity))
	{
		set_pev(iBeamEntity, pev_classname, "beam");
		Beam_Init(iBeamEntity, szSpriteName, flWidth);
		
		return iBeamEntity;
	}
	
	return FM_NULLENT;
}

stock Beam_Init(const iBeamEntity, const szSpriteName[], const Float: flWidth)
{
	set_pev(iBeamEntity, pev_flags, pev(iBeamEntity, pev_flags) | FL_CUSTOMENTITY);
	
	Beam_SetColor(iBeamEntity, Float: {255.0, 255.0, 255.0});
	Beam_SetBrightness(iBeamEntity, 255.0);
	Beam_SetNoise(iBeamEntity, 0);
	Beam_SetFrame(iBeamEntity, 0.0);
	Beam_SetScrollRate(iBeamEntity, 0.0);
	
	set_pev(iBeamEntity, pev_model, szSpriteName);
	
	Beam_SetTexture(iBeamEntity, engfunc(EngFunc_ModelIndex, szSpriteName));
	Beam_SetWidth(iBeamEntity, flWidth);
	
	set_pev(iBeamEntity, pev_skin, 0);
	set_pev(iBeamEntity, pev_sequence, 0);
	set_pev(iBeamEntity, pev_rendermode, 0);
}

stock Beam_PointsInit(const iBeamEntity, const Float: flVecStart[3], const Float: flVecEnd[3])
{
	Beam_SetType(iBeamEntity, BEAM_POINTS);
	Beam_SetStartPos(iBeamEntity, flVecStart);
	Beam_SetEndPos(iBeamEntity, flVecEnd);
	Beam_SetStartAttachment(iBeamEntity, 0);
	Beam_SetEndAttachment(iBeamEntity, 0);
	Beam_RelinkBeam(iBeamEntity);
}

stock Beam_HoseInit(const iBeamEntity, const Float: flVecStart[3], const Float: flVecDirection[3])
{
	Beam_SetType(iBeamEntity, BEAM_HOSE);
	Beam_SetStartPos(iBeamEntity, flVecStart);
	Beam_SetEndPos(iBeamEntity, flVecDirection);
	Beam_SetStartAttachment(iBeamEntity, 0);
	Beam_SetEndAttachment(iBeamEntity, 0);
	Beam_RelinkBeam(iBeamEntity);
}

stock Beam_PointEntInit(const iBeamEntity, const Float: flVecStart[3], const iEndIndex)
{
	Beam_SetType(iBeamEntity, BEAM_ENTPOINT);
	Beam_SetStartPos(iBeamEntity, flVecStart);
	Beam_SetEndEntity(iBeamEntity, iEndIndex);
	Beam_SetStartAttachment(iBeamEntity, 0);
	Beam_SetEndAttachment(iBeamEntity, 0);
	Beam_RelinkBeam(iBeamEntity);
}

stock Beam_EntsInit(const iBeamEntity, const iStartIndex, const iEndIndex)
{
	Beam_SetType(iBeamEntity, BEAM_ENTS);
	Beam_SetStartEntity(iBeamEntity, iStartIndex);
	Beam_SetEndEntity(iBeamEntity, iEndIndex);
	Beam_SetStartAttachment(iBeamEntity, 0);
	Beam_SetEndAttachment(iBeamEntity, 0);
	Beam_RelinkBeam(iBeamEntity);
}

stock Beam_RelinkBeam(const iBeamEntity)
{
	new Float: flOrigin[3];
	new Float: flStartPos[3];
	new Float: flEndPos[3];
	new Float: flMins[3];
	new Float: flMaxs[3];
	
	pev(iBeamEntity, pev_origin, flOrigin);
	
	Beam_GetStartPos(iBeamEntity, flStartPos);
	Beam_GetEndPos(iBeamEntity, flEndPos);
	
	flMins[0] = floatmin(flStartPos[0], flEndPos[0]);
	flMins[1] = floatmin(flStartPos[1], flEndPos[1]);
	flMins[2] = floatmin(flStartPos[2], flEndPos[2]);
	
	flMaxs[0] = floatmax(flStartPos[0], flEndPos[0]);
	flMaxs[1] = floatmax(flStartPos[1], flEndPos[1]);
	flMaxs[2] = floatmax(flStartPos[2], flEndPos[2]);
	
	xs_vec_sub(flMins, flOrigin, flMins);
	xs_vec_sub(flMaxs, flOrigin, flMaxs);
	
	set_pev(iBeamEntity, pev_mins, flMins);
	set_pev(iBeamEntity, pev_maxs, flMaxs);
	
	engfunc(EngFunc_SetSize, iBeamEntity, flMins, flMaxs);
	engfunc(EngFunc_SetOrigin, iBeamEntity, flOrigin);
}
