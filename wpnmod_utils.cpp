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

#include "wpnmod_hooks.h"
#include "wpnmod_utils.h"


#ifdef __linux__

bool g_ExtraThink = false;
bool g_ExtraTouch = false;

#endif

int g_vtblOffsets[VO_End];
int g_pvDataOffsets[pvData_End];

//bool g_bIsShieldWeaponLoaded = false;

edict_t *GetPrivateCbase(edict_t *pEntity, int iOffset)
{
	void *pPrivate = *((void **)((int *)(edict_t *)(INDEXENT(0) + ENTINDEX(pEntity))->pvPrivateData + g_pvDataOffsets[iOffset]));

	if (!pPrivate)
	{
		return NULL;
	}

	return PrivateToEdict(pPrivate);
}

edict_t *GetPrivateCbase(edict_t *pEntity, int iOffset, int iExtraRealOffset)
{
	void *pPrivate = *((void **)((int *)(edict_t *)(INDEXENT(0) + ENTINDEX(pEntity))->pvPrivateData + g_pvDataOffsets[iOffset] + iExtraRealOffset));

	if (!pPrivate)
	{
		return NULL;
	}

	return PrivateToEdict(pPrivate);
}

void SetPrivateCbase(edict_t *pEntity, int iOffset, edict_t* pValue)
{
	*((void**)((int*)(edict_t*)(INDEXENT(0) + ENTINDEX(pEntity))->pvPrivateData + g_pvDataOffsets[iOffset])) = (INDEXENT(0) + ENTINDEX(pValue))->pvPrivateData;
}

void SetPrivateCbase(edict_t *pEntity, int iOffset, edict_t* pValue, int iExtraRealOffset)
{
	*((void**)((int*)(edict_t*)(INDEXENT(0) + ENTINDEX(pEntity))->pvPrivateData + g_pvDataOffsets[iOffset] + iExtraRealOffset)) = (INDEXENT(0) + ENTINDEX(pValue))->pvPrivateData;
}

int PrimaryAmmoIndex(edict_t *pEntity)
{
	return GetPrivateInt(pEntity, pvData_iPrimaryAmmoType);
}

int SecondaryAmmoIndex(edict_t *pEntity)
{
	return GetPrivateInt(pEntity, pvData_iSecondaryAmmoType);
}

int GetAmmoInventory(edict_t* pPlayer, int iAmmoIndex)
{
	if (!IsValidPev(pPlayer) || iAmmoIndex == -1)
	{
		return -1;
	}

	return GetPrivateInt(pPlayer, pvData_rgAmmo, iAmmoIndex - 1);
}

int SetAmmoInventory(edict_t* pPlayer, int iAmmoIndex, int iAmount)
{
	if (!IsValidPev(pPlayer) || iAmmoIndex == -1)
	{
		return 0;
	}

	SetPrivateInt(pPlayer, pvData_rgAmmo, iAmount, iAmmoIndex - 1);
	return 1;
}

edict_t* INDEXENT2(int iEdictNum)
{ 
	if (iEdictNum >= 1 && iEdictNum <= gpGlobals->maxClients)
	{
		return MF_GetPlayerEdict(iEdictNum);
	}

	return (*g_engfuncs.pfnPEntityOfEntIndex)(iEdictNum); 
}

BOOL SwitchWeapon(edict_t* pPlayer, edict_t* pWeapon) 
{
	if (!IsValidPev(pWeapon) || !CAN_DEPLOY(pWeapon))
	{
		return FALSE;
	}

	edict_t* pActiveItem = GetPrivateCbase(pPlayer, pvData_pActiveItem);

	if (pActiveItem == pWeapon)
	{
		return FALSE;
	}

	if (IsValidPev(pActiveItem))
	{
		HOLSTER(pActiveItem);
	}

	SetPrivateCbase(pPlayer, pvData_pLastItem, pActiveItem);
	SetPrivateCbase(pPlayer, pvData_pActiveItem, pWeapon);

	DEPLOY(pWeapon);
	return TRUE;
}

void SelectLastItem(edict_t *pPlayer)
{
	edict_t* pLastItem = GetPrivateCbase(pPlayer, pvData_pLastItem);

	if (!IsValidPev(pLastItem))
	{
		return;
	}

	SwitchWeapon(pPlayer, pLastItem);
}

void SelectItem(edict_t *pPlayer, const char *pstr)
{
	if (!IsValidPev(pPlayer) || !pstr)
	{
		return;
	}

	edict_t *pCheck = NULL;

	for (int i = 0 ; i <= g_iMaxWeaponSlots ; i++)
	{
		pCheck = GetPrivateCbase(pPlayer, pvData_rgpPlayerItems, i);

		while (IsValidPev(pCheck))
		{
			if (!strcmp(STRING(pCheck->v.classname), pstr))
			{
				SwitchWeapon(pPlayer, pCheck);
				return;
			}

			pCheck = GetPrivateCbase(pCheck, pvData_pNext);
		}
	}
}

BOOL GetNextBestWeapon(edict_t* pPlayer, edict_t* pCurrentWeapon)
{
	edict_t* pBest= NULL;
	edict_t* pCheck = NULL;

	ItemInfo pII_Check;
	ItemInfo pII_Current;

	int iBestWeight = -1;

	GET_ITEM_INFO(pCurrentWeapon, &pII_Current);

	if (!CAN_HOLSTER(pCurrentWeapon))
	{
		return FALSE;
	}

	for (int i = 0 ; i <= g_iMaxWeaponSlots ; i++)
	{
		pCheck = GetPrivateCbase(pPlayer, pvData_rgpPlayerItems, i);

		while (IsValidPev(pCheck))
		{
			GET_ITEM_INFO(pCheck, &pII_Check);

			if (pII_Check.iWeight > -1 && pII_Check.iWeight == pII_Current.iWeight && pCheck != pCurrentWeapon)
			{
				if (CAN_DEPLOY(pCheck))
				{
					if (SwitchWeapon(pPlayer, pCheck))
					{
						return TRUE;
					}
				}
			}
			else if (pII_Check.iWeight > iBestWeight && pCheck != pCurrentWeapon)
			{
				if (CAN_DEPLOY(pCheck))
				{
					iBestWeight = pII_Check.iWeight;
					pBest = pCheck;
				}
			}

			pCheck = GetPrivateCbase(pCheck, pvData_pNext);
		}
	}

	if (!pBest)
	{
		return FALSE;
	}

	SwitchWeapon(pPlayer, pBest);
	return TRUE;
}

void SendWeaponAnim(edict_t* pPlayer, edict_t* pWeapon, int iAnim)
{
	#define OBS_IN_EYE 4

	pPlayer->v.weaponanim = iAnim;

	MESSAGE_BEGIN(MSG_ONE, SVC_WEAPONANIM, NULL, pPlayer);
	WRITE_BYTE(iAnim);
	WRITE_BYTE(pWeapon->v.body);
	MESSAGE_END();

	edict_t* pSpectator = NULL;

	// Also send anim to all spectators.
	for (int i = 0; i <= gpGlobals->maxClients; i++)
	{
		if (!MF_IsPlayerIngame(i))
		{
			continue;
		}

		pSpectator = INDEXENT2(i);

		if (pSpectator->v.iuser1 == OBS_IN_EYE && INDEXENT2(pSpectator->v.iuser2) == pPlayer)
		{
			pSpectator->v.weaponanim = iAnim;

			MESSAGE_BEGIN(MSG_ONE, SVC_WEAPONANIM, NULL, pSpectator);
			WRITE_BYTE(iAnim);
			WRITE_BYTE(pWeapon->v.body);
			MESSAGE_END();
		}
	}
}

void GiveNamedItem(edict_t *pPlayer, const char *szName)
{
	edict_t *pItem = Weapon_Spawn(szName, pPlayer->v.origin, Vector (0, 0, 0));

	if (pItem == NULL)
	{
		pItem = Ammo_Spawn(szName, pPlayer->v.origin, Vector (0, 0, 0));
	}

	if (IsValidPev(pItem))
	{
		pItem->v.spawnflags |= SF_NORESPAWN;
		MDLL_Touch(pItem, (edict_t *)pPlayer);

		if (pItem->v.modelindex)
		{
			REMOVE_ENTITY(pItem);
		}
	}
}

BOOL Entity_IsInWorld(edict_t *pEntity)
{
	// position 
	if (pEntity->v.origin.x >= 4096) return FALSE;
	if (pEntity->v.origin.y >= 4096) return FALSE;
	if (pEntity->v.origin.z >= 4096) return FALSE;
	if (pEntity->v.origin.x <= -4096) return FALSE;
	if (pEntity->v.origin.y <= -4096) return FALSE;
	if (pEntity->v.origin.z <= -4096) return FALSE;
	/*
	float flMaxVelocity = CVAR_GET_FLOAT("sv_maxvelocity");

	// speed
	if (pEntity->v.velocity.x >= flMaxVelocity) return FALSE;
	if (pEntity->v.velocity.y >= flMaxVelocity) return FALSE;
	if (pEntity->v.velocity.z >= flMaxVelocity) return FALSE;
	if (pEntity->v.velocity.x <= -flMaxVelocity) return FALSE;
	if (pEntity->v.velocity.y <= -flMaxVelocity) return FALSE;
	if (pEntity->v.velocity.z <= -flMaxVelocity) return FALSE;
	*/
	return TRUE;
}

Vector UTIL_VecToAngles(const Vector &vec)
{
	float rgflVecOut[3];
	VEC_TO_ANGLES(vec, rgflVecOut);
	return Vector(rgflVecOut);
}

void UTIL_MakeAimVectors( const Vector &vecAngles )
{
	float rgflVec[3];
	vecAngles.CopyToArray(rgflVec);
	rgflVec[0] = -rgflVec[0];
	MAKE_VECTORS(rgflVec);
}

void UTIL_Bubbles( Vector mins, Vector maxs, int count )
{
	Vector mid =  (mins + maxs) * 0.5;

	float flHeight = UTIL_WaterLevel( mid,  mid.z, mid.z + 1024 );
	flHeight = flHeight - mins.z;

	MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, mid );
	WRITE_BYTE( TE_BUBBLES );
	WRITE_COORD( mins.x );	// mins
	WRITE_COORD( mins.y );
	WRITE_COORD( mins.z );
	WRITE_COORD( maxs.x );	// maxz
	WRITE_COORD( maxs.y );
	WRITE_COORD( maxs.z );
	WRITE_COORD( flHeight );			// height
	WRITE_SHORT( MODEL_INDEX("sprites/bubble.spr") );
	WRITE_BYTE( count ); // count
	WRITE_COORD( 8 ); // speed
	MESSAGE_END();
}

float UTIL_WaterLevel( const Vector &position, float minz, float maxz )
{
	Vector midUp = position;
	midUp.z = minz;

	if (POINT_CONTENTS(midUp) != CONTENTS_WATER)
	{
		return minz;
	}

	midUp.z = maxz;

	if (POINT_CONTENTS(midUp) == CONTENTS_WATER)
	{
		return maxz;
	}

	float diff = maxz - minz;

	while (diff > 1.0)
	{
		midUp.z = minz + diff/2.0;

		if (POINT_CONTENTS(midUp) == CONTENTS_WATER)
		{
			minz = midUp.z;
		}
		else
		{
			maxz = midUp.z;
		}
		diff = maxz - minz;
	}

	return midUp.z;
}

void UTIL_EjectBrass(const Vector &vecOrigin, const Vector &vecVelocity, float rotation, int model, int soundtype)
{
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecOrigin );
	WRITE_BYTE( TE_MODEL);
	WRITE_COORD( vecOrigin.x);
	WRITE_COORD( vecOrigin.y);
	WRITE_COORD( vecOrigin.z);
	WRITE_COORD( vecVelocity.x);
	WRITE_COORD( vecVelocity.y);
	WRITE_COORD( vecVelocity.z);
	WRITE_ANGLE( rotation );
	WRITE_SHORT( model );
	WRITE_BYTE ( soundtype);
	WRITE_BYTE ( 25 );// 2.5 seconds
	MESSAGE_END();
}

void UTIL_DecalGunshot(TraceResult *pTrace)
{
	if (!pTrace->pHit || pTrace->pHit->free || (pTrace->pHit->v.flags & FL_KILLME))
	{
		return;
	}

	if (pTrace->pHit->v.solid == SOLID_BSP || pTrace->pHit->v.movetype == MOVETYPE_PUSHSTEP )
	{
		int decalNumber = GET_DAMAGE_DECAL(pTrace->pHit);

		if (decalNumber < 0 || decalNumber > (int)g_Decals.size())
		{
			return;
		}

		int index = g_Decals[decalNumber]->index;

		if (index < 0 || pTrace->flFraction == 1.0)
		{
			return;
		}

		MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pTrace->vecEndPos);
		WRITE_BYTE(TE_GUNSHOTDECAL);
		WRITE_COORD(pTrace->vecEndPos.x );
		WRITE_COORD(pTrace->vecEndPos.y );
		WRITE_COORD(pTrace->vecEndPos.z );
		WRITE_SHORT((short)ENTINDEX(pTrace->pHit));
		WRITE_BYTE(index);
		MESSAGE_END();
	}
}

void UTIL_DecalTrace(TraceResult *pTrace, int iDecalIndex)
{
	short entityIndex;
	int message;

	if (iDecalIndex < 0 || pTrace->flFraction == 1.0)
	{
		return;
	}

	// Only decal BSP models
	if (!IsValidPev(pTrace->pHit))
	{
		entityIndex = 0;
	}
	else 
	{
		if (pTrace->pHit && !(pTrace->pHit->v.solid == SOLID_BSP || pTrace->pHit->v.movetype == MOVETYPE_PUSHSTEP))
		{
			return;
		}

		entityIndex = ENTINDEX(pTrace->pHit);
	}

	message = TE_DECAL;
	if (entityIndex != 0)
	{
		if (iDecalIndex > 255)
		{
			message = TE_DECALHIGH;
			iDecalIndex -= 256;
		}
	}
	else
	{
		message = TE_WORLDDECAL;
		if (iDecalIndex > 255)
		{
			message = TE_WORLDDECALHIGH;
			iDecalIndex -= 256;
		}
	}

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(message);
	WRITE_COORD(pTrace->vecEndPos.x);
	WRITE_COORD(pTrace->vecEndPos.y);
	WRITE_COORD(pTrace->vecEndPos.z);
	WRITE_BYTE(iDecalIndex);

	if (entityIndex)
	{
		WRITE_SHORT(entityIndex);
	}

	MESSAGE_END();
}

TraceResult UTIL_GetGlobalTrace( )
{
	TraceResult tr;

	tr.fAllSolid		= gpGlobals->trace_allsolid;
	tr.fStartSolid		= gpGlobals->trace_startsolid;
	tr.fInOpen			= gpGlobals->trace_inopen;
	tr.fInWater			= gpGlobals->trace_inwater;
	tr.flFraction		= gpGlobals->trace_fraction;
	tr.flPlaneDist		= gpGlobals->trace_plane_dist;
	tr.pHit			= gpGlobals->trace_ent;
	tr.vecEndPos		= gpGlobals->trace_endpos;
	tr.vecPlaneNormal	= gpGlobals->trace_plane_normal;
	tr.iHitgroup		= gpGlobals->trace_hitgroup;
	return tr;
}

void UTIL_EmitAmbientSound( edict_t *entity, const Vector &vecOrigin, const char *samp, float vol, float attenuation, int fFlags, int pitch )
{
	float rgfl[3];
	vecOrigin.CopyToArray(rgfl);

	EMIT_AMBIENT_SOUND(entity, rgfl, samp, vol, attenuation, fFlags, pitch);
}

// hit the world, try to play sound based on texture material type
float TEXTURETYPE_PlaySound(TraceResult *ptr,  Vector vecSrc, Vector vecEnd)
{
	char chTextureType;
	float fvol;
	float fvolbar;
	char szbuffer[64];
	const char *pTextureName;
	float rgfl1[3];
	float rgfl2[3];
	const char *rgsz[4];
	int cnt;
	float fattn = ATTN_NORM;

	edict_t *pEntity = ptr->pHit;

	chTextureType = 0;

	if (pEntity && CLASSIFY(pEntity) != CLASS_NONE && CLASSIFY(pEntity) != CLASS_MACHINE)
		// hit body
			chTextureType = CHAR_TEX_FLESH;
	else
	{
		// hit world
		// find texture under strike, get material type
		// copy trace vector into array for trace_texture

		vecSrc.CopyToArray(rgfl1);
		vecEnd.CopyToArray(rgfl2);

		// get texture from entity or world (world is ent(0))
		if (pEntity)
			pTextureName = TRACE_TEXTURE( pEntity, rgfl1, rgfl2 );
		else
			pTextureName = TRACE_TEXTURE( ENT(0), rgfl1, rgfl2 );

		if ( pTextureName )
		{
			// strip leading '-0' or '+0~' or '{' or '!'
			if (*pTextureName == '-' || *pTextureName == '+')
				pTextureName += 2;

			if (*pTextureName == '{' || *pTextureName == '!' || *pTextureName == '~' || *pTextureName == ' ')
				pTextureName++;
			// '}}'
			strcpy(szbuffer, pTextureName);
			szbuffer[CBTEXTURENAMEMAX - 1] = 0;

			// get texture type
			chTextureType = MDLL_PM_FindTextureType(szbuffer);
		}
	}

	switch (chTextureType)
	{
	default:
	case CHAR_TEX_CONCRETE: fvol = 0.9;	fvolbar = 0.6;
		rgsz[0] = "player/pl_step1.wav";
		rgsz[1] = "player/pl_step2.wav";
		cnt = 2;
		break;
	case CHAR_TEX_METAL: fvol = 0.9; fvolbar = 0.3;
		rgsz[0] = "player/pl_metal1.wav";
		rgsz[1] = "player/pl_metal2.wav";
		cnt = 2;
		break;
	case CHAR_TEX_DIRT:	fvol = 0.9; fvolbar = 0.1;
		rgsz[0] = "player/pl_dirt1.wav";
		rgsz[1] = "player/pl_dirt2.wav";
		rgsz[2] = "player/pl_dirt3.wav";
		cnt = 3;
		break;
	case CHAR_TEX_VENT:	fvol = 0.5; fvolbar = 0.3;
		rgsz[0] = "player/pl_duct1.wav";
		rgsz[1] = "player/pl_duct1.wav";
		cnt = 2;
		break;
	case CHAR_TEX_GRATE: fvol = 0.9; fvolbar = 0.5;
		rgsz[0] = "player/pl_grate1.wav";
		rgsz[1] = "player/pl_grate4.wav";
		cnt = 2;
		break;
	case CHAR_TEX_TILE:	fvol = 0.8; fvolbar = 0.2;
		rgsz[0] = "player/pl_tile1.wav";
		rgsz[1] = "player/pl_tile3.wav";
		rgsz[2] = "player/pl_tile2.wav";
		rgsz[3] = "player/pl_tile4.wav";
		cnt = 4;
		break;
	case CHAR_TEX_SLOSH: fvol = 0.9; fvolbar = 0.0;
		rgsz[0] = "player/pl_slosh1.wav";
		rgsz[1] = "player/pl_slosh3.wav";
		rgsz[2] = "player/pl_slosh2.wav";
		rgsz[3] = "player/pl_slosh4.wav";
		cnt = 4;
		break;
	case CHAR_TEX_WOOD: fvol = 0.9; fvolbar = 0.2;
		rgsz[0] = "debris/wood1.wav";
		rgsz[1] = "debris/wood2.wav";
		rgsz[2] = "debris/wood3.wav";
		cnt = 3;
		break;
	case CHAR_TEX_GLASS:
	case CHAR_TEX_COMPUTER:
		fvol = 0.8; fvolbar = 0.2;
		rgsz[0] = "debris/glass1.wav";
		rgsz[1] = "debris/glass2.wav";
		rgsz[2] = "debris/glass3.wav";
		cnt = 3;
		break;
	case CHAR_TEX_FLESH:
		fvol = 1.0;	fvolbar = 0.2;
		rgsz[0] = "weapons/bullet_hit1.wav";
		rgsz[1] = "weapons/bullet_hit2.wav";
		fattn = 1.0;
		cnt = 2;
		break;
	}

	// play material hit sound
	UTIL_EmitAmbientSound(ENT(0), ptr->vecEndPos, rgsz[RANDOM_LONG(0,cnt-1)], fvol, fattn, 0, 96 + RANDOM_LONG(0,0xf));

	return fvolbar;
}

void FireBulletsPlayer(edict_t* pPlayer, edict_t* pAttacker, int iShotsCount, Vector vecSpread, float flDistance, float flDamage, int bitsDamageType, int iTracerFreq)
{
	float x, y, z;
	static int tracerCount;
	TraceResult tr;

	Vector vecSrc = pPlayer->v.origin + pPlayer->v.view_ofs; 
	MAKE_VECTORS(pPlayer->v.v_angle + pPlayer->v.punchangle);

	Vector vecDirShooting = gpGlobals->v_forward;
	Vector vecRight = gpGlobals->v_right;
	Vector vecUp = gpGlobals->v_up;

	if (pAttacker == NULL)
	{
		pAttacker = pPlayer;  // the default attacker is ourselves
	}

	CLEAR_MULTI_DAMAGE();

	for (int iShot = 1; iShot <= iShotsCount; iShot++)
	{
		do // get circular gaussian spread
		{
			x = RANDOM_FLOAT(-0.5,0.5) + RANDOM_FLOAT(-0.5,0.5);
			y = RANDOM_FLOAT(-0.5,0.5) + RANDOM_FLOAT(-0.5,0.5);
			z = x*x + y*y;
		} while (z > 1);

		Vector vecDir = vecDirShooting +
			x * vecSpread.x * vecRight +
			y * vecSpread.y * vecUp;

		Vector vecEnd = vecSrc + vecDir * flDistance;

		TRACE_LINE(vecSrc, vecEnd, dont_ignore_monsters, pPlayer, &tr);

		if (iTracerFreq != 0 && (tracerCount++ % iTracerFreq) == 0)
		{
			// adjust tracer position for player
			Vector vecTracerSrc = vecSrc + Vector (0 , 0 , -4) + gpGlobals->v_right * 2 + gpGlobals->v_forward * 16;

			MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, vecTracerSrc );
			WRITE_BYTE( TE_TRACER );
			WRITE_COORD( vecTracerSrc.x );
			WRITE_COORD( vecTracerSrc.y );
			WRITE_COORD( vecTracerSrc.z );
			WRITE_COORD( tr.vecEndPos.x );
			WRITE_COORD( tr.vecEndPos.y );
			WRITE_COORD( tr.vecEndPos.z );
			MESSAGE_END();
		}

		// do damage, paint decals
		if (tr.flFraction != 1.0)
		{
			TRACE_ATTACK(tr.pHit, pAttacker, flDamage, vecDir, tr, bitsDamageType);
			TEXTURETYPE_PlaySound(&tr, vecSrc, vecEnd);
			UTIL_DecalGunshot(&tr);
		}
	}

	APPLY_MULTI_DAMAGE(pPlayer, pAttacker);
}

// RadiusDamage - this entity is exploding, or otherwise needs to inflict damage upon entities within a certain range.
// only damage ents that can clearly be seen by the explosion!
//
// Blocks 'ghost mines' and 'ghost nades'.
void RadiusDamage2(Vector vecSrc, edict_t* pInflictor, edict_t* pAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType)
{
	TraceResult	tr;
	Vector vecSpot;

	edict_t* pEntity = NULL;

	float flAdjustedDamage;
	float falloff = flRadius ? flDamage / flRadius : 1.0;

	int bInWater = (POINT_CONTENTS(vecSrc) == CONTENTS_WATER);

	if (!pAttacker)
	{
		pAttacker = pInflictor;
	}

	vecSrc.z += 1;

	while (!FNullEnt((pEntity = FIND_ENTITY_IN_SPHERE(pEntity, vecSrc, flRadius))))
	{
		if (pEntity->v.takedamage == DAMAGE_NO)
		{
			continue;
		}

		if (iClassIgnore != CLASS_NONE && CLASSIFY(pEntity) == iClassIgnore)
		{
			continue;
		}

		if (bInWater && pEntity->v.waterlevel == 0)
		{
			continue;
		}

		if (!bInWater && pEntity->v.waterlevel == 3)
		{
			continue;
		}

		vecSpot = (pEntity->v.absmax + pEntity->v.absmin) * 0.5;

		TRACE_LINE(vecSrc, vecSpot, dont_ignore_monsters, pInflictor, &tr);

		if (tr.flFraction != 1.0 && (FNullEnt(tr.pHit) || !ENTINDEX(tr.pHit) || tr.pHit->v.movetype == MOVETYPE_PUSH))
		{
			continue;
		}

		if (pEntity != tr.pHit && pAttacker != pEntity)
		{
			continue;
		}

		flAdjustedDamage = (vecSrc - tr.vecEndPos).Length() * falloff;
		flAdjustedDamage = flDamage - flAdjustedDamage;

		if (flAdjustedDamage <= 0)
		{
			continue;
		}

		if (tr.fStartSolid)
		{
			tr.vecEndPos = vecSrc;
			tr.flFraction = 0.0;
		}

		if (tr.flFraction != 1.0)
		{
			CLEAR_MULTI_DAMAGE();
			TRACE_ATTACK(pEntity, pInflictor, flAdjustedDamage, (tr.vecEndPos - vecSrc).Normalize( ), tr, bitsDamageType);
			APPLY_MULTI_DAMAGE(pInflictor, pAttacker);
		}
		else
		{
			TAKE_DAMAGE(pEntity, pInflictor, pAttacker, flAdjustedDamage, bitsDamageType);
		}
	}
}

void printf2(const char *fmt, ...)
{
	va_list argptr;
	static char string[384];
	va_start(argptr, fmt);
	vsnprintf(string, sizeof(string) - 1, fmt, argptr);
	string[sizeof(string) - 1] = '\0';
	va_end(argptr);

	SERVER_PRINT(string);
}

