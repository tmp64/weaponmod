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
#include "CVirtHook.h"
#include "libFunc.h"


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

	const char GunshotDecals[5][7] =
	{
		"{shot1",
		"{shot2",
		"{shot3",
		"{shot4",
		"{shot5"
	};

	int index = DECAL_INDEX(GunshotDecals[RANDOM_LONG(0, 4)]);
	
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
	if ( entityIndex != 0 )
	{
		if ( iDecalIndex > 255 )
		{
			message = TE_DECALHIGH;
			iDecalIndex -= 256;
		}
	}
	else
	{
		message = TE_WORLDDECAL;
		if ( iDecalIndex > 255 )
		{
			message = TE_WORLDDECALHIGH;
			iDecalIndex -= 256;
		}
	}
	
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( message );
	WRITE_COORD( pTrace->vecEndPos.x );
	WRITE_COORD( pTrace->vecEndPos.y );
	WRITE_COORD( pTrace->vecEndPos.z );
	WRITE_BYTE( iDecalIndex );
	
	if ( entityIndex )
	{
			WRITE_SHORT( entityIndex );
	}

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
	
	Vector vecDirShooting = gpGlobals->v_forward;
	Vector vecRight = gpGlobals->v_right;
	Vector vecUp = gpGlobals->v_up;

	if (pAttacker == NULL)
	{
		pAttacker = pPlayer;  // the default attacker is ourselves
	}
#ifdef _WIN32
	reinterpret_cast<int (__cdecl *)()>(g_dllFuncs[Func_ClearMultiDamage].pAddress)();
#elif __linux__
	reinterpret_cast<int (*)()>(g_dllFuncs[Func_ClearMultiDamage].pAddress)();
#endif
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

			//print_srvconsole("!!!! %d \n", ENTINDEX(tr.pHit));
		}
	}
#ifdef _WIN32
	reinterpret_cast<int (__cdecl *)(entvars_t*, entvars_t*)>(g_dllFuncs[Func_ApplyMultiDamage].pAddress)(&(pPlayer->v), &(pAttacker->v));
#elif __linux__
	reinterpret_cast<int (*)(entvars_t*, entvars_t*)>(g_dllFuncs[Func_ApplyMultiDamage].pAddress)(&(pPlayer->v), &(pAttacker->v));
#endif
}
