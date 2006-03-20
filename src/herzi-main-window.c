/* this file is part of memoranda, a note taking application
 *
 * AUTHORS
 *       Sven Herzberg        <herzi@gnome-de.org>
 *
 * Copyright (C) 2005,2006 Sven Herzberg
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

#include "herzi-main-window.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <glib/gi18n.h>
#include <gtk/gtk.h>
/* for XParseGeometry */
#include <X11/Xlib.h>

#include "gtk-helpers.h"

static GList* windows = NULL;

G_DEFINE_TYPE(HerziMainWindow, herzi_main_window, GTK_TYPE_WINDOW);

static void
hmw_close(GtkAction* action, HerziMainWindow* self) {
	// FIXME: emit delete event
	gtk_object_destroy(GTK_OBJECT(self));
}

static void
hmw_quit(GtkAction* action, HerziMainWindow* self) {
	// FIXME: close all the windows
	gtk_main_quit();
}

static void
hmw_abount() {}

static GtkActionEntry hmw_actions[] = {
	{"File",	NULL,			N_("_File")},
	{"FileClose",	GTK_STOCK_CLOSE,	NULL,
	 NULL,		NULL, // FIXME: Check whether we have a detailed description set in the defaults
	 G_CALLBACK(hmw_close)},
	{"FileQuit",	GTK_STOCK_QUIT,		NULL,
	 NULL,		NULL, // FIXME: Check whether we have a detailed description set in the defaults
	 G_CALLBACK(hmw_quit)}
};

static gchar const * const ui =
"<ui><menubar name='menubar'><menu action='File'>"
	"<placeholder name='FileOpen'/>"
	"<separator/>"
	"<placeholder name='FileSave'/>"
	"<separator/>"
	"<placeholder name='FilePrint'/>"
	"<separator/>"
	"<placeholder name='FileProperties'/>"
	"<separator/>"
	"<menuitem action='FileClose'/>"
	"<menuitem action='FileQuit'/>"
"</menu></menubar></ui>";

/* GType stuff */
static void
hmw_menu_error_dialog(GtkWindow* window, GError* error) {
        GtkWidget *error_dialog = gtk_message_dialog_new(window,
							 (GtkDialogFlags)GTK_DIALOG_DESTROY_WITH_PARENT,
							 GTK_MESSAGE_ERROR,
							 GTK_BUTTONS_CLOSE,
							 _("Building the menu bar failed"), NULL);

        gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(error_dialog),
						 "%s\n\n<i>%s</i>",
						 _("This is an error in your application, see below for more details:"),
						 error->message);

        gtk_dialog_run(GTK_DIALOG(error_dialog));
        gtk_widget_destroy(error_dialog);
}

static void
herzi_main_window_init(HerziMainWindow* self) {
	GtkActionGroup* actions;
	GtkWindow     * window = GTK_WINDOW(self);
	GError   * error = NULL;

	self->vbox  = gtk_vbox_new(FALSE, 0);

	// add ourselves to the window list
	windows = g_list_prepend(windows, self);

	self->ui_manager = gtk_ui_manager_new();
	gtk_window_add_accel_group(window,
				   gtk_ui_manager_get_accel_group(self->ui_manager));
	actions = gtk_action_group_new("MainWindowActions");
	gtk_action_group_add_actions(actions, hmw_actions, G_N_ELEMENTS(hmw_actions), self);
	gtk_ui_manager_insert_action_group(self->ui_manager, actions, 0);
	g_object_unref(actions);
	gtk_ui_manager_add_ui_from_string(self->ui_manager, ui, -1, &error);
	if(error) {
		hmw_menu_error_dialog(window, error);
		g_error_free(error);
		error = NULL;
	}
	gtk_box_pack_start(GTK_BOX(self->vbox), gtk_ui_manager_get_widget(self->ui_manager, "/menubar"), FALSE, FALSE, 0);

	gtk_widget_show_all(self->vbox);
	gtk_container_add(GTK_CONTAINER(self), self->vbox);
}

static void
hmw_dispose(GObject* object) {
	HerziMainWindow* self = HERZI_MAIN_WINDOW(object);

	if(self->disposed) {
		return;
	}
	self->disposed = TRUE;

	g_object_unref(self->ui_manager);
	self->ui_manager = NULL;

	G_OBJECT_CLASS(herzi_main_window_parent_class)->dispose(object);
}

static void
hmw_finalize(GObject* object) {
	windows = g_list_remove(windows, object);

	G_OBJECT_CLASS(herzi_main_window_parent_class)->finalize(object);

	if(!windows) {
		gtk_main_quit();
	}
}

static gboolean
hmw_save_geometry(HerziMainWindow* self) {
	if(HERZI_MAIN_WINDOW_GET_CLASS(self)->set_geometry) {
		gchar* geometry = gtk_window_get_geometry(GTK_WINDOW(self));
		HERZI_MAIN_WINDOW_GET_CLASS(self)->set_geometry(self, geometry);
		g_free(geometry);
	}
	self->geometry_id = 0;
	return FALSE;
}

static gboolean
hmw_configure_event(GtkWidget* widget, GdkEventConfigure* event) {
	HerziMainWindow* self = HERZI_MAIN_WINDOW(widget);

	if(self->geometry_id) {
		g_source_remove(self->geometry_id);
		self->geometry_id = 0;
	}

	if(GTK_WIDGET_VISIBLE(widget) && (gdk_window_get_state(widget->window) & GDK_WINDOW_STATE_MAXIMIZED) == 0) {
		self->geometry_id = g_timeout_add(1000, (GSourceFunc)hmw_save_geometry, self);
	}

	return GTK_WIDGET_CLASS(herzi_main_window_parent_class)->configure_event(widget, event);
}

static void
hmw_show(GtkWidget* widget) {
	HerziMainWindow* self = HERZI_MAIN_WINDOW(widget);
	gchar* geometry = NULL;
	gint   x = 0,
	       y = 0;
	guint  w = 400,
	       h = 300;

	if(HERZI_MAIN_WINDOW_GET_CLASS(self)->get_geometry) {
		geometry = HERZI_MAIN_WINDOW_GET_CLASS(self)->get_geometry(self);
	}

	if(geometry) {
		XParseGeometry(geometry, &x, &y, &w, &h);
	}

	gtk_window_set_default_size(GTK_WINDOW(self), w, h);
	GTK_WIDGET_CLASS(herzi_main_window_parent_class)->show(widget);

	if(geometry) {
		gtk_window_move(GTK_WINDOW(self), x, y);
	}

	g_free(geometry);
}

static void
herzi_main_window_class_init(HerziMainWindowClass* self_class) {
	GObjectClass  * go_class;
	GtkWidgetClass* gw_class;

	/* GObjectClass */
	go_class = G_OBJECT_CLASS(self_class);
	go_class->dispose  = hmw_dispose;
	go_class->finalize = hmw_finalize;

	/* GtkWidgetClass */
	gw_class = GTK_WIDGET_CLASS(self_class);
	gw_class->configure_event = hmw_configure_event;
	gw_class->show            = hmw_show;
}

