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

#include "wpnmod_entity.h"


CEntityManager g_Entity;


edict_t* Wpnmod_SpawnItem(const char* szName, Vector vecOrigin, Vector vecAngles)
{
	if (strstr(szName, "ammo_"))
	{
		return WpnMod_Ammo_Spawn(szName, vecOrigin, vecAngles);
	}

	if (strstr(szName, "weapon_"))
	{
		return WpnMod_Weapon_Spawn(szName, vecOrigin, vecAngles);
	}

	return NULL;
}

edict_t* CreateEntity(char *szName, Vector vecOrigin, Vector vecAngles)
{
	edict_t	*pent = CREATE_NAMED_ENTITY(MAKE_STRING(szName));
	
	if (!IsValidPev(pent))
	{
		return NULL;
	}

	pent->v.origin = vecOrigin;
	pent->v.angles = vecAngles;
	MDLL_Spawn(pent);

	return pent;
}

edict_t* WpnMod_Weapon_Spawn(const char* szName, Vector vecOrigin, Vector vecAngles)
{
	return CreateEntity((char*)szName, vecOrigin, vecAngles);
}

edict_t* WpnMod_Ammo_Spawn(const char* szName, Vector vecOrigin, Vector vecAngles)
{
	int iId = AMMOBOX_GET_ID(szName);

	if (!iId)
	{
		return NULL;
	}

	edict_t* pAmmoBox = CREATE_NAMED_ENTITY(MAKE_STRING(gAmmoBoxReference));

	if (IsValidPev(pAmmoBox))
	{
		MDLL_Spawn(pAmmoBox);
		SET_ORIGIN(pAmmoBox, vecOrigin);

		pAmmoBox->v.classname = MAKE_STRING(AMMOBOX_GET_NAME(iId));
		pAmmoBox->v.angles = vecAngles;

		AMMOBOX_FORWARD_EXECUTE(iId, Fwd_Ammo_Spawn, pAmmoBox, NULL);
	}

	return pAmmoBox;
}
