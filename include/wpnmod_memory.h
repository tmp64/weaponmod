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

#include "amxxmodule.h"
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

#define GET_AMMO_INDEX				g_Memory.GetAmmoIndex
#define REGISTER_AMMO_INFO			g_Memory.AddAmmoNameToAmmoRegistry

#define WEAPON_RESET_INFO			g_Memory.Weapon_ResetInfo
#define WEAPON_GET_ID				g_Memory.Weapon_Exists

#define WEAPON_GET_NAME				g_Memory.Weapon_GetpszName
#define WEAPON_GET_AMMO1			g_Memory.Weapon_GetpszAmmo1
#define WEAPON_GET_AMMO2			g_Memory.Weapon_GetpszAmmo2
#define WEAPON_GET_MAX_AMMO1		g_Memory.Weapon_GetMaxAmmo1
#define WEAPON_GET_MAX_AMMO2		g_Memory.Weapon_GetMaxAmmo2
#define WEAPON_GET_MAX_CLIP			g_Memory.Weapon_GetMaxClip
#define WEAPON_GET_WEIGHT			g_Memory.Weapon_GetWeight
#define WEAPON_GET_FLAGS			g_Memory.Weapon_GetFlags
#define WEAPON_GET_SLOT				g_Memory.Weapon_GetSlot
#define WEAPON_GET_SLOT_POSITION	g_Memory.Weapon_GetSlotPosition

#define WEAPON_SET_NAME				g_Memory.Weapon_SetpszName
#define WEAPON_SET_AMMO1			g_Memory.Weapon_SetpszAmmo1
#define WEAPON_SET_AMMO2			g_Memory.Weapon_SetpszAmmo2
#define WEAPON_SET_MAX_AMMO1		g_Memory.Weapon_SetMaxAmmo1
#define WEAPON_SET_MAX_AMMO2		g_Memory.Weapon_SetMaxAmmo2
#define WEAPON_SET_MAX_CLIP			g_Memory.Weapon_SetMaxClip
#define WEAPON_SET_ID				g_Memory.Weapon_SetId
#define WEAPON_SET_WEIGHT			g_Memory.Weapon_SetWeight
#define WEAPON_SET_FLAGS			g_Memory.Weapon_SetFlags
#define WEAPON_SET_SLOT				g_Memory.Weapon_SetSlot
#define WEAPON_SET_SLOT_POSITION	g_Memory.Weapon_SetSlotPosition


typedef struct
{
	int		iSlot;
	int		iPosition;
	const char	*pszAmmo1;	// ammo 1 type
	int		iMaxAmmo1;		// max ammo 1
	const char	*pszAmmo2;	// ammo 2 type
	int		iMaxAmmo2;		// max ammo 2
	const char	*pszName;
	int		iMaxClip;
	int		iId;
	int		iFlags;
	int		iWeight;// this value used to determine this weapon's importance in autoselection.
} ItemInfo;

typedef struct
{
	const char *pszName;
	int iId;
} AmmoInfo;

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

public:
	CMemory();

	ItemInfo* m_pItemInfoArray;
	AmmoInfo* m_pAmmoInfoArray;

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
	void Parse_GetDispatch(void);
	void Parse_CallGameEntity(void);
	void Parse_WorldPrecache(void);
	void Parse_InfoArrays(void);

	void EnableShieldHitboxTracing(void);

	bool IsNewGCC(void) { return m_bIsNewGCC; };

	module* GetModule_GameDll(void) { return &m_GameDllModule; };
	module* GetModule_Engine(void) { return &m_EngineModule; };
	module* GetModule_Metamod(void) { return &m_MetamodModule; };

	char* GetDllNameByModule(void* base);

	size_t ParseFunc(size_t start, size_t end, char* funcname, unsigned char* pattern, char* mask, size_t bytes);
	size_t ParseFunc(size_t start, size_t end, char* funcname, char* string, unsigned char* pattern, char* mask, size_t bytes);

	const char*	Weapon_GetpszName		(const int iId)		{ return m_pItemInfoArray[iId].pszName; }
	const char*	Weapon_GetpszAmmo1		(const int iId)		{ return m_pItemInfoArray[iId].pszAmmo1; }
	const char* Weapon_GetpszAmmo2		(const int iId)		{ return m_pItemInfoArray[iId].pszAmmo2; }
	int			Weapon_GetMaxAmmo1		(const int iId)		{ return m_pItemInfoArray[iId].iMaxAmmo1; }
	int			Weapon_GetMaxAmmo2		(const int iId)		{ return m_pItemInfoArray[iId].iMaxAmmo2; }
	int			Weapon_GetMaxClip		(const int iId)		{ return m_pItemInfoArray[iId].iMaxClip; }
	int			Weapon_GetId			(const int iId)		{ return m_pItemInfoArray[iId].iId; }
	int			Weapon_GetWeight		(const int iId)		{ return m_pItemInfoArray[iId].iWeight; }
	int			Weapon_GetFlags			(const int iId)		{ return m_pItemInfoArray[iId].iFlags; }
	int			Weapon_GetSlot			(const int iId)		{ return m_pItemInfoArray[iId].iSlot; }
	int			Weapon_GetSlotPosition	(const int iId)		{ return m_pItemInfoArray[iId].iPosition; }

	void	Weapon_SetpszName		(const int iId, const char* pszName)		{ m_pItemInfoArray[iId].pszName = pszName; }
	void	Weapon_SetpszAmmo1		(const int iId, const char* pszAmmo1)		{ m_pItemInfoArray[iId].pszAmmo1 = pszAmmo1; }
	void	Weapon_SetpszAmmo2		(const int iId, const char* pszAmmo2)		{ m_pItemInfoArray[iId].pszAmmo2 = pszAmmo2; }
	void	Weapon_SetMaxAmmo1		(const int iId, const int MaxAmmo1)			{ m_pItemInfoArray[iId].iMaxAmmo1 = MaxAmmo1; }
	void	Weapon_SetMaxAmmo2		(const int iId, const int MaxAmmo2)			{ m_pItemInfoArray[iId].iMaxAmmo2 = MaxAmmo2; }
	void	Weapon_SetMaxClip		(const int iId, const int MaxClip)			{ m_pItemInfoArray[iId].iMaxClip = MaxClip; }
	void	Weapon_SetId			(const int iId)								{ m_pItemInfoArray[iId].iId = iId; }
	void	Weapon_SetWeight		(const int iId, const int Weight)			{ m_pItemInfoArray[iId].iWeight = Weight; }
	void	Weapon_SetFlags			(const int iId, const int Flags)			{ m_pItemInfoArray[iId].iFlags = Flags; }
	void	Weapon_SetSlot			(const int iId, const int Slot)				{ m_pItemInfoArray[iId].iSlot = Slot; }
	void	Weapon_SetSlotPosition	(const int iId, const int SlotPosition)		{ m_pItemInfoArray[iId].iPosition = SlotPosition; }

	int Weapon_Exists(const char* szName)
	{
		for (int i = 1; i < MAX_WEAPONS; i++)
		{
			if (Weapon_GetpszName(i) && !_stricmp(Weapon_GetpszName(i), szName))
			{
				return i;
			}
		}

		return 0;
	}

	int		GetAmmoIndex				(const char *psz);
	bool	AddAmmoNameToAmmoRegistry	(const char *szAmmoname);

	void Weapon_ResetInfo(const int iId)
	{
		memset(&m_pItemInfoArray[iId], 0, sizeof(ItemInfo));
		//memset(&WeaponInfoArray[iId], 0, sizeof(WeaponData));
	}
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
