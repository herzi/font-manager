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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <glib/gi18n.h>
#include <glade/glade.h>
#include <libgnome/libgnome.h>
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

static void
fm_url_hook(GtkAboutDialog *about, gchar const* link) {
	gnome_url_show(link, NULL);
}

int
main(int argc, char** argv) {
#ifdef ENABLE_NLS
	bindtextdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);
#endif
	gtk_init(&argc, &argv);
	glade_init();
	gnome_program_init(PACKAGE, VERSION,
			   LIBGNOME_MODULE,
			   argc, argv,
			   GNOME_PARAM_APP_DATADIR, PACKAGE_DATA_DIR,
			   GNOME_PARAM_HUMAN_READABLE_NAME, _("Font Manager"),
			   NULL);
	gtk_about_dialog_set_url_hook((GtkAboutDialogActivateLinkFunc)fm_url_hook, NULL, NULL);

	glade_set_custom_handler((GladeXMLCustomWidgetHandler)fm_glade_helper, NULL);
	gtk_widget_show(fm_window_new());

	gtk_main();

	return 0;
}

