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

static Accessible*
find_font_manager(void) {
	gint desktop,
	     n_desktops = SPI_getDesktopCount();
	Accessible* font_manager = NULL;

	for(desktop = 0; desktop < n_desktops && !font_manager; desktop++) {
		Accessible* desk = SPI_getDesktop(desktop);
		gint application, n_applications = Accessible_getChildCount(desk);
		for(application = 0; application < n_applications && !font_manager; application++) {
			Accessible* app = Accessible_getChildAtIndex(desk, application);
			gchar* name = Accessible_getName(app);
			if(name && !strcmp("font-manager", name)) {
				font_manager = app;
			} else {
				Accessible_unref(app);
			}
			SPI_freeString(name);
		}
		Accessible_unref(desk);
	}

	if(!font_manager) {
		g_critical("Could not find the font manager (is it running with accessibility enabled?), aborting...");
	}

	return font_manager;
}

int
main(int argc, char** argv) {
	Accessible* font_manager = NULL;

	SPI_init();

	// start the application:
	// GTK_MODULES="gail:atk-bridge" ./font-manager

	font_manager = find_font_manager();
	// get the x window id via IPC
	// grab a screenshot with xwd
	// convert it to a png with convert (for now)
	// load the image with cairo
	// do some magic foo
	// be happy

	Accessible_unref(font_manager);

	return 0;
}

