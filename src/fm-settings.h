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

#ifndef FM_SETTINGS_H
#define FM_SETTINGS_H

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _FMSettings      FMSettings;
typedef struct _GObjectClass    FMSettingsClass;

#define FM_TYPE_SETTINGS         (fm_settings_get_type())
#define FM_SETTINGS(i)           (G_TYPE_CHECK_INSTANCE_CAST((i), FM_TYPE_SETTINGS, FMSettings))

GType       fm_settings_get_type(void);
FMSettings* fm_settings_get     (void);
void        fm_settings_set_font(gchar const* key,
				 gchar const* font);

struct _FMSettings {
	GObject base_instance;
};

G_END_DECLS

#endif /* !FM_SETTINGS_H */
