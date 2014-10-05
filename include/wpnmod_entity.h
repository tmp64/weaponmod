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
/*
#define TRIE_DATA_UNSET 	0
#define TRIE_DATA_CELL		1
#define TRIE_DATA_STRING	2
#define TRIE_DATA_ARRAY		3

class TrieData
{
private:
	cell *m_data;
	cell m_cell;
	cell m_cellcount;
	int m_type;

	void needCells(cell cellcount)
	{
		if (m_cellcount < cellcount)
		{
			if (m_data != NULL)
			{
				free(m_data);
			}
			size_t neededbytes = cellcount * sizeof(cell);
			m_data = static_cast<cell *>(malloc(neededbytes));
			m_cellcount = cellcount;
		}
	}
public:
	void freeCells()
	{
		if (m_data)
		{
			free(m_data);
			m_data = NULL;
		}
		m_cellcount = 0;
	}
	TrieData() : m_data(NULL), m_cell(0), m_cellcount(0), m_type(TRIE_DATA_UNSET) { }
	TrieData(const TrieData &src) : m_data(src.m_data),
		m_cell(src.m_cell),
		m_cellcount(src.m_cellcount),
		m_type(src.m_type) { }
	~TrieData() { }

	int getType() { return m_type; }

	void setCell(cell value)
	{
		freeCells();

		m_cell = value;
		m_type = TRIE_DATA_CELL;
	}
	void setString(cell *value)
	{
		cell len = 0;
		cell *p = value;

		while (*p++ != 0)
		{
			len++;
		}
		len += 1; // zero terminator
		needCells(len);
		memcpy(m_data, value, sizeof(cell) * len);

		m_type = TRIE_DATA_STRING;
	}
	void setArray(cell *value, cell size)
	{
		if (size <= 0)
			return;

		needCells(size);
		memcpy(m_data, value, sizeof(cell) * size);

		m_type = TRIE_DATA_ARRAY;
	}
	bool getCell(cell *out)
	{
		if (m_type == TRIE_DATA_CELL)
		{
			*out = m_cell;
			return true;
		}

		return false;
	}
	bool getString(cell *out, cell max)
	{
		if (m_type == TRIE_DATA_STRING && max >= 0)
		{
			memcpy(out, m_data, (max > m_cellcount ? m_cellcount : max) * sizeof(cell));
			return true;
		}
		return false;
	}
	bool getArray(cell *out, cell max)
	{
		if (m_type == TRIE_DATA_ARRAY && max >= 0)
		{
			memcpy(out, m_data, (max > m_cellcount ? m_cellcount : max) * sizeof(cell));
			return true;
		}
		return false;
	}
	void clear()
	{
		freeCells();
		m_type = TRIE_DATA_UNSET;
	}
};
*/
class CEntityManager
{
public:

	class CEntity
	{
	public:

		CEntity()
		{
			m_iThink = 0;
			m_iTouch = 0;
			m_iExplode = 0;
		}

		~CEntity()
		{
			for (int i = 0; i < (int)m_TouchFilter.size(); i++)
			{
				m_TouchFilter[i].clear();
			}
		}

		int m_iThink;
		int m_iTouch;
		int m_iExplode;

		std::vector <std::string> m_TouchFilter;
	};

	typedef enum _forwardtypes
	{
		FORWARD_THINK,
		FORWARD_TOUCH,
		FORWARD_EXPLODE,
	} FORWARDTYPE;

	//typedef std::map<int, std::map<std::string, TrieData*>> TInt2StrTrieMap;

	bool m_bAlloced;
	CEntity** m_EntsData;
	//TInt2StrTrieMap m_Tries;

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

	void OnAllocEntPrivateData(edict_t *pEdict)
	{
		if (IsEntityValid(pEdict))
		{
			memset((m_EntsData[ENTINDEX(pEdict)] = new CEntity), 0, sizeof(CEntity));

			/*
			m_Tries[ENTINDEX(pEdict)]["lol"] = new TrieData;
			m_Tries[ENTINDEX(pEdict)]["lol2"] = new TrieData;
			m_Tries[ENTINDEX(pEdict)]["lol3"] = new TrieData;
			m_Tries[ENTINDEX(pEdict)]["lol4"] = new TrieData;


			TInt2StrTrieMap::iterator i = m_Tries.begin();
			*/

			/*
			const char *key = "lol";

			if (m_Tries[0][key] == NULL)
			{
				m_Tries[0][key] = new TrieData;
				printf2("WTF %d\n", ENTINDEX(pEdict));
			}
			else
			{
				m_Tries[0][key]->setCell(8);
				delete m_Tries[0][key];
			}
			*/
		}
	}

	void OnFreeEntPrivateData(edict_t *pEdict)
	{
		if (IsEntityValid(pEdict))
		{
			int iIndex = ENTINDEX(pEdict);
			delete m_EntsData[iIndex];
			m_EntsData[iIndex] = NULL;
		}
	}

	void AddClassnameToTouchFilter(edict_t *pEdict, std::string strClassname)
	{
		if (IsEntityValid(pEdict))
		{
			CEntity *p = m_EntsData[ENTINDEX(pEdict)];
			p->m_TouchFilter.push_back(strClassname);
		}
	}

	void SetAmxxForward(edict_t *pEdict, FORWARDTYPE forwardType, int iForwardID)
	{
		if (IsEntityValid(pEdict))
		{
			CEntity *p = m_EntsData[ENTINDEX(pEdict)];

			switch (forwardType)
			{
			case FORWARD_THINK:
				p->m_iThink = iForwardID;
				break;

			case FORWARD_TOUCH:
				p->m_iTouch = iForwardID;
				break;

			case FORWARD_EXPLODE:
				p->m_iExplode = iForwardID;
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

		CEntity *ent = m_EntsData[ENTINDEX(pEdict)];

		switch (forwardType)
		{
		case FORWARD_TOUCH:
			if (!ent->m_iTouch)
			{
				return;
			}
			va_list p;
			edict_t *pToucher;
			va_start(p, forwardType);
			pToucher = va_arg(p, edict_t*);
			va_end(p);
			if (ent->m_TouchFilter.size())
			{
				bool bSkipTouch = true;
				for (int i = 0; i < (int)ent->m_TouchFilter.size(); i++)
				{
					if (!strcmp(STRING(pToucher->v.classname), ent->m_TouchFilter[i].c_str()))
					{
						bSkipTouch = false;
					}
				}
				if (bSkipTouch)
				{
					return;
				}
			}
			MF_ExecuteForward
			(
				ent->m_iTouch,
				static_cast<cell>(ENTINDEX(pEdict)),
				static_cast<cell>(ENTINDEX(pToucher))
			);
			break;
		case FORWARD_EXPLODE:
			if (ent->m_iExplode)
			{
				va_list p;
				va_start(p, forwardType);
				MF_ExecuteForward
				(
					ent->m_iExplode,
					static_cast<cell>(ENTINDEX(pEdict)),
					reinterpret_cast<cell>(&va_arg(p, TraceResult))
				);
				va_end(p);
			}
			break;
		case FORWARD_THINK:
			if (ent->m_iThink)
			{
				if (!strstr(STRING(pEdict->v.classname), "weapon_"))
				{
					MF_ExecuteForward
					(
						ent->m_iThink,
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
						ent->m_iThink,
						static_cast<cell>(ENTINDEX(pEdict)),
						static_cast<cell>(IsValidPev(pPlayer) ? ENTINDEX(pPlayer) : -1),
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

extern edict_t* Ammo_Spawn(const char* szName, Vector vecOrigin, Vector vecAngles);
extern edict_t* Weapon_Spawn(const char* szName, Vector vecOrigin, Vector vecAngles);
extern edict_t* Wpnmod_SpawnItem(const char* szName, Vector vecOrigin, Vector vecAngles);

#endif // _WPNMOD_ENTITY_H
