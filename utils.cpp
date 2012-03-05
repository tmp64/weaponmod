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


short g_sModelIndexBloodDrop; // holds the sprite index for blood drops
short g_sModelIndexBloodSpray; // holds the sprite index for blood spray (bigger)


BOOL UTIL_ShouldShowBlood( int color )
{
	if ( color != DONT_BLEED )
	{
		if ( color == BLOOD_COLOR_RED )
		{
			if ( CVAR_GET_FLOAT("violence_hblood") != 0 )
				return TRUE;
		}
		else
		{
			if ( CVAR_GET_FLOAT("violence_ablood") != 0 )
				return TRUE;
		}
	}
	return FALSE;
}

void UTIL_BloodDrips(const Vector &origin, int color, int amount)
{
	if (!UTIL_ShouldShowBlood(color))
		return;

	if (color == DONT_BLEED || amount == 0)
		return;

	amount *= 2;

	if (amount > 255)
		amount = 255;

	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, origin );
		WRITE_BYTE( TE_BLOODSPRITE );
		WRITE_COORD( origin.x);								// pos
		WRITE_COORD( origin.y);
		WRITE_COORD( origin.z);
		WRITE_SHORT( g_sModelIndexBloodSpray );				// initial sprite model
		WRITE_SHORT( g_sModelIndexBloodDrop );				// droplet sprite models
		WRITE_BYTE( color );								// color index into host_basepal
		WRITE_BYTE( min( max( 3, amount / 10 ), 16 ) );		// size
	MESSAGE_END();
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

	const char g_GunshotDecals[5][7] =
	{
		"{shot1",
		"{shot2",
		"{shot3",
		"{shot4",
		"{shot5"
	};

	int index = DECAL_INDEX(g_GunshotDecals[RANDOM_LONG(0, 4)]);
	
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

void UTIL_MakeVectors( const Vector &vecAngles )
{
	MAKE_VECTORS( vecAngles );
}

void UTIL_EmitAmbientSound( edict_t *entity, const Vector &vecOrigin, const char *samp, float vol, float attenuation, int fFlags, int pitch )
{
	float rgfl[3];
	vecOrigin.CopyToArray(rgfl);

	EMIT_AMBIENT_SOUND(entity, rgfl, samp, vol, attenuation, fFlags, pitch);
}

float TEXTURETYPE_PlaySound(TraceResult *ptr,  Vector vecSrc, Vector vecEnd)
{
	// hit the world, try to play sound based on texture material type
	
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

	UTIL_MakeVectors(pPlayer->v.v_angle + pPlayer->v.punchangle);
	
	Vector vecDirShooting = gpGlobals->v_forward;;
	Vector vecRight = gpGlobals->v_right;
	Vector vecUp = gpGlobals->v_up;

	if (pAttacker == NULL)
	{
		pAttacker = pPlayer;  // the default attacker is ourselves
	}

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
		Vector vecEnd;

		vecEnd = vecSrc + vecDir * flDistance;

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
			if (IsValidPev(tr.pHit))
			{
				// Headshot
				if (tr.iHitgroup == 1)
				{
					flDamage *= 3;
				}

				if (CLASSIFY(tr.pHit) != CLASS_NONE)
				{
					*((int *)tr.pHit->pvPrivateData + m_LastHitGroup) = tr.iHitgroup;
				}

				int iBloodColor = BLOOD_COLOR(tr.pHit);

				if (iBloodColor != DONT_BLEED)
				{
					TRACE_BLEED(tr.pHit, flDamage, vecDir, tr, bitsDamageType);
					UTIL_BloodDrips(tr.vecEndPos - vecDir * 4, iBloodColor, (int)flDamage);
				}

				TAKE_DAMAGE(tr.pHit, pPlayer, pAttacker, flDamage, bitsDamageType);

				// WTF?
				gpGlobals->v_forward = vecDirShooting;
				gpGlobals->v_right = vecRight;
				gpGlobals->v_up = vecUp;
			}

			TEXTURETYPE_PlaySound(&tr, vecSrc, vecEnd);
			UTIL_DecalGunshot(&tr);
		}
	}
}