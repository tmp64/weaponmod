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

 
#if defined _hl_wpnmod_included
	#endinput
#endif
#define _hl_wpnmod_included

#if AMXX_VERSION_NUM >= 175
	#pragma reqlib weaponmod
#if !defined AMXMODX_NOAUTOLOAD
	#pragma loadlib weaponmod
#endif
#else
	#pragma library weaponmod
#endif


#include <hl_wpnmod_const>
#include <hl_wpnmod_deprecated>
#include <fakemeta>


#define SET_MODEL(%0,%1) engfunc(EngFunc_SetModel, %0, %1)
#define PRECACHE_MODEL(%0) engfunc(EngFunc_PrecacheModel,%0)
#define PRECACHE_SOUND(%0) engfunc(EngFunc_PrecacheSound,%0)
#define PRECACHE_GENERIC(%0) engfunc(EngFunc_PrecacheGeneric,%0)

/**
 * Register new weapon in module.
 *
 * @param szName           The weapon name.
 * @param iSlot            SlotID (1...5).
 * @param iPosition        NumberInSlot (1...5).
 * @param szAmmo1          Primary ammo type ("9mm", "uranium", "MY_AMMO" etc).
 * @param iMaxAmmo1        Max amount of primary ammo.
 * @param szAmmo2          Secondary ammo type.
 * @param iMaxAmmo2        Max amount of secondary ammo.
 * @param iMaxClip         Max amount of ammo in weapon's clip.
 * @param iFlags           Weapon's flags (see defines).
 * @param iWeight          This value used to determine this weapon's importance in autoselection.
 * 
 * @return                 The ID of registerd weapon or -1 on failure. (integer)
 */
native WpnMod_RegisterWeapon(const szName[], const iSlot, const iPosition, const szAmmo1[], const iMaxAmmo1, const szAmmo2[], const iMaxAmmo2, const iMaxClip, const iFlags, const iWeight);

/**
 * Register weapon's forward.
 *
 * @param iWeaponID        The ID of registered weapon.
 * @param iForward         Forward type to register.
 * @param szCallBack       The forward to call.
 */
native WpnMod_RegisterWeaponForward(const iWeaponID, const e_WpnFwds: iForward, const szCallBack[]);

/**
 * Register new ammobox in module.
 *
 * @param szName           The ammobox classname.
 * 
 * @return                 The ID of registerd ammobox or -1 on failure. (integer)
 */
native WpnMod_RegisterAmmoBox(const szClassname[]);

/**
 * Register ammobox's forward.
 *
 * @param iAmmoboxID       The ID of registered ammobox.
 * @param iForward         Forward type to register.
 * @param szCallBack       The forward to call.
 */
native WpnMod_RegisterAmmoBoxForward(const iWeaponID, const e_AmmoFwds: iForward, const szCallBack[]);

/**
 * Returns any ItemInfo variable for weapon. Use the e_ItemInfo_* enum.
 *
 * @param iId              The ID of registered weapon or weapon entity Id.
 * @param iInfoType        ItemInfo type.
 *
 * @return                 Weapon's ItemInfo variable.
 */
native WpnMod_GetWeaponInfo(const iId, const e_ItemInfo: iInfoType, any:...);

/**
 * Returns any AmmoInfo variable for ammobox. Use the e_AmmoInfo_* enum.
 *
 * @param iId              The ID of registered ammobox or ammobox entity Id.
 * @param iInfoType        e_AmmoInfo_* type.
 *
 * @return                 Ammobox's AmmoInfo variable.
 */
native WpnMod_GetAmmoBoxInfo(const iId, const e_AmmoInfo: iInfoType, any:...);

/**
 * Gets number of registered weapons.
 *
 * @return                 Number of registered weapons. (integer)
 */
native WpnMod_GetWeaponNum();

 /**
 * Gets number of registered ammoboxes.
 *
 * @return                 Number of registered ammoboxes. (integer)
 */
native WpnMod_GetAmmoBoxNum();

/**
 * Spawn an item by name.
 *
 * @param szClassName      Item's classname.
 * @param vecOrigin        Origin were to spawn.
 * @param vecAngles        Angles.
 *
 * @return                 Item entity index or -1 on failure. (integer)
 */
native WpnMod_CreateItem(const szClassName[], const Float: vecOrigin[3] = {0.0, 0.0, 0.0}, const Float: vecAngles[3] = {0.0, 0.0, 0.0});

/**
 * Default deploy function.
 *
 * @param iItem            Weapon's entity index.
 * @param szViewModel      Weapon's view model (V).
 * @param szWeaponModel    Weapon's player model (P).
 * @param iAnim            Sequence number of deploy animation.
 * @param szAnimExt        Animation extension.
 */
native WpnMod_DefaultDeploy(const iItem, const szViewModel[], const szWeaponModel[], const iAnim, const szAnimExt[]);

/**
 * Default reload function.
 *
 * @param iItem            Weapon's entity index.
 * @param iClipSize        Maximum weapon's clip size.
 * @param iAnim            Sequence number of reload animation.
 * @param flDelay          Reload delay time.
 */
native WpnMod_DefaultReload(const iItem, const iClipSize, const iAnim, const Float: flDelay);

/**
 * Sets the weapon so that it can play empty sound again.
 *
 * @param iItem            Weapon's entity index.
 */
native WpnMod_ResetEmptySound(const iItem);

/**
 * Plays the weapon's empty sound.
 *
 * @param iItem            Weapon's entity index.
 */
native WpnMod_PlayEmptySound(const iItem);

/**
* Get player's ammo inventory.
 *
 * @param iPlayer          Player id.
 * @param szAmmoName       Ammo type. ("9mm", "uranium", "MY_AMMO" etc..)
 *
 * @return				Amount of given ammo. (integer)
*/
native WpnMod_GetPlayerAmmo(const iPlayer, const szAmmoName[]);

/**
* Set player's ammo inventory.
 *
 * @param iPlayer          Player id.
 * @param szAmmoName       Ammo type. ("9mm", "uranium", "MY_AMMO" etc..)
 * @param iAmount          Ammo amount.
*/
native WpnMod_SetPlayerAmmo(const iPlayer, const szAmmoName[], const iAmount);

/**
 * Plays weapon's animation.
 *
 * @param iItem            Weapon's entity index.
 * @param iAnim            Sequence number.
 */
native WpnMod_SendWeaponAnim(const iItem, const iAnim);

/**
 * Sets the activity for player based on an event or current state.
 *
 * @param iPlayer          Player index.
 * @param iPlayerAnim      Animation (see PLAYER_ANIM constants).
 */
native WpnMod_SetPlayerAnim(const iPlayer, const PLAYER_ANIM: iPlayerAnim);
















/**
 * Sets weapon's think function. Analogue of set_task native.
 * 
 * Usage: 
 * 	wpnmod_set_think(iItem, "M249_CompleteReload");
 * 	set_pev(iItem, pev_nextthink, get_gametime() + 1.52);
 *
 * @param iItem				Weapon's entity index.
 * @param szCallBack		The forward to call.
 */
native wpnmod_set_think(const iItem, const szCallBack[]);

/**
 * Sets entity's touch calback. 
 * 
 * @param iEntity			Entity index.
 * @param szCallBack		The forward to call.
 */
native wpnmod_set_touch(const iEntity, const szCallBack[]);










/**
 * Fire bullets from player's weapon.
 *
 * @param iPlayer			Player index.
 * @param iAttacker			Attacker index (usualy it equal to previous param).
 * @param iShotsCount		Number of shots.
 * @param vecSpread			Spread.
 * @param flDistance		Max shot distance.
 * @param flDamage			Damage amount.
 * @param bitsDamageType	Damage type.
 * @param iTracerFreq		Tracer frequancy.
 */
native wpnmod_fire_bullets(const iPlayer, const iAttacker, const iShotsCount, const Float: vecSpread[3], const Float: flDistance, const Float: flDamage, const bitsDamageType, const iTracerFreq);

/**
 * Fire default contact grenade from player's weapon.
 *
 * @param iPlayer			Player index.
 * @param vecStart			Start position.
 * @param vecVelocity		Velocity.
 * @param szCallBack		The forward to call on explode.
 *
 * @return					Contact grenade index or -1 on failure. (integer)
 */
native wpnmod_fire_contact_grenade(const iPlayer, const Float: vecStart[3], const Float: vecVelocity[3], const szCallBack[] = "");

/**
 * Fire default timed grenade from player's weapon.
 *
 * @param iPlayer			Player index.
 * @param vecStart			Start position.
 * @param vecVelocity		Velocity.
 * @param flTime			Time before detonate.
 * @param szCallBack		The forward to call on explode.
 *
 * @return					Contact grenade index or -1 on failure. (integer)
 */
native wpnmod_fire_timed_grenade(const iPlayer, const Float: vecStart[3], const Float: vecVelocity[3], const Float: flTime = 3.0, const szCallBack[] = "");

/**
 * Make damage upon entities within a certain range.
 * 	Only damage ents that can clearly be seen by the explosion.
 *
 * @param vecSrc			Origin of explosion.
 * @param iInflictor		Entity which causes the damage impact.
 * @param iAttacker			Attacker index.
 * @param flDamage			Damage amount.
 * @param flRadius			Damage radius.
 * @param iClassIgnore		Class to ignore.
 * @param bitsDamageType	Damage type (DMG_BLAST and etc).
 */
native wpnmod_radius_damage(const Float: vecSrc[3], const iInflictor, const iAttacker, const Float: flDamage, const Float: flRadius, const iClassIgnore, const bitsDamageType);

/**
 * Same as wpnmod_radius_damage, but blocks 'ghost mines' and 'ghost nades'.
 *
 * @param vecSrc			Origin of explosion.
 * @param iInflictor		Entity which causes the damage impact.
 * @param iAttacker			Attacker index.
 * @param flDamage			Damage amount.
 * @param flRadius			Damage radius.
 * @param iClassIgnore		Class to ignore.
 * @param bitsDamageType	Damage type (DMG_BLAST and etc).
 */
native wpnmod_radius_damage2(const Float: vecSrc[3], const iInflictor, const iAttacker, const Float: flDamage, const Float: flRadius, const iClassIgnore, const bitsDamageType);

/**
 * Resets the global multi damage accumulator.
 */
 native wpnmod_clear_multi_damage();

 /**
 * Inflicts contents of global multi damage register on entity.
 *
 * @param iInflictor		Entity which causes the damage impact.
 * @param iAttacker			Attacker index.
 */
 native wpnmod_apply_multi_damage(const iInflictor, const iAttacker);

/**
 * Eject a brass from player's weapon.
 *
 * @param iPlayer			Player index.
 * @param iShellModelIndex	Index of precached shell's model.
 * @param iSoundtype		Bounce sound type (see defines).
 * @param flForwardScale	Forward scale value.
 * @param flUpScale			Up scale value.
 * @param flRightScale		Right scale value.
 */
native wpnmod_eject_brass(const iPlayer, const iShellModelIndex, const iSoundtype, const Float: flForwardScale, const Float: flUpScale, const Float: flRightScale);





/**
 * Returns index of random damage decal for given entity.
 *
 * @param iEntity		Entity.
 *
 * @return				Index of damage decal. (integer)
 */
native wpnmod_get_damage_decal(const iEntity);

/**
 * Get player's gun position. Result will set in vecResult.
 *
 * @param iPlayer			Player index.
 * @param vecResult			Calculated gun position.
 * @param flForwardScale	Forward scale value.
 * @param flUpScale			Up scale value.
 * @param flRightScale		Right scale value.
 */
native wpnmod_get_gun_position(const iPlayer, Float: vecResult[3], const Float: flForwardScale = 1.0, const Float: flRightScale = 1.0, const Float: flUpScale = 1.0);

/**
 * Explode and then remove entity.
 *
 * @param iEntity			Entity index.
 * @param bitsDamageType	Damage type.
 * @param szCallBack		The forward to call on explode.
 */
native wpnmod_explode_entity(const iEntity, const bitsDamageType = 0, const szCallBack[] = "");

/**
 * Draw decal by index or name on trace end.
 *
 * @param iTrace			Trace handler.
 * @param iDecalIndex		Decal index.
 * @param szDecalName		Decal name.
 */
native wpnmod_decal_trace(const iTrace, const iDecalIndex = -1, const szDecalName[] = "");

/**
 * Detects the texture of an entity from a direction.
 *
 * @param iEntity			Entity index that we want to get the texture.
 * @param vecSrc			The point from where the trace starts.
 * @param vecEnd			The point where the trace ends.
 * @param szTextureName		Buffer to save the texture name.
 * @param iLen				Buffer's length.
 */
native wpnmod_trace_texture(const iEntity, const Float: vecSrc[3], const Float: vecEnd[3], szTextureName[], const iLen);
