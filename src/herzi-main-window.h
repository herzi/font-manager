/* this file is part of gnome-ide, a gnome ide
 *
 * AUTHORS
 *       Sven Herzberg        <herzi@gnome-de.org>
 *
 * Copyright (C) 2005 Sven Herzberg
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of the
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

#ifndef HERZI_MAIN_WINDOW_H
#define HERZI_MAIN_WINDOW_H

#include <gtk/gtkuimanager.h>
#include <gtk/gtkwindow.h>

G_BEGIN_DECLS

typedef struct _HerziMainWindow      HerziMainWindow;
typedef struct _HerziMainWindowClass HerziMainWindowClass;

#define HERZI_TYPE_MAIN_WINDOW         (herzi_main_window_get_type())
#define HERZI_MAIN_WINDOW(i)           (G_TYPE_CHECK_INSTANCE_CAST((i), HERZI_TYPE_MAIN_WINDOW, HerziMainWindow))
#define HERZI_MAIN_WINDOW_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST((c), HERZI_TYPE_MAIN_WINDOW, HerziMainWindowClass))
#define HERZI_IS_MAIN_WINDOW(i)        (G_TYPE_CHECK_INSTANCE_TYPE((i), HERZI_TYPE_MAIN_WINDOW))
#define HERZI_IS_MAIN_WINDOW_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE((c), HERZI_TYPE_MAIN_WINDOW))
#define HERZI_MAIN_WINDOW_GET_CLASS(i) (G_TYPE_INSTANCE_GET_CLASS((i), HERZI_TYPE_MAIN_WINDOW, HerziMainWindowClass))

GType herzi_main_window_get_type(void);

struct _HerziMainWindow {
	GtkWindow     base_instance;
	gboolean      disposed;

	guint         geometry_id;

	GtkWidget   * vbox;
	GtkUIManager* ui_manager;
};

struct _HerziMainWindowClass {
	GtkWindowClass base_class;

	gchar* (*get_geometry) (HerziMainWindow* self);
	void   (*set_geometry) (HerziMainWindow* self,
				gchar const    * geometry);
};

G_END_DECLS

#endif /* !HERZI_MAIN_WINDOW_H */

