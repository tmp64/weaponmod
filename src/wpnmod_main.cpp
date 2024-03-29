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

#include "wpnmod_config.h"
#include "wpnmod_entity.h"
#include "wpnmod_items.h"
#include "wpnmod_parse.h"
#include "wpnmod_hooks.h"


int AmxxCheckGame(const char* game)
{
	return !stricmp(game, "cstrike") || !stricmp(game, "czero") ? AMXX_GAME_BAD : AMXX_GAME_OK;
}

void OnAmxxAttach(void)
{
	MF_AddNatives(Natives);
	g_Config.LoadGameData();
	g_Items.LoadGameData();
}

// Called by Meta_Attach.
int WpnMod_Init(void)
{
	g_log.Init();

	WPNMOD_LOG("Start.\n");
	WPNMOD_LOG(" Version %s %s\n", Plugin_info.version, SERVER_OS);

	if (!g_Memory.Init())
	{
		WPNMOD_LOG("Errors occurred. Please visit http://aghl.ru/forum/ for support.\n");
		return 0;
	}

	WPNMOD_LOG("Done.\n");
	return 1;
}

void WpnMod_Init_GameMod(void)
{
	g_Entity.AllocEntities();
	g_Config.InitGameMod();
}

void WpnMod_Precache(void)
{
	g_Config.SetConfigFile();

	UTIL_PrecacheOther("item_suit");
	UTIL_PrecacheOther("item_battery");
	UTIL_PrecacheOther("item_healthkit");
	UTIL_PrecacheOther("item_longjump");

	
	for (int i = 1; i < MAX_WEAPONS; i++)
	{
		if (WEAPON_GET_NAME(i))
		{
			//printf2("!!!!!! Default: %d %s\n", i, WEAPON_GET_NAME(i));

			WEAPON_MAKE_DEFAULT(i);

			g_Items.m_pCurrentSlots[WEAPON_GET_SLOT(i)][WEAPON_GET_SLOT_POSITION(i)] = 1;
		}
	}

	g_Config.LoadBlackList();
}

void ServerActivate_Post(edict_t *pEdictList, int edictCount, int clientMax)
{
	//printf2("------------------\n");
	//for (int i = 1; i < MAX_WEAPONS; i++)
	//{
	//	printf2("!!!!!! Current: %d %s  %d\n", i, WEAPON_GET_NAME(i), WEAPON_GET_SLOT(i));
	//}


	g_Config.ServerActivate();
	RETURN_META(MRES_IGNORED);
}

void ServerDeactivate()
{
	g_Items.ServerDeactivate();
	g_Config.ServerDeactivate();
	RETURN_META(MRES_IGNORED);
}

void OnAmxxDetach(void)
{
	g_Memory.UnsetHooks();
	g_Items.FreeWeaponSlots();
	g_Entity.FreeEntities();
}

int DecalIndex_Post(const char *name)
{
	g_Config.DecalPushList(name);
	RETURN_META_VALUE(MRES_IGNORED, 0);
}

void *PvAllocEntPrivateData_Post(edict_t *pEdict, int32 cb)
{
	g_Entity.OnAllocEntPrivateData(pEdict);
	RETURN_META_VALUE(MRES_IGNORED, 0);
}

void ClientCommand(edict_t *pEntity)
{
	if (CConfig::ClientCommand(pEntity))
	{
		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}

