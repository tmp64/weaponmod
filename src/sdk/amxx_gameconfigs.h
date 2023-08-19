/*
 * Half-Life Weapon Mod
 * Copyright (c) 2012 - 2023 AGHL.RU Dev Team
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

#pragma once
#include <memory>
#include "sdk/amxxmodule.h"
#include "sdk/IGameConfigs.h"
#include "wpnmod_log.h"

 //! Deleter for IGameConfig pointers.
struct AmxxGameConfigDeleter
{
	void operator()(IGameConfig* pCfg)
	{
		MF_GetConfigManager()->CloseGameConfigFile(pCfg);
	}
};

//! Smart pointer for IGameConfig.
using IGameConfigPtr = std::unique_ptr<IGameConfig, AmxxGameConfigDeleter>;

//! Loads a game config file. Crashes if fails.
static IGameConfigPtr WpnMod_LoadGameConfigFile(const char* file)
{
	IGameConfig* pCfg = nullptr;
	char error[256] = "";

	if (!MF_GetConfigManager()->LoadGameConfigFile(file, &pCfg, error, sizeof(error)))
	{
		WPNMOD_LOG("Failed to load game config '%s': %s\n", file, error);
		std::abort();
	}

	return IGameConfigPtr(pCfg, AmxxGameConfigDeleter());
}
