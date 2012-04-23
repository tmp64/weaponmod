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

 
#if defined _hl_wpnmod_const_included
	#endinput
#endif

#define _hl_wpnmod_const_included


// Temp entity bounce sound types
#define TE_BOUNCE_NULL					0
#define TE_BOUNCE_SHELL					1
#define TE_BOUNCE_SHOTSHELL				2

// Gun volumes
#define LOUD_GUN_VOLUME					1000
#define NORMAL_GUN_VOLUME				600
#define QUIET_GUN_VOLUME				200

// Gun flash
#define	BRIGHT_GUN_FLASH				512
#define NORMAL_GUN_FLASH				256
#define	DIM_GUN_FLASH					128

// Weapon flags
#define ITEM_FLAG_SELECTONEMPTY			1
#define ITEM_FLAG_NOAUTORELOAD			2
#define ITEM_FLAG_NOAUTOSWITCHEMPTY		4
#define ITEM_FLAG_LIMITINWORLD			8
#define ITEM_FLAG_EXHAUSTIBLE			16 // A player can totally exhaust their ammo supply and lose this weapon

// Default spreads
#define VECTOR_CONE_1DEGREES			Float:{ 0.00873, 0.00873, 0.00873 }
#define VECTOR_CONE_2DEGREES			Float:{ 0.01745, 0.01745, 0.01745 }
#define VECTOR_CONE_3DEGREES			Float:{ 0.02618, 0.02618, 0.02618 }
#define VECTOR_CONE_4DEGREES			Float:{ 0.03490, 0.03490, 0.03490 }
#define VECTOR_CONE_5DEGREES			Float:{ 0.04362, 0.04362, 0.04362 }
#define VECTOR_CONE_6DEGREES			Float:{ 0.05234, 0.05234, 0.05234 }
#define VECTOR_CONE_7DEGREES			Float:{ 0.06105, 0.06105, 0.06105 }
#define VECTOR_CONE_8DEGREES			Float:{ 0.06976, 0.06976, 0.06976 }
#define VECTOR_CONE_9DEGREES			Float:{ 0.07846, 0.07846, 0.07846 }
#define VECTOR_CONE_10DEGREES			Float:{ 0.08716, 0.08716, 0.08716 }
#define VECTOR_CONE_15DEGREES			Float:{ 0.13053, 0.13053, 0.13053 }
#define VECTOR_CONE_20DEGREES			Float:{ 0.17365, 0.17365, 0.17365 }

// For CLASSIFY
#define	CLASS_NONE						0
#define CLASS_MACHINE					1
#define CLASS_PLAYER					2
#define	CLASS_HUMAN_PASSIVE				3
#define CLASS_HUMAN_MILITARY			4
#define CLASS_ALIEN_MILITARY			5
#define CLASS_ALIEN_PASSIVE				6
#define CLASS_ALIEN_MONSTER				7
#define CLASS_ALIEN_PREY				8
#define CLASS_ALIEN_PREDATOR			9
#define CLASS_INSECT					10
#define CLASS_PLAYER_ALLY				11
#define CLASS_PLAYER_BIOWEAPON			12 // hornets and snarks.launched by players
#define CLASS_ALIEN_BIOWEAPON			13 // hornets and snarks.launched by the alien menace

enum PLAYER_ANIM
{
	PLAYER_IDLE,
	PLAYER_WALK,
	PLAYER_JUMP,
	PLAYER_SUPERJUMP,
	PLAYER_DIE,
	PLAYER_ATTACK1,
};

enum e_AmmoFwds
{
	/**
	 * Forward params for all functions:
	 * 
	 * @param iItem				Ammobox's entity index.
	 */
	
	Fwd_Ammo_Spawn,					/* This is called whenever a ammobox entity is created. */
	Fwd_Ammo_AddAmmo,

	Fwd_Ammo_End
};

enum e_WpnFwds
{
	/**
	 * Forward params for all functions:
	 * 
	 * @param iItem				Weapon's entity index.
	 * @param iPlayer			Player index, owner of weapon.
	 * @param iClip				Amount of ammo in weapon's clip.
	 * @param iAmmo				Amount of ammo in backpack on a user for weapon.
	 */
	
	Fwd_Wpn_Spawn,					/* This is called whenever a weapon entity is created. */
	Fwd_Wpn_CanDeploy,				/* Whether or not this weapon can be deployed. */
	Fwd_Wpn_Deploy,					/* Deploys a weapon. */
	Fwd_Wpn_Idle,					/* Displays the idle animation for the weapon. */
	Fwd_Wpn_PrimaryAttack,			/* Called when the main attack of a weapon is triggered. */
	Fwd_Wpn_SecondaryAttack,		/* Called when the secondary attack of a weapon is triggered. */
	Fwd_Wpn_Reload,					/* Called when the weapon is reloaded. */
	Fwd_Wpn_CanHolster,				/* Whether or not the weapon can be holstered. */
	Fwd_Wpn_Holster,				/* Holsters a weapon. */
	Fwd_Wpn_IsUseable,				/* Whether or not the weapon is usable. (has ammo, etc.) */

	Fwd_Wpn_End
};

enum e_Offsets
{
	// Weapon
	Offset_flStartThrow,
	Offset_flReleaseThrow,
	Offset_iChargeReady,
	Offset_iInAttack,
	Offset_iFireState,
	Offset_iFireOnEmpty,				// True when the gun is empty and the player is still holding down the attack key(s).
	Offset_flPumpTime,
	Offset_iInSpecialReload,			// Are we in the middle of a reload for the shotguns.
	Offset_flNextPrimaryAttack,			// Soonest time ItemPostFrame will call PrimaryAttack.
	Offset_flNextSecondaryAttack,		// Soonest time ItemPostFrame will call SecondaryAttack.
	Offset_flTimeWeaponIdle,			// Soonest time ItemPostFrame will call WeaponIdle.
	Offset_iPrimaryAmmoType,			// "Primary" ammo index into players m_rgAmmo[].
	Offset_iSecondaryAmmoType,			// "Secondary" ammo index into players m_rgAmmo[].
	Offset_iClip,						// Number of shots left in the primary weapon clip, -1 it not used.
	Offset_iInReload,					// Are we in the middle of a reload.
	Offset_iDefaultAmmo,				// How much ammo you get when you pick up this weapon as placed by a level designer.
	
	// Player
	Offset_flNextAttack,				// Cannot attack again until this time.
	Offset_iWeaponVolume,				// How loud the player's weapon is right now.
	Offset_iWeaponFlash,				// Brightness of the weapon flash.
	Offset_iLastHitGroup,				// The last body region that took damage.
	Offset_iFOV,						// Field of view.
	
	Offset_End
};