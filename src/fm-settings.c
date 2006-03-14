/* This file is part of font manager
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

#include "fm-settings.h"

#include <gconf/gconf-client.h>

static FMSettings* instance = NULL;

FMSettings*
fm_settings_get(void) {
	return g_object_new(FM_TYPE_SETTINGS, NULL);
}

void
fm_settings_set_font(gchar const* key, gchar const* font) {
	GConfClient* c = gconf_client_get_default();
	GError* e = NULL;
	// FIXME: add error handling with the status bar
	gconf_client_set_string(c, key, font, &e);
	g_object_unref(c);

	if(e) {
		g_warning("%s", e->message);
		g_error_free(e);
		e = NULL;
	}
}

/* GType */
G_DEFINE_TYPE(FMSettings, fm_settings, G_TYPE_OBJECT);

static void
fm_settings_init(FMSettings* self) {}

static GObject*
fs_constructor(GType type, guint n_params, GObjectConstructParam* params) {
	GObject* retval;

	if(!instance) {
		G_OBJECT_CLASS(fm_settings_parent_class)->constructor(type, n_params, params);
		instance = FM_SETTINGS(retval);
	} else {
		retval = G_OBJECT(instance);
		g_object_freeze_notify(retval);
	}

	return retval;
}

static void
fm_settings_class_init(FMSettingsClass* self_class) {
	GObjectClass* go_class = G_OBJECT_CLASS(go_class);
	go_class->constructor = fs_constructor;
}

