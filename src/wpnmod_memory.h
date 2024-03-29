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

#ifndef _WPNMOD_MEMORY_H
#define _WPNMOD_MEMORY_H

#include "sdk/amxxmodule.h"
#include "wpnmod_hooker.h"


#ifdef __linux__
	#define SERVER_OS "Linux"
#else
	#define SERVER_OS "Windows"
#endif

#define HOOK_FUNC(call)					\
{										\
	"",  NULL, (void*)call, {}, {}, 0,	\
}										\




class IRehldsApi;

class CMemory
{
private:
	size_t	m_start_gamedll;
	size_t	m_end_gamedll;

	size_t	m_start_engine;
	size_t	m_end_engine;

	module	m_EngineModule;
	module	m_GameDllModule;
	module	m_MetamodModule;

	bool	m_bSuccess;
	bool	m_bIsNewGCC;

	IRehldsApi* m_pRehldsApi = nullptr;

public:
	CMemory();

	void* m_pSubRemove;
	void* m_pClearMultiDamage;
	void* m_pApplyMultiDamage;
	void* m_pPlayerSetAnimation;
	void* m_pWorldPrecache;

	bool Init(void);
	void UnsetHooks(void);

	void Parse_SubRemove(void);
	void Parse_FallThink(void);
	void Parse_SetAnimation(void);
	void Parse_GiveNamedItem(void);
	void Parse_ClearMultiDamage(void);
	void Parse_ApplyMultiDamage(void);
	void Parse_AmmoSpawn(void);
	void Parse_ItemSpawn(void);
	void Parse_CallGameEntity(void);
	void Parse_WorldPrecache(void);
	void Parse_InfoArrays(void);

	// Engine
	void FindReHldsApi();
	void UnsetReHldsHooks();
	void Parse_GetDispatch(void);

	void EnableShieldHitboxTracing(void);

	bool IsNewGCC(void) { return m_bIsNewGCC; };

	module* GetModule_GameDll(void) { return &m_GameDllModule; };
	module* GetModule_Engine(void) { return &m_EngineModule; };
	module* GetModule_Metamod(void) { return &m_MetamodModule; };

	char* GetDllNameByModule(void* base, char* buf, size_t bufSize);

	size_t ParseFunc(size_t start, size_t end, char* funcname, unsigned char* pattern, char* mask, size_t bytes);
	size_t ParseFunc(size_t start, size_t end, char* funcname, char* string, unsigned char* pattern, char* mask, size_t bytes);

};

extern CMemory g_Memory;

extern function g_fh_GiveNamedItem;
extern function g_fh_FallThink;
extern function g_fh_AmmoSpawn;
extern function g_fh_ItemSpawn;
extern function g_fh_GetDispatch;
extern function g_fh_CallGameEntity;
extern function g_fh_WorldPrecache;

#endif // _WPNMOD_MEMORY_H
