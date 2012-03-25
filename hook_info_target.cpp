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


int g_iAmmoBoxIndex;

edict_t* g_pEntity;

BOOL g_InfoTargetHooksEnabled;

CVirtHook g_VirtHook_InfoTarget;
AmmoBoxData AmmoBoxInfoArray[MAX_WEAPONS];



edict_t* Ammo_Spawn(int iId, Vector vecOrigin, Vector vecAngles)
{
	edict_t* pAmmoBox = NULL;

	static int iszAllocStringCached;

	if (iszAllocStringCached || (iszAllocStringCached = MAKE_STRING("info_target")))
	{
		pAmmoBox = CREATE_NAMED_ENTITY(iszAllocStringCached);
	}

	if (IsValidPev(pAmmoBox))
	{
		MDLL_Spawn(pAmmoBox);

		pAmmoBox->v.classname = MAKE_STRING(AmmoBoxInfoArray[iId].pszName);
		pAmmoBox->v.movetype = MOVETYPE_TOSS;
		pAmmoBox->v.solid = SOLID_TRIGGER;

		SET_SIZE(pAmmoBox, Vector(-16.0, -16.0, 0.0), Vector(16.0, 16.0, 16.0));
		SET_ORIGIN(pAmmoBox, vecOrigin);

		pAmmoBox->v.angles = vecAngles;

		if (AmmoBoxInfoArray[iId].iForward[Fwd_Ammo_Spawn])
		{
			MF_ExecuteForward
			(
				AmmoBoxInfoArray[iId].iForward[Fwd_Ammo_Spawn],
				static_cast<cell>(ENTINDEX(pAmmoBox)),
				static_cast<cell>(0)
			);
		}
	}

	return pAmmoBox;
}



void Ammo_Respawn(edict_t *pAmmoBox)
{
	if (pAmmoBox->v.spawnflags & SF_NORESPAWN)
	{
		return;
	}

	pAmmoBox->v.effects |= EF_NODRAW;
	pAmmoBox->v.solid = SOLID_NOT;
	pAmmoBox->v.nextthink = gpGlobals->time + AMMO_RESPAWN_TIME;
}



#ifdef _WIN32
	void __fastcall InfoTarget_Think(void *pPrivate)
#elif __linux__
	void InfoTarget_Think(void *pPrivate)
#endif
{
	static int k;

	g_pEntity = PrivateToEdict(pPrivate);

	if (!IsValidPev(g_pEntity))
	{
		return;
	}

	for (k = 0; k < g_iAmmoBoxIndex; k++)
	{
		if (!strcmpi(STRING(g_pEntity->v.classname), AmmoBoxInfoArray[k].pszName))
		{
			if (g_pEntity->v.effects & EF_NODRAW)
			{
				EMIT_SOUND_DYN2(g_pEntity, CHAN_WEAPON, "items/suitchargeok1.wav", 1.0, ATTN_NORM, 0, 150);

				g_pEntity->v.effects &= ~EF_NODRAW;
				g_pEntity->v.effects |= EF_MUZZLEFLASH;
			}

			g_pEntity->v.solid = SOLID_TRIGGER;
			break;
		}
	}

#ifdef _WIN32
	reinterpret_cast<int (__fastcall *)(void *, int)>(g_VirtHook_InfoTarget.GetOrigFunc(VirtFunc_Think))(pPrivate, NULL);
#elif __linux__
	reinterpret_cast<int (*)(void *)>(g_VirtHook_InfoTarget.GetOrigFunc(VirtFunc_Think))(pPrivate);
#endif
}



#ifdef _WIN32
	void __fastcall InfoTarget_Touch(void *pPrivate, int i, void *pPrivate2)
#elif __linux__
	void InfoTarget_Touch(void *pPrivate, , void *pOther)
#endif
{
	static int k;
	static edict_t* pOther;

	g_pEntity = PrivateToEdict(pPrivate);
	pOther = PrivateToEdict(pPrivate2);

	if (!IsValidPev(g_pEntity) || !IsValidPev(pOther))
	{
		return;
	}

	if (MF_IsPlayerAlive(ENTINDEX(pOther)))
	{
		for (k = 0; k < g_iAmmoBoxIndex; k++)
		{
			if (!strcmp(STRING(g_pEntity->v.classname), AmmoBoxInfoArray[k].pszName) && AmmoBoxInfoArray[k].iForward[Fwd_Ammo_AddAmmo])
			{
				if (
						MF_ExecuteForward
						(
							AmmoBoxInfoArray[k].iForward[Fwd_Ammo_AddAmmo], 
							static_cast<cell>(ENTINDEX(g_pEntity)),
							static_cast<cell>(ENTINDEX(pOther))
						)
					)
				{
					(g_pEntity->v.spawnflags & SF_NORESPAWN) ? REMOVE_ENTITY(g_pEntity) : Ammo_Respawn(g_pEntity);
					break;
				}
			}
		}
	}

#ifdef _WIN32
	reinterpret_cast<int (__fastcall *)(void *, int, void *)>(g_VirtHook_InfoTarget.GetOrigFunc(VirtFunc_Touch))(pPrivate, NULL, pPrivate2);
#elif __linux__
	reinterpret_cast<int (*)(void *, void *)>(g_VirtHook_InfoTarget.GetOrigFunc(VirtFunc_Touch))(pPrivate, pPrivate2);
#endif
}


void ActivateInfoTargetHooks()
{
	edict_t *pEdict = CREATE_ENTITY();

    CALL_GAME_ENTITY(PLID, "info_target", &pEdict->v);
    
    if (pEdict->pvPrivateData == NULL)
    {
        REMOVE_ENTITY(pEdict);
		return;
    }

	g_VirtHook_InfoTarget.SetHook(pEdict, VirtFunc_Touch, (int)InfoTarget_Touch);
	g_VirtHook_InfoTarget.SetHook(pEdict, VirtFunc_Think, (int)InfoTarget_Think);

	REMOVE_ENTITY(pEdict);
}