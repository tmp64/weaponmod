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


#include "wpnmod_grenade.h"


edict_t* Grenade_ShootContact(edict_t *pOwner, Vector vecStart, Vector vecVelocity)
{
	edict_t *pGrenade = CREATE_NAMED_ENTITY(MAKE_STRING("grenade"));

	if (IsValidPev(pGrenade))
	{
		MDLL_Spawn(pGrenade);

		pGrenade->v.gravity = 0.5;
		pGrenade->v.velocity = vecVelocity;
		pGrenade->v.angles = UTIL_VecToAngles(pGrenade->v.velocity);
		pGrenade->v.owner = pOwner;
	
		SetTouch_(pGrenade, (void*)Grenade_ExplodeTouch);
		SetThink_(pGrenade, (void*)Grenade_ThinkBeforeContact);
		
		pGrenade->v.nextthink = gpGlobals->time;
		pGrenade->v.avelocity.x = RANDOM_FLOAT(-100, -500);

		g_Ents[ENTINDEX(pGrenade)].iExplode = NULL;

		SET_ORIGIN(pGrenade, vecStart);
	}

	return pGrenade;
}

edict_t* Grenade_ShootTimed(edict_t *pOwner, Vector vecStart, Vector vecVelocity, float flTime)
{
	edict_t *pGrenade = CREATE_NAMED_ENTITY(MAKE_STRING("grenade"));

	if (IsValidPev(pGrenade))
	{
		MDLL_Spawn(pGrenade);

		pGrenade->v.gravity = 0.5;
		pGrenade->v.friction = 0.8;
		pGrenade->v.velocity = vecVelocity;
		pGrenade->v.angles = UTIL_VecToAngles(pGrenade->v.velocity);
		pGrenade->v.owner = pOwner;

		SetTouch_(pGrenade, (void*)Grenade_BounceTouch);
		SetThink_(pGrenade, (void*)Grenade_TumbleThink);

		pGrenade->v.dmgtime = gpGlobals->time + flTime;
		pGrenade->v.nextthink = gpGlobals->time + flTime;

		pGrenade->v.sequence = RANDOM_LONG( 3, 6 );
		pGrenade->v.framerate = 1.0;

		g_Ents[ENTINDEX(pGrenade)].iExplode = NULL;

		SET_ORIGIN(pGrenade, vecStart);
		SET_MODEL(pGrenade, "models/w_grenade.mdl");
	}

	return pGrenade;
}

void Grenade_Explode(edict_t* pGrenade, int bitsDamageType)
{
	Vector vecSpot;
	TraceResult pTrace;

	pGrenade->v.solid = SOLID_NOT;
	pGrenade->v.takedamage = DAMAGE_NO;

	vecSpot = pGrenade->v.origin - pGrenade->v.velocity.Normalize() * 32;
	TRACE_LINE(vecSpot, vecSpot + pGrenade->v.velocity.Normalize() * 64, ignore_monsters, pGrenade, &pTrace);

	if (pTrace.flFraction == 1.0)
	{
		vecSpot = pGrenade->v.origin + Vector (0, 0, 8);
		TRACE_LINE(vecSpot, vecSpot + Vector (0, 0, -40), ignore_monsters, pGrenade, &pTrace);
	}

	if (pTrace.flFraction != 1.0)
	{
		float flPullOut = pGrenade->v.fov ? pGrenade->v.fov : pGrenade->v.dmg * 0.6;

		TraceResult tr;
		TRACE_LINE(pGrenade->v.origin, pTrace.vecEndPos + (pTrace.vecPlaneNormal * flPullOut), ignore_monsters, pGrenade, &tr);

		if (tr.flFraction == 1.0)
		{
			pGrenade->v.origin = tr.vecEndPos;
		}
	}

	int iContents = POINT_CONTENTS(pGrenade->v.origin);
	
	if (!(pGrenade->v.spawnflags & SF_EXPLOSION_NOFIREBALL))
	{
		MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pGrenade->v.origin);
			WRITE_BYTE(TE_EXPLOSION);		// This makes a dynamic light and the explosion sprites/sound
			WRITE_COORD(pGrenade->v.origin.x);	// Send to PAS because of the sound
			WRITE_COORD(pGrenade->v.origin.y);
			WRITE_COORD(pGrenade->v.origin.z);
			WRITE_SHORT(MODEL_INDEX((iContents == CONTENTS_WATER) ? "sprites/WXplo1.spr" : "sprites/zerogxplode.spr"));
			WRITE_BYTE(pGrenade->v.dmg * .30); // scale * 10
			WRITE_BYTE(15); // framerate
			WRITE_BYTE(TE_EXPLFLAG_NONE);
		MESSAGE_END();
	}

	if (!(pGrenade->v.spawnflags & SF_EXPLOSION_NODAMAGE))
	{
		edict_t *pOwner = pGrenade->v.owner;
		pGrenade->v.owner = NULL; // can't traceline attack owner if this is set

		RadiusDamage2(pGrenade->v.origin, pGrenade, pOwner, pGrenade->v.dmg, pGrenade->v.dmg * 2.5, CLASS_NONE, bitsDamageType);
	}

	if (!(pGrenade->v.spawnflags & SF_EXPLOSION_NODECAL))
	{
		UTIL_DecalTrace(&pTrace, DECAL_INDEX(RANDOM_FLOAT(0, 1) ? "{scorch1" : "{scorch2"));
	}

	if (!(pGrenade->v.spawnflags & SF_EXPLOSION_NODEBRIS) && iContents != CONTENTS_WATER)
	{
		switch (RANDOM_LONG(0, 2))
		{
			case 0:	EMIT_SOUND_DYN2(pGrenade, CHAN_VOICE, "weapons/debris1.wav", 0.55, ATTN_NORM, 0, PITCH_NORM);	break;
			case 1:	EMIT_SOUND_DYN2(pGrenade, CHAN_VOICE, "weapons/debris2.wav", 0.55, ATTN_NORM, 0, PITCH_NORM);	break;
			case 2:	EMIT_SOUND_DYN2(pGrenade, CHAN_VOICE, "weapons/debris3.wav", 0.55, ATTN_NORM, 0, PITCH_NORM);	break;
		}
	}

	if (!(pGrenade->v.spawnflags & SF_EXPLOSION_NOSPARKS) && iContents != CONTENTS_WATER)
	{
		edict_t* pSpark = NULL;
		int sparkCount = RANDOM_LONG(0, 3);
		
		for (int i = 0; i < sparkCount; i++)
		{
			pSpark = CREATE_NAMED_ENTITY(MAKE_STRING("spark_shower"));

			if (IsValidPev(pSpark))
			{
				pSpark->v.origin = pGrenade->v.origin;
				pSpark->v.angles = pTrace.vecPlaneNormal;

				MDLL_Spawn(pSpark);
			}
		}
	}

	if (!(pGrenade->v.spawnflags & SF_EXPLOSION_NOSMOKE))
	{
		edict_t* pTaskSmoke = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));

		if (IsValidPev(pTaskSmoke))
		{
			SetThink_(pTaskSmoke, (void*)Grenade_ThinkSmoke);

			pTaskSmoke->v.classname = MAKE_STRING("wpnmod_smoke_task");
			pTaskSmoke->v.nextthink = gpGlobals->time + 0.3;
			pTaskSmoke->v.dmg = pGrenade->v.dmg;
			
			SET_ORIGIN(pTaskSmoke, pGrenade->v.origin);
		}
	}

	int iGrenade = ENTINDEX(pGrenade);

	if (g_Ents[iGrenade].iExplode)
	{
		MF_ExecuteForward(g_Ents[iGrenade].iExplode, static_cast<cell>(ENTINDEX(pGrenade)), reinterpret_cast<cell>(&pTrace));
	}

	g_Ents[iGrenade].iExplode = NULL;

	SetEntForward(pGrenade, Think, NULL, NULL);
	SetEntForward(pGrenade, Touch, NULL, NULL);

	pGrenade->v.velocity = Vector(0, 0, 0);
	pGrenade->v.effects |= EF_NODRAW;
	pGrenade->v.flags |= FL_KILLME;
}

#ifdef _WIN32
void __fastcall Grenade_BounceTouch(void *pPrivate, int i, void *pPrivate2)
#else
void Grenade_BounceTouch(void *pPrivate, void *pPrivate2)
#endif
{
	edict_t* pGrenade = PrivateToEdict(pPrivate);

	if (!IsValidPev(pGrenade))
	{
		return;
	}

	edict_t* pOther = PrivateToEdict(pPrivate2);

	if (pOther == pGrenade->v.owner)
	{
		return;
	}

	// only do damage if we're moving fairly fast
	if (GetPrivateFloat(pGrenade, pvData_flNextAttack) < gpGlobals->time && pGrenade->v.velocity.Length() > 100)
	{
		if (pGrenade->v.owner)
		{
			TraceResult tr = UTIL_GetGlobalTrace();

			CLEAR_MULTI_DAMAGE();
			TRACE_ATTACK(pOther, pGrenade->v.owner, 1, gpGlobals->v_forward, tr, DMG_CLUB);
			APPLY_MULTI_DAMAGE(pGrenade, pGrenade->v.owner);
		}

		SetPrivateFloat(pGrenade, pvData_flNextAttack, gpGlobals->time + 1.0);
	}

	if (pGrenade->v.flags & FL_ONGROUND)
	{
		pGrenade->v.velocity = pGrenade->v.velocity * 0.8;
		pGrenade->v.sequence = RANDOM_LONG(1, 1);

		if (pGrenade->v.velocity.Length() <= 20)
		{
			pGrenade->v.avelocity = Vector(0, 0, 0);
		}
	}
	else if (pGrenade->v.fuser4 <= gpGlobals->time)
	{
		switch (RANDOM_LONG(0, 2))
		{
			case 0:	EMIT_SOUND_DYN2(pGrenade, CHAN_VOICE, "weapons/grenade_hit1.wav", 0.25, ATTN_NORM, 0, PITCH_NORM);	break;
			case 1:	EMIT_SOUND_DYN2(pGrenade, CHAN_VOICE, "weapons/grenade_hit2.wav", 0.25, ATTN_NORM, 0, PITCH_NORM);	break;
			case 2:	EMIT_SOUND_DYN2(pGrenade, CHAN_VOICE, "weapons/grenade_hit3.wav", 0.25, ATTN_NORM, 0, PITCH_NORM);	break;
		}
	}

	pGrenade->v.fuser4 = gpGlobals->time + 0.1;
	pGrenade->v.framerate = pGrenade->v.velocity.Length() / 200.0;

	if (pGrenade->v.framerate > 1.0)
	{
		pGrenade->v.framerate = 1;
	}
	else if (pGrenade->v.framerate < 0.5)
	{
		pGrenade->v.framerate = 0;
	}
}

#ifdef _WIN32
void __fastcall Grenade_ThinkBeforeContact(void *pPrivate)
#else
void Grenade_ThinkBeforeContact(void *pPrivate)
#endif
{
	edict_t* pGrenade = PrivateToEdict(pPrivate);

	if (!IsValidPev(pGrenade))
	{
		return;
	}

	if (!Entity_IsInWorld(pGrenade))
	{
		pGrenade->v.flags |= FL_KILLME;
		return;
	}

	pGrenade->v.nextthink = gpGlobals->time + 0.2;

	if (pGrenade->v.waterlevel != 0)
	{
		pGrenade->v.velocity = pGrenade->v.velocity * 0.5;
	}
}


#ifdef _WIN32
void __fastcall Grenade_TumbleThink(void *pPrivate)
#else
void Grenade_TumbleThink(void *pPrivate)
#endif
{
	edict_t* pGrenade = PrivateToEdict(pPrivate);

	if (!IsValidPev(pGrenade))
	{
		return;
	}

	if (!Entity_IsInWorld(pGrenade))
	{
		pGrenade->v.flags |= FL_KILLME;
		return;
	}

	pGrenade->v.nextthink = gpGlobals->time + 0.1;

	if (pGrenade->v.waterlevel != 0)
	{
		pGrenade->v.velocity = pGrenade->v.velocity * 0.5;
		pGrenade->v.framerate = 0.2;
	}

	if (pGrenade->v.dmgtime <= gpGlobals->time)
	{
		Grenade_Explode(pGrenade, DMG_BLAST);
	}	
}

#ifdef _WIN32
void __fastcall Grenade_ExplodeTouch(void *pPrivate, int i, void *pPrivate2)
#else
void Grenade_ExplodeTouch(void *pPrivate, void *pPrivate2)
#endif
{
	edict_t* pGrenade = PrivateToEdict(pPrivate);

	if (!IsValidPev(pGrenade))
	{
		return;
	}
	
	Grenade_Explode(pGrenade, DMG_BLAST);
}

#ifdef _WIN32
void __fastcall Grenade_ThinkSmoke(void *pPrivate)
#else
void Grenade_ThinkSmoke(void *pPrivate)
#endif
{
	edict_t* pEntity = PrivateToEdict(pPrivate);

	if (!IsValidPev(pEntity))
	{
		return;
	}

	if (POINT_CONTENTS(pEntity->v.origin) == CONTENTS_WATER)
	{
		UTIL_Bubbles(pEntity->v.origin - Vector( 64, 64, 64 ), pEntity->v.origin + Vector( 64, 64, 64 ), 100 );
	}
	else
	{
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pEntity->v.origin);
			WRITE_BYTE(TE_SMOKE);
			WRITE_COORD(pEntity->v.origin.x);
			WRITE_COORD(pEntity->v.origin.y);
			WRITE_COORD(pEntity->v.origin.z);
			WRITE_SHORT(MODEL_INDEX("sprites/steam1.spr"));
			WRITE_BYTE(pEntity->v.dmg* 0.4); // scale * 10
			WRITE_BYTE(12); // framerate
		MESSAGE_END();
	}

	pEntity->v.flags |= FL_KILLME;
}
