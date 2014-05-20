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
#include "wpnmod_parse.h"
#include "wpnmod_hooks.h"



int AmxxCheckGame(const char* game)
{
	return !stricmp(game, "cstrike") || !stricmp(game, "czero") ? AMXX_GAME_BAD : AMXX_GAME_OK;
}

void OnAmxxAttach(void)
{
	MF_AddNatives(Natives);
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

int DispatchSpawn(edict_t *pent)
{
	g_Config.InitGameMod();
	g_Config.WorldPrecache();

	RETURN_META_VALUE(MRES_IGNORED, 0);
}

void ServerActivate_Post(edict_t *pEdictList, int edictCount, int clientMax)
{
	g_Config.ServerActivate();
	RETURN_META(MRES_IGNORED);
}

void ServerDeactivate()
{
	g_Config.ServerDeactivate();
	RETURN_META(MRES_IGNORED);
}

void OnAmxxDetach(void)
{
	g_Memory.UnsetHooks();
	g_Config.ServerShutDown();
}

int FN_DecalIndex_Post(const char *name)
{
	g_Config.DecalPushList(name);
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

void FN_OnFreeEntPrivateData(edict_t *pEnt)
{
	if (g_Ents != NULL && IsValidPev(pEnt))
	{
		int iEntity = ENTINDEX(pEnt);

		g_Ents[iEntity].iThink = NULL;
		g_Ents[iEntity].iTouch = NULL;
		g_Ents[iEntity].iExplode = NULL;
	}

	RETURN_META(MRES_IGNORED);
}

