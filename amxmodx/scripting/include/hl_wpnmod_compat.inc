/**
 * Half-Life Weapon Mod Compatibility include
 * Copyright (c) 2012 - 2014 AGHL.RU Dev Team
 *
 *  These natives placed here only for backward compatibility. DON'T USE THEM!!!
 */
 
 
#if defined _hl_wpnmod_compat_included
	#endinput
#endif

#define _hl_wpnmod_compat_included


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
#define	wpnmod_register_weapon WpnMod_RegisterWeapon

/**
 * Register weapon's forward.
 *
 * @param iWeaponID        The ID of registered weapon.
 * @param iForward         Forward type to register.
 * @param szCallBack       The forward to call.
 */
#define wpnmod_register_weapon_forward WpnMod_RegisterWeaponForward

/**
 * Returns any ItemInfo variable for weapon. Use the e_ItemInfo_* enum.
 *
 * @param iId              The ID of registered weapon or weapon entity Id.
 * @param iInfoType        ItemInfo type.
 *
 * @return                 Weapon's ItemInfo variable.
 */
#define wpnmod_get_weapon_info WpnMod_GetWeaponInfo

/**
 * Gets number of registered weapons.
 *
 * @return                 Number of registered weapons. (integer)
 */
#define wpnmod_get_weapon_count WpnMod_GetWeaponNum

/**
 * Register new ammobox in module.
 *
 * @param szName           The ammobox classname.
 * 
 * @return                 The ID of registerd ammobox or -1 on failure. (integer)
 */
#define wpnmod_register_ammobox WpnMod_RegisterAmmoBox

/**
 * Register ammobox's forward.
 *
 * @param iAmmoboxID       The ID of registered ammobox.
 * @param iForward         Forward type to register.
 * @param szCallBack       The forward to call.
 */
#define wpnmod_register_ammobox_forward WpnMod_RegisterAmmoBoxForward

/**
 * Gets number of registered weapons.
 *
 * @return                 Number of registered weapons. (integer)
 */
#define wpnmod_get_ammobox_info WpnMod_GetAmmoBoxInfo

/**
 * Gets number of registered ammoboxes.
 *
 * @return                 Number of registered ammoboxes. (integer)
 */
#define wpnmod_get_ammobox_count WpnMod_GetAmmoBoxNum

/**
 * Spawn an item by name.
 *
 * @param szClassName      Item's classname.
 * @param vecOrigin        Origin were to spawn.
 * @param vecAngles        Angles.
 *
 * @return                 Item entity index or -1 on failure. (integer)
 */
#define	wpnmod_create_item WpnMod_CreateItem

/**
 * Default deploy function.
 *
 * @param iItem            Weapon's entity index.
 * @param szViewModel      Weapon's view model (V).
 * @param szWeaponModel    Weapon's player model (P).
 * @param iAnim            Sequence number of deploy animation.
 * @param szAnimExt        Animation extension.
 */
#define	wpnmod_default_deploy WpnMod_DefaultDeploy

/**
 * Default reload function.
 *
 * @param iItem            Weapon's entity index.
 * @param iClipSize        Maximum weapon's clip size.
 * @param iAnim            Sequence number of reload animation.
 * @param flDelay          Reload delay time.
 */
#define	wpnmod_default_reload WpnMod_DefaultReload

/**
 * Sets the weapon so that it can play empty sound again.
 *
 * @param iItem            Weapon's entity index.
 */
#define	wpnmod_reset_empty_sound WpnMod_ResetEmptySound

/**
 * Plays the weapon's empty sound.
 *
 * @param iItem            Weapon's entity index.
 */
#define	wpnmod_play_empty_sound WpnMod_PlayEmptySound

/**
* Get player's ammo inventory.
 *
 * @param iPlayer          Player id.
 * @param szAmmoName       Ammo type. ("9mm", "uranium", "MY_AMMO" etc..)
 *
 * @return                 Amount of given ammo. (integer)
*/
#define	wpnmod_get_player_ammo WpnMod_GetPlayerAmmo

/**
* Set player's ammo inventory.
 *
 * @param iPlayer          Player id.
 * @param szAmmoName       Ammo type. ("9mm", "uranium", "MY_AMMO" etc..)
 * @param iAmount          Ammo amount.
*/
#define	wpnmod_set_player_ammo WpnMod_SetPlayerAmmo

/**
 * Plays weapon's animation.
 *
 * @param iItem            Weapon's entity index.
 * @param iAnim            Sequence number.
 */
#define	wpnmod_send_weapon_anim WpnMod_SendWeaponAnim

/**
 * Sets the activity for player based on an event or current state.
 *
 * @param iPlayer          Player index.
 * @param iPlayerAnim      Animation (see PLAYER_ANIM constants).
 */
#define	wpnmod_set_player_anim WpnMod_SetPlayerAnim

stock const e_pvData: __OffsetsCompat_CBase[CBase_End] =
{
	PV_ENT_pPlayer,
	PV_ENT_pNext,
	PV_ENT_rgpPlayerItems,
	PV_ENT_pActiveItem,
	PV_ENT_pLastItem,
};

stock const e_pvData: __OffsetsCompat[Offset_End] =
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
 * Set animation extension for player.
 *
 * @param iPlayer          Player id.
 * @param szAnimExt[]      Animation extension prefix.
 */
stock wpnmod_set_anim_ext(const iPlayer, const szAnimExt[])
{
	WpnMod_SetPrivateData(iPlayer, PV_SZ_szAnimExtention, szAnimExt);
}

/**
 * Get animation extension for player.
 *
 * @param iPlayer          Player id.
 * @param szDest[]         Buffer.
 * @param iMaxLen          Max buffer size.
 */
stock wpnmod_get_anim_ext(const iPlayer, szDest[], iMaxLen = sizeof szDest)
{
	WpnMod_GetPrivateData(iPlayer, PV_SZ_szAnimExtention, szDest, iMaxLen);
}

/**
 * Sets an integer from private data.
 *
 * @param iEntity          Entity index.
 * @param iOffset          Offset (see e_Offsets constants).
 * @param iValue           Value.
 */
stock wpnmod_set_offset_int(const iEntity, const e_Offsets: iOffset, const iValue)
{
	WpnMod_SetPrivateData(iEntity, __OffsetsCompat[iOffset], iValue);
}

/**
 * Returns an integer from private data.
 *
 * @param iEntity          Entity index.
 * @param iOffset          Offset (see e_Offsets constants).
 * 
 * @return                 Value from private data. (integer)
 */
stock wpnmod_get_offset_int(const iEntity, const e_Offsets: iOffset)
{
	return WpnMod_GetPrivateData(iEntity, __OffsetsCompat[iOffset]);
}

/**
 * Sets a float from private data.
 *
 * @param iEntity          Entity index.
 * @param iOffset          Offset (see e_Offsets constants).
 * @param flValue          Value.
 */
stock wpnmod_set_offset_float(const iEntity, const e_Offsets: iOffset, const Float: flValue)
{
	WpnMod_SetPrivateData(iEntity, __OffsetsCompat[iOffset], flValue);
}

/**
 * Returns a float from private data.
 *
 * @param iEntity          Entity index.
 * @param iOffset          Offset (see e_Offsets constants).
 * 
 * @return                 Value from private data. (float)
 */
stock Float: wpnmod_get_offset_float(const iEntity, const e_Offsets: iOffset)
{
	return WpnMod_GetPrivateData(iEntity, __OffsetsCompat[iOffset]);
}

/**
 * Set the corresponding cbase field in private data with the index.
 *
 * @param iEntity           The entity to examine the private data.
 * @param iOffset           Offset (See e_CBase constants).
 * @param iValue            The index to store.
 * @param iExtraOffset      The extra offset.
 */
stock wpnmod_set_offset_cbase(const iEntity, const e_CBase: iOffset, const iValue, const iExtraOffset = 0)
{
	WpnMod_SetPrivateData(iEntity, __OffsetsCompat_CBase[iOffset], flValue, iExtraOffset);
}

/**
 * This will return an index of the corresponding cbase field in private data.
 *
 * @param iEntity            The entity to examine the private data.
 * @param iOffset            Offset (See e_CBase constants).
 * @param iExtraOffset       The extra offset.
 *
 * @return                   Value from private data. (integer)
 */
stock wpnmod_get_offset_cbase(const iEntity, const e_CBase: iOffset, const iExtraOffset = 0)
{
	return WpnMod_GetPrivateData(iEntity, __OffsetsCompat_CBase[iOffset], iExtraOffset);
}

/**
 * Sets entity's think function. Analogue of set_task native.
 * 
 * Usage: 
 * 	wpnmod_set_think(iItem, "M249_CompleteReload");
 * 	set_pev(iItem, pev_nextthink, get_gametime() + 1.52);
 *
 * @param iItem              Entity's entity index.
 * @param szCallBack         The forward to call.
 */
#define wpnmod_set_think WpnMod_SetThink

/**
 * Sets entity's touch calback. 
 * 
 * @param iEntity            Entity's index.
 * @param szCallBack         The forward to call.
 */
#define wpnmod_set_touch WpnMod_SetTouch

/**
 * Fire bullets from player's weapon.
 *
 * @param iPlayer            Player index.
 * @param iAttacker          Attacker index (usualy it equal to previous param).
 * @param iShotsCount        Number of shots.
 * @param vecSpread          Spread.
 * @param flDistance         Max shot distance.
 * @param flDamage           Damage amount.
 * @param bitsDamageType     Damage type.
 * @param iTracerFreq        Tracer frequancy.
 */
#define wpnmod_fire_bullets WpnMod_FireBullets











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
