/* This file is part of camarilla
 *
 * AUTHORS
 *     Sven Herzberg  <herzi@gnome-de.org>
 *
 * Copyright (C) 2006  Sven Herzberg <herzi@gnome-de.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include <glib/gmessages.h>
#include <cspi/spi.h>

static void
print_accessible(Accessible* acc, gint depth) {
	gint i = 0, children = 0;
	gchar* name = Accessible_getName(acc);
	gchar* role = Accessible_getRoleName(acc);
	for(; i < depth; i++) {
		g_print("\t");
	}
	if(name && *name) {
		g_print("%s (%s)\n", name, role);
	} else {
		g_print("(%s)\n", role);
	}
	SPI_freeString(role);
	SPI_freeString(name);

	children = Accessible_getChildCount(acc);
	for(i = 0; i < children; i++) {
		Accessible* child = Accessible_getChildAtIndex(acc, i);
		print_accessible(child, depth + 1);
		Accessible_unref(child);
	}
}

int
main(int argc, char** argv) {
	gint i, n_desktops;

	SPI_init();
	n_desktops = SPI_getDesktopCount();

	// start the application:
	// GTK_MODULES="gail:atk-bridge" ./font-manager

	for(i = 0; i < n_desktops; i++) {
		Accessible* desktop = SPI_getDesktop(i);
		print_accessible(desktop, 0);
		Accessible_unref(desktop);
	}

	return 0;
}

