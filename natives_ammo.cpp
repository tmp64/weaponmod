/*
 * Half-Life Weapon Mod
 * Copyright (c) 2012 AGHL.RU Dev Team
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

#include "weaponmod.h"

void ActivateInfoTargetHooks();


/**
 * Register new ammobox in module.
 *
 * @param szName			The ammobox classname.
 * 
 * @return					The ID of registerd ammobox or -1 on failure. (integer)
 *
 * native wpnmod_register_ammobox(const szClassname[]);
 */
static cell AMX_NATIVE_CALL wpnmod_register_ammobox(AMX *amx, cell *params)
{
	if (g_iAmmoBoxIndex >= MAX_WEAPONS)
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "Ammobox limit reached.");
		return -1;
	}

	AmmoBoxInfoArray[g_iAmmoBoxIndex].pszName = STRING(ALLOC_STRING(MF_GetAmxString(amx, params[1], 0, NULL)));

	if (!g_InfoTargetHooksEnabled)
	{
		g_InfoTargetHooksEnabled = TRUE;
		ActivateInfoTargetHooks();
	}

	return g_iAmmoBoxIndex++;
}

/**
 * Register ammobox's forward.
 *
 * @param iAmmoboxID		The ID of registered ammobox.
 * @param iForward			Forward type to register.
 * @param szCallBack		The forward to call.
 *
 * native wpnmod_register_ammobox_forward(const iWeaponID, const e_AmmoFwds: iForward, const szCallBack[]);
 */
static cell AMX_NATIVE_CALL wpnmod_register_ammobox_forward(AMX *amx, cell *params)
{
	int iId = params[1];

	if (iId < 0 || iId >= MAX_WEAPONS)
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "Invalid ammobox id provided. Got: %d  Valid: 0 up to %d.", iId, MAX_WEAPONS - 1);
		return 0;
	}

	int Fwd = params[2];

	if (Fwd < 0 || Fwd >= Fwd_Ammo_End)
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "Function out of bounds. Got: %d  Max: %d.", iId, Fwd_Ammo_End - 1);
		return 0;
	}

	const char *funcname = MF_GetAmxString(amx, params[3], 0, NULL);

	AmmoBoxInfoArray[iId].iForward[Fwd] = MF_RegisterSPForwardByName
	(
		amx, 
		funcname, 
		FP_CELL,
		FP_CELL,
		FP_DONE
	);

	if (AmmoBoxInfoArray[iId].iForward[Fwd] == -1)
	{
		AmmoBoxInfoArray[iId].iForward[Fwd] = 0;
		MF_LogError(amx, AMX_ERR_NATIVE, "Function not found (%d, \"%s\").", Fwd, funcname);
		return 0;
	}

	return 1;
}

AMX_NATIVE_INFO Natives_Ammo[] = 
{
	{ "wpnmod_register_ammobox", wpnmod_register_ammobox},
	{ "wpnmod_register_ammobox_forward", wpnmod_register_ammobox_forward},

	{ NULL, NULL }
};
