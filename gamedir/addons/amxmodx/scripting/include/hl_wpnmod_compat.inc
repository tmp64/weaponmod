/** 
 * Who put these backwards... (c)
 */
 
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

#if defined _hl_wpnmod_compat_included
	#endinput
#endif

#define _hl_wpnmod_compat_included

#define	wpnmod_register_weapon WpnMod_RegisterWeapon
#define wpnmod_register_weapon_forward WpnMod_RegisterWeaponForward
#define wpnmod_get_weapon_info WpnMod_GetWeaponInfo
#define wpnmod_get_weapon_count WpnMod_GetWeaponNum
#define wpnmod_register_ammobox WpnMod_RegisterAmmoBox
#define wpnmod_register_ammobox_forward WpnMod_RegisterAmmoBoxForward
#define wpnmod_get_ammobox_info WpnMod_GetAmmoBoxInfo
#define wpnmod_get_ammobox_count WpnMod_GetAmmoBoxNum
#define	wpnmod_create_item WpnMod_CreateItem
#define	wpnmod_default_deploy WpnMod_DefaultDeploy
#define	wpnmod_default_reload WpnMod_DefaultReload
#define	wpnmod_reset_empty_sound WpnMod_ResetEmptySound
#define	wpnmod_play_empty_sound WpnMod_PlayEmptySound
#define	wpnmod_get_player_ammo WpnMod_GetPlayerAmmo
#define	wpnmod_set_player_ammo WpnMod_SetPlayerAmmo
#define	wpnmod_send_weapon_anim WpnMod_SendWeaponAnim
#define	wpnmod_set_player_anim WpnMod_SetPlayerAnim
#define wpnmod_set_think WpnMod_SetThink
#define wpnmod_set_touch WpnMod_SetTouch
#define wpnmod_fire_bullets WpnMod_FireBullets

#define	wpnmod_set_anim_ext(%0,%1) WpnMod_SetPrivateData(%0, PV_SZ_szAnimExtention, %1)
#define	wpnmod_get_anim_ext(%0,%1,%2) WpnMod_GetPrivateData(%0, PV_SZ_szAnimExtention, %1, %2)
#define	wpnmod_set_offset_int(%0,%1,%2) WpnMod_SetPrivateData(%0, e_pvData: __OffsetsCompat[%1], %2)
#define	wpnmod_get_offset_int(%0,%1) WpnMod_GetPrivateData(%0, e_pvData: __OffsetsCompat[%1])
#define	wpnmod_set_offset_float(%0,%1,%2) WpnMod_SetPrivateData(%0, e_pvData: __OffsetsCompat[%1], %2)
#define	wpnmod_get_offset_float(%0,%1) WpnMod_GetPrivateData(%0, e_pvData: __OffsetsCompat[%1])
#define wpnmod_set_offset_cbase(%0,%1,%2) WpnMod_SetPrivateData(%0, e_pvData: __OffsetsCompat_CBase[%1], %2)
#define wpnmod_get_offset_cbase _GET_CBASE_1:_GET_CBASE_0

// Credits to Y_Less
#define _GET_CBASE_0(%0,%1) _GET_CBASE_1:_GET_CBASE_0(%0,%1,0)
#define _GET_CBASE_1:_GET_CBASE_0(%0,%1,%2) WpnMod_GetPrivateData(%0, e_pvData: __OffsetsCompat_CBase[%1], %2)

enum e_CBase
{
	CBase_pPlayer,
	CBase_pNext,
	CBase_rgpPlayerItems,
	CBase_pActiveItem,
	CBase_pLastItem,
	CBase_End
};

enum e_Offsets
{
	Offset_flStartThrow,
	Offset_flReleaseThrow,
	Offset_iChargeReady,
	Offset_iInAttack,
	Offset_iFireState,
	Offset_iFireOnEmpty,
	Offset_flPumpTime,
	Offset_iInSpecialReload,
	Offset_flNextPrimaryAttack,
	Offset_flNextSecondaryAttack,
	Offset_flTimeWeaponIdle,
	Offset_iPrimaryAmmoType,
	Offset_iSecondaryAmmoType,
	Offset_iClip,
	Offset_iInReload,
	Offset_iDefaultAmmo,
	Offset_flNextAttack,
	Offset_iWeaponVolume,
	Offset_iWeaponFlash,
	Offset_iLastHitGroup,
	Offset_iFOV,
	Offset_iuser1,
	Offset_iuser2,
	Offset_iuser3,
	Offset_iuser4,
	Offset_fuser1,
	Offset_fuser2,
	Offset_fuser3,
	Offset_fuser4,
	Offset_End
};

stock const __OffsetsCompat_CBase[CBase_End] =
{
	PV_ENT_pPlayer,
	PV_ENT_pNext,
	PV_ENT_rgpPlayerItems,
	PV_ENT_pActiveItem,
	PV_ENT_pLastItem,
};

stock const __OffsetsCompat[Offset_End] =
{
	PV_FL_flStartThrow,
	PV_FL_flReleaseThrow,
	PV_INT_iChargeReady,
	PV_INT_iInAttack,
	PV_INT_iFireState,
	PV_INT_iFireOnEmpty,
	PV_FL_flPumpTime,
	PV_INT_iInSpecialReload,
	PV_FL_flNextPrimaryAttack,
	PV_FL_flNextSecondaryAttack,
	PV_FL_flTimeWeaponIdle,
	PV_INT_iPrimaryAmmoType,
	PV_INT_iSecondaryAmmoType,
	PV_INT_iClip,
	PV_INT_iInReload,
	PV_INT_iDefaultAmmo,
	PV_FL_flNextAttack,
	PV_INT_iWeaponVolume,
	PV_INT_iWeaponFlash,
	PV_INT_iLastHitGroup,
	PV_INT_iFOV,
	PV_INT_iuser1,
	PV_INT_iuser2,
	PV_INT_iuser3,
	PV_INT_iuser4,
	PV_FL_fuser1,
	PV_FL_fuser2,
	PV_FL_fuser3,
	PV_FL_fuser4
};







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
