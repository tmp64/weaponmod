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


#ifndef _WPNMOD_ENTITY_H
#define _WPNMOD_ENTITY_H

#include "wpnmod_config.h"
#include "wpnmod_utils.h"


extern edict_t* Ammo_Spawn(const char* szName, Vector vecOrigin, Vector vecAngles);
extern edict_t* Weapon_Spawn(const char* szName, Vector vecOrigin, Vector vecAngles);
extern edict_t* Wpnmod_SpawnItem(const char* szName, Vector vecOrigin, Vector vecAngles);


class CEntityManager
{
private:

	struct EntField
	{
		const char *pKey;
		void *pValue;
	};

	class CEntity
	{
	public:
		CEntity()
		{
			m_iThink = NULL;
			m_iTouch = NULL;
			m_iExplode = NULL;
		}

		int m_iThink;
		int m_iTouch;
		int m_iExplode;

		std::vector <EntField*> m_pFields;
	};

	bool m_bAlloced;
	CEntity** m_EntsData;

public:

	typedef enum _forwardtypes
	{
		FORWARD_THINK,
		FORWARD_TOUCH,
		FORWARD_EXPLODE,
	} FORWARDTYPE;

	CEntityManager()
	{
		m_bAlloced = false;
	}

	void AllocEntities(void)
	{
		if (!m_bAlloced)
		{
			m_bAlloced = true;
			m_EntsData = new CEntity*[gpGlobals->maxEntities + 1];
		}
	}

	void FreeEntities(void)
	{
		for (int i = 0; i <= gpGlobals->maxEntities; ++i)
		{
			delete m_EntsData[i];
		}

		delete [] m_EntsData;
	}

	bool IsEntityValid(edict_t *pEdict)
	{
		return m_bAlloced && IsValidPev(pEdict);
	}

	bool EntitySetField(edict_t *pEdict, const char* pKey, void *pValue)
	{
		if (!IsEntityValid(pEdict))
		{
			return false;
		}

		EntField *p = new EntField;

		p->pKey = pKey;
		p->pValue = pValue;

		m_EntsData[ENTINDEX(pEdict)]->m_pFields.push_back(p);

		return true;
	}

	void *EntityGetField(edict_t *pEdict, const char* pKey)
	{
		if (IsEntityValid(pEdict))
		{
			std::vector <EntField*> pFields = m_EntsData[ENTINDEX(pEdict)]->m_pFields;

			for (int i = 0; i < (int)pFields.size(); i++)
			{
				if (!strcmp(pFields[i]->pKey, pKey))
				{
					return pFields[i]->pValue;
				}
			}
		}

		return NULL;
	}

	void OnAllocEntPrivateData(edict_t *pEdict)
	{
		if (IsEntityValid(pEdict))
		{
			memset((m_EntsData[ENTINDEX(pEdict)] = new CEntity), 0, sizeof(CEntity));
		}
	}

	void OnFreeEntPrivateData(edict_t *pEdict)
	{
		if (IsEntityValid(pEdict))
		{
			int iIndex = ENTINDEX(pEdict);

			for (int i = 0; i < (int)m_EntsData[iIndex]->m_pFields.size(); i++)
			{
				if (strstr(m_EntsData[iIndex]->m_pFields[i]->pKey, "m_sz"))
				{
					delete [] m_EntsData[iIndex]->m_pFields[i]->pValue;
				}
				else
				{
					delete m_EntsData[iIndex]->m_pFields[i]->pValue;
				}
				
				delete m_EntsData[iIndex]->m_pFields[i];
			}

			m_EntsData[iIndex]->m_pFields.clear();
			delete m_EntsData[iIndex];
			m_EntsData[iIndex] = NULL;
		}
	}

	void SetAmxxForward(edict_t *pEdict, FORWARDTYPE forwardType, int iForwardID)
	{
		if (IsEntityValid(pEdict))
		{
			switch (forwardType)
			{
			case FORWARD_THINK:
				m_EntsData[ENTINDEX(pEdict)]->m_iThink = iForwardID;
				break;

			case FORWARD_TOUCH:
				m_EntsData[ENTINDEX(pEdict)]->m_iTouch = iForwardID;
				break;

			case FORWARD_EXPLODE:
				m_EntsData[ENTINDEX(pEdict)]->m_iExplode = iForwardID;
				break;
			}
		}
	}

	void ExecuteAmxxForward(edict_t *pEdict, FORWARDTYPE forwardType, ...)
	{
		if (!IsEntityValid(pEdict))
		{
			return;
		}

		switch (forwardType)
		{
		case FORWARD_TOUCH:
			if (m_EntsData[ENTINDEX(pEdict)]->m_iTouch)
			{
				va_list p;
				va_start(p, forwardType);

				MF_ExecuteForward
				(
					m_EntsData[ENTINDEX(pEdict)]->m_iTouch,

					static_cast<cell>(ENTINDEX(pEdict)),
					static_cast<cell>(ENTINDEX(va_arg(p, edict_t*)))
				);

				va_end(p);
			}
			break;

		case FORWARD_EXPLODE:
			if (m_EntsData[ENTINDEX(pEdict)]->m_iExplode)
			{
				va_list p;
				va_start(p, forwardType);

				MF_ExecuteForward
				(
					m_EntsData[ENTINDEX(pEdict)]->m_iExplode,
					
					static_cast<cell>(ENTINDEX(pEdict)),
					reinterpret_cast<cell>(&va_arg(p, TraceResult))
				);

				va_end(p);
			}
			break;

		case FORWARD_THINK:
			if (m_EntsData[ENTINDEX(pEdict)]->m_iThink)
			{
				if (!strstr(STRING(pEdict->v.classname), "weapon_"))
				{
					MF_ExecuteForward
					(
						m_EntsData[ENTINDEX(pEdict)]->m_iThink,

						static_cast<cell>(ENTINDEX(pEdict)),
						static_cast<cell>(-1),
						static_cast<cell>(-1),
						static_cast<cell>(-1),
						static_cast<cell>(-1)
					);
				}
				else
				{
					edict_t* pPlayer = GetPrivateCbase(pEdict, pvData_pPlayer);

					MF_ExecuteForward
					(
						m_EntsData[ENTINDEX(pEdict)]->m_iThink,

						static_cast<cell>(ENTINDEX(pEdict)),
						static_cast<cell>(ENTINDEX(pPlayer)),
						static_cast<cell>(GetPrivateInt(pEdict, pvData_iClip)),
						static_cast<cell>(GetAmmoInventory(pPlayer, PrimaryAmmoIndex(pEdict))),
						static_cast<cell>(GetAmmoInventory(pPlayer, SecondaryAmmoIndex(pEdict)))
					);
				}
			}
		}
	}
};

extern CEntityManager g_Entitys;

#endif // _WPNMOD_ENTITY_H
