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

#include "gtk-helpers.h"

#include <gtk/gtkmain.h>

void
gtk_main_flush(void) {
	while(gtk_events_pending()) {
		gtk_main_iteration();
	}
}

static void
_hfc_gtk_tree_cell_data_func_int(GtkTreeViewColumn* column, GtkCellRenderer* renderer, GtkTreeModel* model, GtkTreeIter* iter, gpointer data) {
	guint col = GPOINTER_TO_INT(data);
	guint value;
	gchar* text;

	gtk_tree_model_get(model, iter, col, &value, -1);
	text = g_strdup_printf("%u", value);
	g_object_set(renderer, "text", text, NULL);
	g_free(text);
}
GtkTreeCellDataFunc hfc_gtk_tree_cell_data_func_int = _hfc_gtk_tree_cell_data_func_int;

void
gtk_widget_get_extends(GtkWidget* widget, gint* x, gint* y, gint* w, gint* h) {
	GdkWindow* window = widget->window;
	gdk_window_get_origin(window, x, y);

	if(x) {
		*x += widget->allocation.x;
	}

	if(y) {
		*y += widget->allocation.y;
	}

	if(w) {
		*w =  widget->allocation.width;
	}

	if(h) {
		*h =  widget->allocation.height;
	}
}

void
gtk_widget_hide_now(GtkWidget* self) {
	g_return_if_fail(GTK_IS_WIDGET(self));

	gtk_widget_hide(self);
	gtk_main_flush();
}

void
gtk_widget_set_cursor(GtkWidget* self, GdkCursorType type) {
	GdkCursor * cursor  = NULL;
	GdkDisplay* display = NULL;

	g_return_if_fail(GTK_IS_WIDGET(self));

	display = g_object_ref(gtk_widget_get_display(self));
	cursor = gdk_cursor_new_for_display(display, type);
	gdk_window_set_cursor(self->window, cursor);
	gdk_display_flush(display);
	gdk_cursor_unref(cursor);
	g_object_unref(display);
}

gchar*
gtk_window_get_geometry(GtkWindow* self) {
	int x,y,w,h;

	g_return_val_if_fail(GTK_IS_WINDOW(self), NULL);
	g_return_val_if_fail(gtk_window_get_gravity(self) == GDK_GRAVITY_NORTH_WEST, NULL);

	gtk_window_get_position(self, &x, &y);
	gtk_window_get_size(self, &w, &h);

	return g_strdup_printf ("%dx%d+%d+%d", w, h, x, y);
}

