/* This file is part of font-manager
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

#include <glade/glade.h>
#include "fm-preview-list.h"
#include "fm-window.h"

static GtkWidget*
fm_glade_helper(GladeXML* xml, gchar* func_name, gchar* name, gchar* string, gchar* string2, gint int1, gint int2) {
	if(!strcmp(name, "preview_list")) {
		GtkWidget* w = fm_preview_list_new();
		gtk_widget_show(w);
		return w;
	} else {
		return NULL;
	}
}

int
main(int argc, char** argv) {
	gtk_init(&argc, &argv);
	glade_init();

	glade_set_custom_handler((GladeXMLCustomWidgetHandler)fm_glade_helper, NULL);
	gtk_widget_show(fm_window_new());

	gtk_main();

	return 0;
}
