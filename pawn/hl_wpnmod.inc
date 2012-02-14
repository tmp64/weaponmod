/* AMX Mod X
*	Half-Life Weapon Mod module API.
*
* http://aghl.ru/forum/ - Russian Half-Life and Adrenaline Gamer Community
*
* This file is provided as is (no warranties)
*/

#if defined hl_wpnmod_included
  #endinput
#endif
#define _hl_wpnmod_included

#if AMXX_VERSION_NUM >= 175
 #pragma reqlib hl_wpnmod
 #if !defined AMXMODX_NOAUTOLOAD
  #pragma loadlib hl_wpnmod
 #endif
#else
 #pragma library hl_wpnmod
#endif


// Temp entity bounce sound types
#define TE_BOUNCE_NULL			0
#define TE_BOUNCE_SHELL			1
#define TE_BOUNCE_SHOTSHELL		2

// Gun volumes
#define LOUD_GUN_VOLUME			1000
#define NORMAL_GUN_VOLUME		600
#define QUIET_GUN_VOLUME		200

// Gun flash
#define	BRIGHT_GUN_FLASH		512
#define NORMAL_GUN_FLASH		256
#define	DIM_GUN_FLASH			128

// Weapon flags
#define ITEM_FLAG_SELECTONEMPTY		1
#define ITEM_FLAG_NOAUTORELOAD		2
#define ITEM_FLAG_NOAUTOSWITCHEMPTY	4
#define ITEM_FLAG_LIMITINWORLD		8
#define ITEM_FLAG_EXHAUSTIBLE		16 // A player can totally exhaust their ammo supply and lose this weapon

// Default spreads
#define VECTOR_CONE_1DEGREES	Float:{ 0.00873, 0.00873, 0.00873 }
#define VECTOR_CONE_2DEGREES	Float:{ 0.01745, 0.01745, 0.01745 }
#define VECTOR_CONE_3DEGREES	Float:{ 0.02618, 0.02618, 0.02618 }
#define VECTOR_CONE_4DEGREES	Float:{ 0.03490, 0.03490, 0.03490 }
#define VECTOR_CONE_5DEGREES	Float:{ 0.04362, 0.04362, 0.04362 }
#define VECTOR_CONE_6DEGREES	Float:{ 0.05234, 0.05234, 0.05234 }
#define VECTOR_CONE_7DEGREES	Float:{ 0.06105, 0.06105, 0.06105 }
#define VECTOR_CONE_8DEGREES	Float:{ 0.06976, 0.06976, 0.06976 }
#define VECTOR_CONE_9DEGREES	Float:{ 0.07846, 0.07846, 0.07846 }
#define VECTOR_CONE_10DEGREES	Float:{ 0.08716, 0.08716, 0.08716 }
#define VECTOR_CONE_15DEGREES	Float:{ 0.13053, 0.13053, 0.13053 }
#define VECTOR_CONE_20DEGREES	Float:{ 0.17365, 0.17365, 0.17365 }

enum PLAYER_ANIM
{
	PLAYER_IDLE,
	PLAYER_WALK,
	PLAYER_JUMP,
	PLAYER_SUPERJUMP,
	PLAYER_DIE,
	PLAYER_ATTACK1,
};

enum e_Forwards
{
	Fwd_Spawn,
	Fwd_CanDeploy,
	Fwd_Deploy,
	Fwd_Idle,
	Fwd_PrimaryAttack,
	Fwd_SecondaryAttack,
	Fwd_Reload,
	Fwd_CanHolster,
	Fwd_Holster,

	Fwd_End
};

enum e_Offsets
{
	// Weapon
	Offset_flStartThrow,
	Offset_flReleaseThrow,
	Offset_iChargeReady,
	Offset_iInAttack,
	Offset_iFireState,
	Offset_iFireOnEmpty,				// true when the gun is empty and the player is still holding down the attack key(s)
	Offset_flPumpTime,
	Offset_iInSpecialReload,			// Are we in the middle of a reload for the shotguns
	Offset_flNextPrimaryAttack,			// soonest time ItemPostFrame will call PrimaryAttack
	Offset_flNextSecondaryAttack,		// soonest time ItemPostFrame will call SecondaryAttack
	Offset_flTimeWeaponIdle,			// soonest time ItemPostFrame will call WeaponIdle
	Offset_iPrimaryAmmoType,			// "primary" ammo index into players m_rgAmmo[]
	Offset_iSecondaryAmmoType,			// "secondary" ammo index into players m_rgAmmo[]
	Offset_iClip,						// number of shots left in the primary weapon clip, -1 it not used
	Offset_iInReload,					// are we in the middle of a reload;
	Offset_iDefaultAmmo,				// how much ammo you get when you pick up this weapon as placed by a level designer.
	
	// Player
	Offset_flNextAttack,				// cannot attack again until this time
	Offset_iWeaponVolume,				// how loud the player's weapon is right now
	Offset_iWeaponFlash,				// brightness of the weapon flash
	
	Offset_End
};


/**
 * Register new weapon in module.
 *
 * @param szName		The weapon name.
 * @param iSlot			SlotID (1...5).
 * @param iPosition		NumberInSlot (1...5).
 * @param szAmmo1		Primary ammo type ("9mm", "uranium", "MY_AMMO" etc).
 * @param iMaxAmmo1		Max amount of primary ammo.
 * @param szAmmo2		Secondary ammo type.
 * @param iMaxAmmo2		Max amount of secondary ammo.
 * @param iMaxClip		Max amount of ammo in weapon's clip.
 * @param iFlags		Weapon's flags (see defines).
 * @param iWeight		This value used to determine this weapon's importance in autoselection.
 * 
 * @return				The ID of registerd weapon or 0 on failure. (integer)
 */
native wpnmod_register_weapon(const szName[], const iSlot, const iPosition, const szAmmo1[], const iMaxAmmo1, const szAmmo2[], const iMaxAmmo2, const iMaxClip, const iFlags, const iWeight);

/**
 * Register weapon's forward.
 *
 * @param iWeaponID		The ID of registered weapon.
 * @param iForward		Forward type to register.
 * @param szCallBack	The forward to call.
 */
native wpnmod_register_forward(const iWeaponID, const e_Forwards: iForward, const szCallBack[]);