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

#ifndef FM_WINDOW_H
#define FM_WINDOW_H

#include <gtk/gtk.h>
#include "herzi-main-window.h"

G_BEGIN_DECLS

typedef struct _FMWindow      FMWindow;
typedef struct _FMWindowClass FMWindowClass;

#define FM_TYPE_WINDOW         (fm_window_get_type())

GType      fm_window_get_type(void);
GtkWidget* fm_window_new     (void);

struct _FMWindow {
	HerziMainWindow base_instance;

	GtkTreeModel  * model;
	GtkStatusbar  * status;
	gint            context_load;
	GtkProgressBar* progress;

	GtkWidget     * preview;
};

struct _FMWindowClass {
	HerziMainWindowClass base_class;
};

G_END_DECLS

#endif /* !FM_WINDOW_H */
