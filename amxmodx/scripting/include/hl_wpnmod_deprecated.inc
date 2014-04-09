//*********************************************************************
//*                  These natives are DEPRECATED!                    *
//*         They placed here only for backward compatibility.         *
//*                        DON'T USE THEM!!!                          *
//*********************************************************************

#if defined _hl_wpnmod_deprecated_included
	#endinput
#endif

#define _hl_wpnmod_deprecated_included

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

native wpnmod_register_weapon(const szName[], const iSlot, const iPosition, const szAmmo1[], const iMaxAmmo1, const szAmmo2[], const iMaxAmmo2, const iMaxClip, const iFlags, const iWeight);
native wpnmod_register_weapon_forward(const iWeaponID, const e_WpnFwds: iForward, const szCallBack[]);
native wpnmod_register_ammobox(const szClassname[]);
native wpnmod_register_ammobox_forward(const iWeaponID, const e_AmmoFwds: iForward, const szCallBack[]);
native wpnmod_get_weapon_info(const iId, const e_ItemInfo: iInfoType, any:...);
native wpnmod_get_ammobox_info(const iId, const e_AmmoInfo: iInfoType, any:...);
native wpnmod_get_weapon_count();
native wpnmod_get_ammobox_count();
native wpnmod_create_item(const szName[], const Float: vecOrigin[3] = {0.0, 0.0, 0.0}, const Float: vecAngles[3] = {0.0, 0.0, 0.0});
native wpnmod_set_anim_ext(const iPlayer, const szAnimExt[]);
native wpnmod_get_anim_ext(const iPlayer, szDest[], iMaxLen);
native wpnmod_set_offset_int(const iEntity, const e_Offsets: iOffset, const iValue);
native wpnmod_get_offset_int(const iEntity, const e_Offsets: iOffset);
native wpnmod_set_offset_float(const iEntity, const e_Offsets: iOffset, const Float: flValue);
native Float: wpnmod_get_offset_float(const iEntity, const e_Offsets: iOffset);
native wpnmod_set_offset_cbase(const iEntity, const e_CBase: iOffset, const iValue, const iExtraOffset = 0);
native wpnmod_get_offset_cbase(const iEntity, const e_CBase: iOffset, const iExtraOffset = 0);
native wpnmod_default_deploy(const iItem, const szViewModel[], const szWeaponModel[], const iAnim, const szAnimExt[]);
native wpnmod_default_reload(const iItem, const iClipSize, const iAnim, const Float: flDelay);
native wpnmod_reset_empty_sound(const iItem);
native wpnmod_play_empty_sound(const iItem);
native wpnmod_get_player_ammo(const iPlayer, const szAmmoName[]);
native wpnmod_set_player_ammo(const iPlayer, const szAmmoName[], const iAmount);
native wpnmod_send_weapon_anim(const iItem, const iAnim);
native wpnmod_set_player_anim(const iPlayer, const PLAYER_ANIM: iPlayerAnim);
