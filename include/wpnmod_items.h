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

#ifndef _WPNMOD_ITEMS_H
#define _WPNMOD_ITEMS_H

#include "amxxmodule.h"

#include <string>
#include <vector>

#include "wpnmod_config.h"


#define WEAPON_RESPAWN_TIME					20

#define ITEM_FLAG_SELECTONEMPTY				1
#define ITEM_FLAG_NOAUTORELOAD				2
#define ITEM_FLAG_NOAUTOSWITCHEMPTY			4
#define ITEM_FLAG_LIMITINWORLD				8
#define ITEM_FLAG_EXHAUSTIBLE				16

#define AMMOBOX_REGISTER			g_Items.Ammobox_Register
#define AMMOBOX_GET_ID				g_Items.Ammobox_GetId
#define AMMOBOX_GET_NAME			g_Items.Ammobox_GetName
#define AMMOBOX_GET_COUNT			g_Items.Ammobox_GetCount

#define AMMOBOX_FORWARD_REGISTER	g_Items.Ammobox_RegisterForward
#define AMMOBOX_FORWARD_EXECUTE		g_Items.Ammobox_ExecuteForward

#define GET_AMMO_INDEX				g_Items.GetAmmoIndex

#define WEAPON_REGISTER				g_Items.Weapon_RegisterWeapon
#define WEAPON_RESET_INFO			g_Items.Weapon_ResetInfo
#define WEAPON_GET_ID				g_Items.Weapon_Exists
#define WEAPON_GET_NAME				g_Items.Weapon_GetpszName
#define WEAPON_GET_AMMO1			g_Items.Weapon_GetpszAmmo1
#define WEAPON_GET_AMMO2			g_Items.Weapon_GetpszAmmo2
#define WEAPON_GET_MAX_AMMO1		g_Items.Weapon_GetMaxAmmo1
#define WEAPON_GET_MAX_AMMO2		g_Items.Weapon_GetMaxAmmo2
#define WEAPON_GET_MAX_CLIP			g_Items.Weapon_GetMaxClip
#define WEAPON_GET_WEIGHT			g_Items.Weapon_GetWeight
#define WEAPON_GET_FLAGS			g_Items.Weapon_GetFlags
#define WEAPON_GET_SLOT				g_Items.Weapon_GetSlot
#define WEAPON_GET_SLOT_POSITION	g_Items.Weapon_GetSlotPosition

#define WEAPON_IS_CUSTOM			g_Items.Weapon_IsCustom
#define WEAPON_IS_DEFAULT			g_Items.Weapon_IsDefault

#define WEAPON_MAKE_DEFAULT			g_Items.Weapon_MarkAsDefault

#define WEAPON_FORWARD_REGISTER		g_Items.Weapon_RegisterForward
#define WEAPON_FORWARD_EXECUTE		g_Items.Weapon_ExecuteForward
#define WEAPON_FORWARD_IS_EXIST		g_Items.Weapon_GetForward


typedef struct
{
	const char *pszName;
	int iId;
} AmmoInfo;

typedef struct
{
	int		iSlot;
	int		iPosition;
	const char	*pszAmmo1;
	int		iMaxAmmo1;
	const char	*pszAmmo2;
	int		iMaxAmmo2;
	const char	*pszName;
	int		iMaxClip;
	int		iId;
	int		iFlags;
	int		iWeight;
} ItemInfo;

enum e_AmmoFwds
{
	Fwd_Ammo_Spawn,
	Fwd_Ammo_AddAmmo,
	Fwd_Ammo_End
};

enum e_WpnFwds
{
	Fwd_Wpn_Spawn,
	Fwd_Wpn_CanDeploy,
	Fwd_Wpn_Deploy,
	Fwd_Wpn_Idle,
	Fwd_Wpn_PrimaryAttack,
	Fwd_Wpn_SecondaryAttack,
	Fwd_Wpn_Reload,
	Fwd_Wpn_CanHolster,
	Fwd_Wpn_Holster,
	Fwd_Wpn_IsUseable,
	Fwd_Wpn_AddToPlayer,
	Fwd_Wpn_AddToPlayer2,
	Fwd_Wpn_ItemPostFrame,
	Fwd_Wpn_End
};

class CItems
{
public:
	ItemInfo* m_pItemInfoArray;
	AmmoInfo* m_pAmmoInfoArray;

	enum e_WpnType
	{
		Wpn_None,
		Wpn_Default,
		Wpn_Custom,
		Wpn_End
	};

	class CAmmoBoxInfo
	{
	public:
		//CPlugin* m_pAmxx;
		std::string m_strClassname;
		int m_AmxxForwards[Fwd_Ammo_End];

		CAmmoBoxInfo()
		{
			//m_pAmxx = NULL;
			memset(m_AmxxForwards, 0, sizeof(m_AmxxForwards));
		}
	}; std::vector <CAmmoBoxInfo*> m_AmmoBoxesInfo;

	class CWeaponInfo
	{
	public:
		//CPlugin* m_pAmxx;
		e_WpnType m_WpnType;
		int m_AmxxForwards[Fwd_Wpn_End];

		CWeaponInfo()
		{
			//m_pAmxx = NULL;
			m_WpnType = Wpn_None;
			memset(m_AmxxForwards, 0, sizeof(m_AmxxForwards));
		}
	}; CWeaponInfo m_WeaponsInfo[MAX_WEAPONS];

	CItems();

	int** m_pCurrentSlots;
	int m_iMaxWeaponSlots;
	int m_iMaxWeaponPositions;

	void AllocWeaponSlots			(int slots, int positions);
	void FreeWeaponSlots			(void);
	bool CheckSlots					(int iWeaponID);
	void ServerDeactivate			(void);

	bool m_bWeaponRefHooked;
	bool m_bAmmoBoxRefHooked;

	int Ammobox_Register			(const char *name);
	int Ammobox_RegisterForward		(int iId, e_AmmoFwds fwdType, AMX *amx, const char *pFuncName);
	int	Ammobox_ExecuteForward		(int iId, e_AmmoFwds fwdType, edict_t* pAmmobox, edict_t* pPlayer);
	int Ammobox_GetCount			(void);
	int Ammobox_GetId				(const char *name);
	const char* Ammobox_GetName		(int iId);

	int GetAmmoIndex				(const char *psz);
	bool AddAmmoNameToAmmoRegistry	(const char *szAmmoname);
	
	int Weapon_RegisterWeapon		(AMX *amx, cell *params);
	int Weapon_RegisterForward		(int iId, e_WpnFwds fwdType, AMX *amx, const char * pFuncName);
	int Weapon_ExecuteForward		(int iId, e_WpnFwds fwdType, edict_t* pWeapon, edict_t* pPlayer);
	int Weapon_GetForward			(int iId, e_WpnFwds fwdType);
	int Weapon_Exists				(const char* szName);

	bool Weapon_IsCustom			(int iId);
	bool Weapon_IsDefault			(int iId);

	void Weapon_MarkAsDefault		(int iId);
	void Weapon_ResetInfo			(int iId);

	int Weapon_GetMaxAmmo1			(const int iId)		{ return m_pItemInfoArray[iId].iMaxAmmo1; }
	int Weapon_GetMaxAmmo2			(const int iId)		{ return m_pItemInfoArray[iId].iMaxAmmo2; }
	int Weapon_GetMaxClip			(const int iId)		{ return m_pItemInfoArray[iId].iMaxClip; }
	int Weapon_GetId				(const int iId)		{ return m_pItemInfoArray[iId].iId; }
	int Weapon_GetWeight			(const int iId)		{ return m_pItemInfoArray[iId].iWeight; }
	int Weapon_GetFlags				(const int iId)		{return m_pItemInfoArray[iId].iFlags; }
	int Weapon_GetSlot				(const int iId)		{ return m_pItemInfoArray[iId].iSlot; }
	int Weapon_GetSlotPosition		(const int iId)		{ return m_pItemInfoArray[iId].iPosition; }

	const char*	Weapon_GetpszName	(const int iId)		{ return m_pItemInfoArray[iId].pszName; }
	const char*	Weapon_GetpszAmmo1	(const int iId)		{ return m_pItemInfoArray[iId].pszAmmo1; }
	const char* Weapon_GetpszAmmo2	(const int iId)		{ return m_pItemInfoArray[iId].pszAmmo2; }
};

extern CItems g_Items;

extern const char* gWeaponReference;
extern const char* gAmmoBoxReference;


#endif // _WPNMOD_ITEMS_H
