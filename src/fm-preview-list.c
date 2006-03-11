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

#include "fm-preview-list.h"

#include <gtk/gtkcellrenderertext.h>
#include <gtk/gtktreeselection.h>
#include <glade/glade.h>
#include "fm-preview-model.h"

GtkWidget*
fm_preview_list_new(void) {
	return g_object_new(FM_TYPE_PREVIEW_LIST, "headers-visible", FALSE, NULL);
}

void
fm_preview_list_add(FMPreviewList* self, FcPattern* pattern) {
	fm_preview_model_add(FM_PREVIEW_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(self))), pattern);
}

void
fm_preview_list_set_text(FMPreviewList* self, gchar const* text) {
	fm_preview_model_set_text(FM_PREVIEW_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(self))), text);
}

void
fm_preview_list_set_size(FMPreviewList* self, gdouble size) {
	fm_preview_model_set_size(FM_PREVIEW_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(self))), size);
}

void
fm_preview_list_clear(FMPreviewList* self) {
	fm_preview_model_clear(FM_PREVIEW_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(self))));
}

/* GType */
G_DEFINE_TYPE(FMPreviewList, fm_preview_list, GTK_TYPE_TREE_VIEW);

static gboolean
fw_tree_selection_func(GtkTreeSelection* selection, GtkTreeModel* model, GtkTreePath* path, gboolean was_selected) {
	return gtk_tree_path_get_indices(path)[gtk_tree_path_get_depth(path) - 1] % 2;
}

static void
fm_preview_list_init(FMPreviewList* self) {
	GtkTreeView* tree = GTK_TREE_VIEW(self);
	gtk_tree_view_set_model(tree, self->model);
	gtk_tree_view_insert_column_with_attributes(tree, -1,
						    "Preview", gtk_cell_renderer_text_new(),
						    "background", FM_PREVIEW_MODEL_COL_BACKGROUND,
						    "ellipsize", FM_PREVIEW_MODEL_COL_ELLIPSIZE,
						    "font-desc", FM_PREVIEW_MODEL_COL_FONT,
						    "text", FM_PREVIEW_MODEL_COL_TEXT,
						    NULL);
	gtk_tree_selection_set_mode(gtk_tree_view_get_selection(tree), GTK_SELECTION_MULTIPLE);
	gtk_tree_selection_set_select_function(gtk_tree_view_get_selection(tree),
					       (GtkTreeSelectionFunc)fw_tree_selection_func,
					       self,
					       NULL);

	gtk_tree_view_set_model(GTK_TREE_VIEW(self), fm_preview_model_new());
}

static void
fpl_display_font(GtkTreeModel* model, GtkTreePath* path, GtkTreeIter* iter, gpointer data) {
	GtkWidget* widget = data;
	GError* error = NULL;
	FcPattern* pattern = NULL;
	gchar* argv[] = {
		"gnome-font-viewer",
		NULL,
		NULL
	};
	gtk_tree_model_get(model, iter, FM_PREVIEW_MODEL_COL_PATTERN, &pattern, -1);
	FcPatternGetString(pattern, FC_FILE, 0, &argv[1]);

	gdk_spawn_on_screen(gtk_widget_get_screen(widget),
			    NULL,
			    argv,
			    NULL,
			    G_SPAWN_SEARCH_PATH,
			    NULL,
			    NULL,
			    NULL,
			    &error);

	if(error) {
		g_warning("%s", error->message);
		g_error_free(error);
		error = NULL;
	}
}

static void
fpl_display_fonts(FMPreviewList* self) {
	gtk_tree_selection_selected_foreach(gtk_tree_view_get_selection(GTK_TREE_VIEW(self)),
					    fpl_display_font,
					    self);
}

static gboolean
fpl_button_press_event(GtkWidget* widget, GdkEventButton* ev) {
	gboolean retval = FALSE;
	if(ev->button == 3) {
		FMPreviewList* self = FM_PREVIEW_LIST(widget);
		GladeXML* xml = glade_xml_new("../data/font-manager.glade", "popup_preview", NULL);
		if(!gtk_tree_selection_count_selected_rows(gtk_tree_view_get_selection(GTK_TREE_VIEW(self)))) {
			GTK_WIDGET_CLASS(fm_preview_list_parent_class)->button_press_event(widget, ev);
		}

		gtk_menu_popup(glade_xml_get_widget(xml, "popup_preview"), NULL, NULL, NULL, NULL, ev->button, ev->time);
		g_signal_connect_swapped(glade_xml_get_widget(xml, "popup_display_font"), "activate",
					 G_CALLBACK(fpl_display_fonts), self);

		g_object_unref(xml);
	} else {
		retval |= GTK_WIDGET_CLASS(fm_preview_list_parent_class)->button_press_event(widget, ev);
	}

	return retval;
}

static void
fm_preview_list_class_init(FMPreviewListClass* self_class) {
	GtkWidgetClass* gw_class = GTK_WIDGET_CLASS(self_class);
	gw_class->button_press_event = fpl_button_press_event;
}

