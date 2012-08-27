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
#include "utils.h"


void WpnModCommand(void)
{
	const char *cmd = CMD_ARGV(1);

	if (!strcmp(cmd, "version")) 
	{
		print_srvconsole("%s %s (%s)\n", Plugin_info.name, Plugin_info.version, Plugin_info.url);
		print_srvconsole("Author:\n\tKORD_12.7 (AGHL.RU Dev Team)\n");
		print_srvconsole("Compiled: %s\n", __DATE__ ", " __TIME__);

	}
	else if (!strcmp(cmd, "credits"))
	{
		print_srvconsole("Credits:\n\t6a6kin, GordonFreeman, Lev, noo00oob.\n");
	}
	else if (!strcmp(cmd, "items"))
	{
		int i = 0;
		int items = 0;
		int weapons = 0;
		int ammo = 0;

		print_srvconsole("\nCurrently loaded weapons:\n");

		for (i = LIMITER_WEAPON + 1; i <= g_iWeaponIndex; i++)
		{
			items++;
			print_srvconsole(" [%2d] %-23.22s\n", ++weapons, pszName(i));
		}

		print_srvconsole("\nCurrently loaded ammo:\n");

		for (i = 0; i < g_iAmmoBoxIndex; i++)
		{
			items++;
			print_srvconsole(" [%2d] %-23.22s\n", ++ammo, AmmoBoxInfoArray[i].pszName);
		}

		print_srvconsole("\nTotal:\n");
		print_srvconsole("%4d items (%d weapons, %d ammo).\n\n", items, weapons, ammo);
	}
	else if (!strcmp(cmd, "gpl"))
	{
		print_srvconsole("Half-Life Weapon Mod\n");
		print_srvconsole("\n");
		print_srvconsole(" by the AGHL.RU Dev Team\n");
		print_srvconsole("\n");
		print_srvconsole("\n");
		print_srvconsole("  This program is free software; you can redistribute it and/or modify it\n");
		print_srvconsole("  under the terms of the GNU General Public License as published by the\n");
		print_srvconsole("  Free Software Foundation; either version 2 of the License, or (at\n");
		print_srvconsole("  your option) any later version.\n");
		print_srvconsole("\n");
		print_srvconsole("  This program is distributed in the hope that it will be useful, but\n");
		print_srvconsole("  WITHOUT ANY WARRANTY; without even the implied warranty of\n");
		print_srvconsole("  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU\n");
		print_srvconsole("  General Public License for more details.\n");
		print_srvconsole("\n");
		print_srvconsole("  You should have received a copy of the GNU General Public License\n");
		print_srvconsole("  along with this program; if not, write to the Free Software Foundation,\n");
		print_srvconsole("  Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA\n");
		print_srvconsole("\n");
		print_srvconsole("  In addition, as a special exception, the author gives permission to\n");
		print_srvconsole("  link the code of this program with the Half-Life Game Engine (\"HL\n");
		print_srvconsole("  Engine\") and Modified Game Libraries (\"MODs\") developed by Valve,\n");
		print_srvconsole("  L.L.C (\"Valve\"). You must obey the GNU General Public License in all\n");
		print_srvconsole("  respects for all of the code used other than the HL Engine and MODs\n");
		print_srvconsole("  from Valve. If you modify this file, you may extend this exception\n");
		print_srvconsole("  to your version of the file, but you are not obligated to do so. If\n");
		print_srvconsole("  you do not wish to do so, delete this exception statement from your\n");
		print_srvconsole("  version.\n");
		print_srvconsole("\n");
	}
	else
	{
		// Unknown command
		print_srvconsole("Usage: wpnmod < command > [ argument ]\n");
		print_srvconsole("Commands:\n");
		print_srvconsole("   %-22s - %s\n", "version", "displays version information.");
		print_srvconsole("   %-22s - %s\n", "credits", "displays credits information.");
		print_srvconsole("   %-22s - %s\n", "items", "displays information about registered weapons and ammo.");
		print_srvconsole("   %-22s - %s\n", "gpl", "print the license.");
	}
}
