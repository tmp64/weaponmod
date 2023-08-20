# Half-Life WeaponMod

This is a fork of WeaponMod with support for new GameDLL versions (8308+) and
BugfixedHL-Rebased.

## Differences from upstream
- Offsets are loaded from AMX Mod X instead of being hardcoded.
  - VTable offsets now use AMXX's game config system (`amxmodx/data/gamedata`).
- Support for latest version of HLDM.
- ReHLDS support (currently requires custom patch, see below).

## Requirements
- [AMX Mod X 1.10+](https://www.amxmodx.org/downloads-new.php?branch=master)
- ReHLDS on Linux
  - It must support `GetDispatch` hook. See below.

## Supported games
The module currently supports the following games. Pull requests with other
games and mods are welcome.

- Half-Life Deathmatch [Windows & Linux]
  - Windows requires [BugfixedHL-Rebased](https://github.com/tmp64/BugfixedHL-Rebased)
  - Linux requires [ReHLDS](https://github.com/dreamstalker/rehlds)

## ReHLDS support
The module relies on ReHLDS providing a hook for `GetDispatch` function. [It has
not been added to upstream yet.](https://github.com/dreamstalker/rehlds/pull/980)

For now, you can download the version that is supported by Weapon Mod
[here.](https://github.com/tmp64/rehlds/actions?query=branch%3Afeature%2Fgetdispatch-hook-ci)

## Credits
- Original module by KORD_12.7 & AGHL.ru Dev Team
- [AGHL.ru link](http://aghl.ru/forum/viewtopic.php?f=42&t=721)
- [Upstream SVN repo](https://svn.aghl.ru:8443/!/#HLModules/view/head/weaponmod/trunk)
  / [Git mirror](https://github.com/tmp64/weaponmod_mirror)
- 8308 fix by [@rtxa](https://github.com/rtxa/weaponmod_mirror/tree/fix-last-hlds-build-linux) (before gamedata support was added)

# Adding mod support
This guide provides some of the steps to add support for mods to WeaponMod.
It will mostly focus on adding support for Linux (since there are absolutely no
reasons to run servers on Windows).

## Prerequisites
- The mod's game DLL (`.so`) must be compiled with debug information.
- The mod must be compiled with a modern version of GCC (4.6+) or Clang.

## GameData
WeaponMod uses AMXX 1.9's new "game config" feature. It allows plugins and
modules to read various offsets from a centralized mod-specific config file
instead of hardcoding them. WeaponMod's config is located in
`amxmodx/data/gamedata/weaponmod.games`. This directory will later be refered
to as `weaponmod.games`.

In this guide you will need to make copies of files in `valve` for your mod.

1. Open `amxmodx/data/gamedata/weaponmod.games`
2. Create directory with you mod name: `yourmod`
3. Open `master.games.txt`
4. Create a comment with your mod's full name (like for HLDM).
5. For any file you create you need to add a record in `master.games.txt`.

## Selecting reference entities
You need to select a reference weapon and a reference ammo box. WeaponMod will
use them as bases for custom weapons. Most mods can use `weapon_crowbar` and
`ammo_rpgclip`.

1. Copy `weaponmod.games/valve/settings.txt` to
   `weaponmod.games/yourmod/settings.txt`.
2. Change reference entities.
3. Add the file to `weaponmod.games/master.games.txt`

## Generating entity offsets
Basically, you need to generate `amxmodx/data/gamedata/common.games/entities.games` for your
mod. [It seems that AMXX devs used some private tool for that.](https://github.com/alliedmodders/amxmodx/pull/771)
You can copy files from `valve` and hope for the best.

You may try to parse the output from `pahole` tool ([reference](https://forums.alliedmods.net/showthread.php?t=273210)).

## Generating VTable offsets
Same as previous, but for `amxmodx/data/gamedata/common.games/virtual.games`.

## Getting Weapon and Ammo VTable offsets
These offsets are not provided by AMXX and you will have to get them
yourself.

You need to get VTable offsets for classes `CBasePlayerWeapon` and `CBasePlayerItem`.

### Via `pahole`
1.  Install `pahole` on Linux.
2.  Pipe the output into a file. Ignore "not supported" messages.
    ```bash
    pahole ./hl.so > hl.so.symbols
    ```
3.  Open the file. Find substring `class EntityClassName :`
    ```cpp
    class CBasePlayerWeapon : public CBasePlayerItem {
    public:

        /* class CBasePlayerItem     <ancestor>; */      /*     0   140 */
        virtual int Save(class CBasePlayerWeapon *, class CSave &);

        virtual int Restore(class CBasePlayerWeapon *, class CRestore &);
    ```
4.  Scroll down to `/* vtable has XX entries:`. There's the VTable with
    offsets Note that it isn't sorted (for some reason).
    ```c
    [3] = Save((null)), 
    [4] = Restore((null)), 
    [58] = AddToPlayer((null)), 
    [59] = AddDuplicate((null)), 
    [76] = ExtractAmmo((null)), 
    [77] = ExtractClipAmmo((null)), 
    [78] = AddWeapon((null)), 
    [65] = UpdateItemInfo((null)), 
    [79] = PlayEmptySound((null)), 
    [80] = ResetEmptySound((null)), 
    [81] = SendWeaponAnim((null)), 
    [61] = CanDeploy((null)), 
    [82] = IsUseable((null)), 
    [67] = ItemPostFrame((null)), 
    [83] = PrimaryAttack((null)), 
    [84] = SecondaryAttack((null)), 
    [85] = Reload((null)), 
    [86] = WeaponIdle((null)), 
    [73] = UpdateClientData((null)), 
    [87] = RetireWeapon((null)), 
    [88] = ShouldWeaponIdle((null)), 
    [64] = Holster((null)), 
    [89] = UseDecrement((null)), 
    [71] = PrimaryAmmoIndex((null)), 
    [72] = SecondaryAmmoIndex((null)), 
    [74] = GetWeaponPtr((null)), 
    ```

### Via `gdb`
We will use `gdb` to print vtable as an array and then use line numbers as
offsets.

1.  Install `gdb` on Linux.
2.  Open the Game DLL with GDB: `gdb ./hl.so`
3.  Type in GDB console (replace `EntityClassName` with class name):
    ```
    set pagination off
    set print array on
    print *(int**)&'vtable for EntityClassName'@100
    ```
4.  You will get 100 lines of array items. Copy them to a text editor that
    shows line numbers (e.g. VS Code, Notepad++).
    ```
    1. $1 =   {0x0,
    2. 0x1bba10 <typeinfo for CBasePlayerWeapon>,
    3. 0x58738 <CBaseEntity::Spawn()>,
    4. 0x5874c <CBaseEntity::Precache()>,
    5. 0x129fc8 <CBaseDelay::KeyValue(KeyValueData_s*)>,
    6. 0x149f2c <CBasePlayerWeapon::Save(CSave&)>,
    7. 0x149f8c <CBasePlayerWeapon::Restore(CRestore&)>,
    8. 0x533e4 <CBaseEntity::ObjectCaps()>,
    ...
    ```
5.  Remove first 3 lines (`0x0`, `typeinfo` and the next element)
    ```
    1. 0x5874c <CBaseEntity::Precache()>,
    2. 0x129fc8 <CBaseDelay::KeyValue(KeyValueData_s*)>,
    3. 0x149f2c <CBasePlayerWeapon::Save(CSave&)>,
    4. 0x149f8c <CBasePlayerWeapon::Restore(CRestore&)>,
    6. 0x533e4 <CBaseEntity::ObjectCaps()>,
    ...
    ```
6.  Find the functions by their names in the text editor. Line number will be
    the `"linux"` offset.
7.  As far as I know, Linux vtable offset should match Windows so put the same
    number for `"windows"`.
