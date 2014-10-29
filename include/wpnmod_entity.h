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

#define FWD_ENT_THINK		0
#define FWD_ENT_TOUCH		1
#define FWD_ENT_EXPLODE		2

class CEntityManager
{
public:

	class CEntity
	{
	public:
		int m_iThink;
		int m_iTouch;
		int m_iExplode;

		std::vector <std::string> m_TouchFilter;
	};

	CEntity** m_EntsData;

	//typedef std::map<int, std::map<std::string, TrieData*>> TInt2StrTrieMap;
	//TInt2StrTrieMap m_Tries;

	void AllocEntities(void)
	{
		m_EntsData = new CEntity*[gpGlobals->maxEntities + 1];

		for (int i = 0; i <= gpGlobals->maxEntities; ++i)
		{
			memset((m_EntsData[i] = new CEntity), 0, sizeof(CEntity));
		}
	}

	void FreeEntities(void)
	{
		if (m_EntsData)
		{
			for (int i = 0; i <= gpGlobals->maxEntities; ++i)
			{
				delete m_EntsData[i];
			}

			delete[] m_EntsData;
		}
	}

	void OnAllocEntPrivateData(edict_t *pEdict)
	{
		if (m_EntsData && IsValidPev(pEdict))
		{
			int iIndex = ENTINDEX(pEdict);

			m_EntsData[iIndex]->m_iThink = 0;
			m_EntsData[iIndex]->m_iTouch = 0;
			m_EntsData[iIndex]->m_iExplode = 0;

			m_EntsData[iIndex]->m_TouchFilter.clear();


			//memset((m_EntsData[ENTINDEX(pEdict)] = new CEntity), 0, sizeof(CEntity));

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

	void AddClassnameToTouchFilter(edict_t *pEdict, std::string strClassname)
	{
		// strClassname.trim();
		strClassname.erase(strClassname.find_last_not_of(" \n\r\t") + 1);

		if (!strClassname.empty() && IsValidPev(pEdict))
		{
			CEntity *p = m_EntsData[ENTINDEX(pEdict)];
			p->m_TouchFilter.push_back(strClassname);
		}
	}

	void SetAmxxForward(edict_t *pEdict, int iForwardType, int iForwardID)
	{
		if (IsValidPev(pEdict))
		{
			CEntity *p = m_EntsData[ENTINDEX(pEdict)];

			switch (iForwardType)
			{
			case FWD_ENT_THINK:
				p->m_iThink = iForwardID;
				break;
			case FWD_ENT_TOUCH:
				p->m_iTouch = iForwardID;
				break;
			case FWD_ENT_EXPLODE:
				p->m_iExplode = iForwardID;
				break;
			}
		}
	}

	void ExecuteAmxxForward(edict_t *pEdict, int iForwardType, void *pVar = NULL)
	{
		if (!IsValidPev(pEdict))
		{
			return;
		}

		CEntity *p = m_EntsData[ENTINDEX(pEdict)];

		switch (iForwardType)
		{
		case FWD_ENT_EXPLODE: // pVar - explosion trace result
			if (p->m_iExplode)
			{
				MF_ExecuteForward
				(
					p->m_iExplode,
					static_cast<cell>(ENTINDEX(pEdict)),
					reinterpret_cast<cell>(((TraceResult*)pVar)),
					MF_PrepareCellArrayA(reinterpret_cast<cell *>(&(((TraceResult*)pVar)->vecEndPos)), 3, false),
					MF_PrepareCellArrayA(reinterpret_cast<cell *>(&pEdict->v.origin), 3, false)
				);
			}
			break;
		case FWD_ENT_THINK:
			if (p->m_iThink)
			{
				// Entity is NOT a weapon
				if (!strstr(STRING(pEdict->v.classname), "weapon_"))
				{
					MF_ExecuteForward
					(
						p->m_iThink,
						static_cast<cell>(ENTINDEX(pEdict)),
						MF_PrepareCellArrayA(reinterpret_cast<cell *>(&pEdict->v.origin), 3, false)
					);
				}
				else // Entity is a weapon
				{
					edict_t* pPlayer = GetPrivateCbase(pEdict, pvData_pPlayer);
					MF_ExecuteForward
					(
						p->m_iThink,
						static_cast<cell>(ENTINDEX(pEdict)),
						static_cast<cell>(IsValidPev(pPlayer) ? ENTINDEX(pPlayer) : -1),
						static_cast<cell>(GetPrivateInt(pEdict, pvData_iClip)),
						static_cast<cell>(GetAmmoInventory(pPlayer, PrimaryAmmoIndex(pEdict))),
						static_cast<cell>(GetAmmoInventory(pPlayer, SecondaryAmmoIndex(pEdict)))
					);
				}
			}
			break;
		case FWD_ENT_TOUCH: // pVar - entity toucher
			if (p->m_iTouch)
			{
				if (p->m_TouchFilter.size())
				{
					bool bSkipTouch = true;
					for (int i = 0; i < (int)p->m_TouchFilter.size(); i++)
					{
						if (!strcmp(STRING(((edict_t *)pVar)->v.classname), p->m_TouchFilter[i].c_str()))
						{
							bSkipTouch = false;
						}
					}
					if (bSkipTouch)
					{
						break;
					}
				}
				MF_ExecuteForward
				(
					p->m_iTouch,
					static_cast<cell>(ENTINDEX(pEdict)),
					static_cast<cell>(ENTINDEX(((edict_t *)pVar))),
					static_cast<const char*>(STRING(((edict_t *)pVar)->v.classname)),
					MF_PrepareCellArrayA(reinterpret_cast<cell *>(&pEdict->v.origin), 3, false)
				);
			}
		}
	}
};

extern CEntityManager g_Entity;

extern edict_t* WpnMod_Ammo_Spawn(const char* szName, Vector vecOrigin, Vector vecAngles);
extern edict_t* WpnMod_Weapon_Spawn(const char* szName, Vector vecOrigin, Vector vecAngles);
extern edict_t* Wpnmod_SpawnItem(const char* szName, Vector vecOrigin, Vector vecAngles);

extern edict_t* CreateEntity(char *szName, Vector vecOrigin, Vector vecAngles);

#endif // _WPNMOD_ENTITY_H
