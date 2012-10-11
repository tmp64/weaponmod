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

#include "weaponmod.h"
#include "hooks.h"
#include "utils.h"


edict_t* INDEXENT2(int iEdictNum)
{ 
	if (iEdictNum >= 1 && iEdictNum <= gpGlobals->maxClients)
		return MF_GetPlayerEdict(iEdictNum);
	else
		return (*g_engfuncs.pfnPEntityOfEntIndex)(iEdictNum); 
}


edict_t *GetPrivateCbase(edict_t *pEntity, int iOffset)
{
    void *pPrivate=*((void **)((int *)(edict_t *)(INDEXENT(0) + ENTINDEX(pEntity))->pvPrivateData + iOffset));

    if (!pPrivate)
    {
        return NULL;
    }

    return PrivateToEdict(pPrivate);	
}


int Player_AmmoInventory(edict_t* pPlayer, edict_t* pWeapon, BOOL bPrimary)
{
	int iAmmoIndex = (int)*((int *)pWeapon->pvPrivateData + (bPrimary ? m_iPrimaryAmmoType : m_iSecondaryAmmoType));

	if (iAmmoIndex == -1)
	{
		return -1;
	}

	return (int)*((int *)pPlayer->pvPrivateData + m_rgAmmo + iAmmoIndex - 1);
}


int Player_Set_AmmoInventory(edict_t* pPlayer, edict_t* pWeapon, BOOL bPrimary, int Amount)
{
	int iAmmoIndex = (int)*((int *)pWeapon->pvPrivateData + (bPrimary ? m_iPrimaryAmmoType : m_iSecondaryAmmoType));

	if (iAmmoIndex == -1)
	{
		return 0;
	}

	*((int *)pPlayer->pvPrivateData + m_rgAmmo + iAmmoIndex - 1) = Amount;

	return 1;
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

const char* get_localinfo(const char* name , const char* def = 0)
{
	const char* b = LOCALINFO((char*)name);
	if (((b==0)||(*b==0)) && def)
		SET_LOCALINFO((char*)name,(char*)(b = def));
	return b;
}

void print_srvconsole(char *fmt, ...)
{
	va_list argptr;
	static char string[384];
	va_start(argptr, fmt);
	vsnprintf(string, sizeof(string) - 1, fmt, argptr);
	string[sizeof(string) - 1] = '\0';
	va_end(argptr);
       
	SERVER_PRINT(string);
}

Vector ParseVec(char *pString)
{
	char *pValue;
	Vector vecResult;

	vecResult.x = atoi(pValue = pString);

	for (int i = 0; i < 2; i++)
	{
		pValue = strchr(pValue + i, ' ');
		vecResult[i + 1] = atoi(pValue);
	}

	return vecResult;
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
	int iEntity;
	int index = DECAL_INDEX("{shot1") + RANDOM_LONG(0, 4);
	
	if (index < 0 || pTrace->flFraction == 1.0)
	{
		return;
	}

	if (!IsValidPev(pTrace->pHit))
	{
		iEntity = 0;
	}
	else
	{
		if (pTrace->pHit && !(pTrace->pHit->v.solid == SOLID_BSP || pTrace->pHit->v.movetype == MOVETYPE_PUSHSTEP))
		{
			return;
		}

		iEntity = ENTINDEX(pTrace->pHit);
	}

	MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pTrace->vecEndPos );
		WRITE_BYTE( TE_GUNSHOTDECAL );
		WRITE_COORD( pTrace->vecEndPos.x );
		WRITE_COORD( pTrace->vecEndPos.y );
		WRITE_COORD( pTrace->vecEndPos.z );
		WRITE_SHORT( iEntity );
		WRITE_BYTE( index );
	MESSAGE_END();
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

void UTIL_EmitAmbientSound( edict_t *entity, const Vector &vecOrigin, const char *samp, float vol, float attenuation, int fFlags, int pitch )
{
	float rgfl[3];
	vecOrigin.CopyToArray(rgfl);

	EMIT_AMBIENT_SOUND(entity, rgfl, samp, vol, attenuation, fFlags, pitch);
}

void SendWeaponAnim(edict_t* pPlayer, edict_t* pWeapon, int iAnim)
{
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
	char *rgsz[4];
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
// Blocks 'ghost mines' and 'ghost nades' by fixing two bugs found in HLDM gamedll and HL engine.
// Credits to Jussi Kivilinna and Lev.
void RadiusDamage2(Vector vecSrc, edict_t* pInflictor, edict_t* pAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType)
{
	TraceResult	tr;
	TraceResult tr2;

	edict_t* pEntity = NULL;

	float flAdjustedDamage;
	float falloff = flRadius ? flDamage / flRadius : 1.0;

	int bInWater = (POINT_CONTENTS(vecSrc) == CONTENTS_WATER);

	vecSrc.z += 1;// in case grenade is lying on the ground

	if (!pAttacker)
	{
		pAttacker = pInflictor;
	}

	// iterate on all entities in the vicinity.
	while (!FNullEnt((pEntity = FIND_ENTITY_IN_SPHERE(pEntity, vecSrc, flRadius))))
	{
		if (pEntity->v.takedamage != DAMAGE_NO)
		{
			// UNDONE: this should check a damage mask, not an ignore
			if (iClassIgnore != CLASS_NONE && CLASSIFY(pEntity) == iClassIgnore)
			{// houndeyes don't hurt other houndeyes with their attack
				continue;
			}

			// blast's don't tavel into or out of water
			if (bInWater && pEntity->v.waterlevel == 0)
				continue;

			if (!bInWater && pEntity->v.waterlevel == 3)
				continue;
			
			TRACE_LINE(vecSrc, (pEntity->v.absmax + pEntity->v.absmin) * 0.5, dont_ignore_monsters, pInflictor, &tr);

			if (tr.pHit != pEntity && (tr.flFraction == 1.0 && tr.fStartSolid == 1))
			{
				// Bug number 2: TraceLine starts from solid (BSP object) and doesn't hits anything on it's way.
				// fStartSolid is set to '1', fAllSolid is set to '1' and flFraction is set to '1.0'.
				// More over, pHit is set to 'worldspawn' and vecEndPos is correctly set to spot to where we asked it to trace to.
				// Bug here may be in that that engine sets fAllSolid to '1' (part of the path is not in the wall) or
				// in that that we doesn't hit an entity or
				// in that that engine doesn't set flFraction and vecEndPos correctly when it hits same BSP object.
				tr.flFraction = 0.0f;
			}

			if ((tr.flFraction == 1.0 || tr.pHit == pEntity) && tr.fStartSolid)
			{
				// Bug number 1: Explosion has been happen inside BSP object.
				// But condition is the same as for explosion inside a player, so we just trace back and see what we will hit.
				TRACE_LINE(tr.vecEndPos, vecSrc, dont_ignore_monsters, pInflictor, &tr2);

				if (tr2.flFraction != 1.0 && !tr2.fStartSolid && tr2.pHit != pEntity)
				{
					tr.flFraction = 0.0f;
					tr.pHit = tr2.pHit;
				}
			}

			if (tr.flFraction == 1.0 || tr.pHit == pEntity)
			{// the explosion can 'see' this entity, so hurt them!
				if (tr.fStartSolid)
				{
					// if we're stuck inside them, fixup the position and distance
					tr.vecEndPos = vecSrc;
					tr.flFraction = 0.0;
				}
				
				// decrease damage for an ent that's farther from the bomb.
				flAdjustedDamage = (vecSrc - tr.vecEndPos).Length() * falloff;
				flAdjustedDamage = flDamage - flAdjustedDamage;
			
				if (flAdjustedDamage < 0)
				{
					flAdjustedDamage = 0;
				}

				if (tr.flFraction != 1.0)
				{
					CLEAR_MULTI_DAMAGE();
					TRACE_ATTACK(pEntity, pInflictor, flAdjustedDamage, (tr.vecEndPos - vecSrc).Normalize( ), tr, bitsDamageType);
					APPLY_MULTI_DAMAGE(pInflictor, pAttacker);
				}
				//else
				//{
				//	pEntity->TakeDamage ( pevInflictor, pevAttacker, flAdjustedDamage, bitsDamageType );
				//}
			}
		}
	}
}
