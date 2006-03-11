/* this file is part of criawips, a gnome presentation application
 *
 * AUTHORS
 *       Sven Herzberg        <herzi@gnome-de.org>
 *
 * Copyright (C) 2005,2006 Sven Herzberg
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#ifndef GTK_HELPERS_H
#define GTK_HELPERS_H

#include <gtk/gtktreeview.h>
#include <gtk/gtkwindow.h>

#include "glib-helpers.h"

G_BEGIN_DECLS

#define GTK_CLIPBOARD_TEXT_RECEIVED_FUNC(f) (GtkClipboardTextReceivedFunc)(f)

extern GtkTreeCellDataFunc hfc_gtk_tree_cell_data_func_int;

void gtk_widget_get_extends(GtkWidget* widget, gint* x, gint* y, gint* w, gint* h);
void gtk_widget_hide_now(GtkWidget* self);
void gtk_main_flush     (void);

void   gtk_widget_set_cursor  (GtkWidget* self, GdkCursorType type);
gchar* gtk_window_get_geometry(GtkWindow* self);

G_END_DECLS

#endif /* GTK_HELPERS_H */

