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

#include "fm-window.h"

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <glib/gi18n.h>
#include <fontconfig/fontconfig.h>
#include <pango/pangofc-fontmap.h>
#include <glade/glade.h>

#include "fm-preview-list.h"

GtkWidget*
fm_window_new(void) {
	return g_object_new(FM_TYPE_WINDOW, NULL);
}

/* GType */
G_DEFINE_TYPE(FMWindow, fm_window, HERZI_TYPE_MAIN_WINDOW);

struct data {
	FMWindow * self;
	FcConfig * c;
	FcFontSet* s;
	gint i;
};

enum {
	COL_NAME,
	COL_STYLE,
	COL_PATTERN,
	N_COLUMNS
};

static gboolean
update_list(struct data* data) {
	gint8* s = NULL;

	if(!FcPatternGetString(data->s->fonts[data->i], "family", 0, &s)) {
		GtkTreeIter iter;
		gtk_list_store_append(data->self->model, &iter);
		gtk_list_store_set(data->self->model, &iter,
				   COL_NAME, s,
				   COL_PATTERN, data->s->fonts[data->i],
				   -1);
		if(!FcPatternGetString(data->s->fonts[data->i], FC_STYLE, 0, &s)) {
			gtk_list_store_set(data->self->model, &iter,
					   COL_STYLE, s,
					   -1);
		}
	}
	data->i++;
	gtk_progress_bar_set_fraction(data->self->progress, 1.0 * data->i / data->s->nfont);

	if(data->i >= data->s->nfont) {
		gtk_widget_hide(data->self->progress);
		gtk_statusbar_pop(data->self->status, data->self->context_load);
		FcConfigDestroy(data->c);
		g_free(data);
		data = NULL;
	}

	return data != NULL;
}

static void
fw_prepare_lazy_init(FMWindow* self) {
	struct data* data = g_new0(struct data, 1);

	data->self = self;
	data->c = FcInitLoadConfigAndFonts();
	data->s = FcConfigGetFonts(data->c, FcSetSystem);
	data->i = 0;

	gtk_statusbar_push(data->self->status, data->self->context_load, _("Loading system fonts..."));
	g_idle_add((GSourceFunc)(update_list), data);
	gtk_widget_show(self->progress);
}

static void
add_selected_font(GtkTreeModel* model, GtkTreePath* path, GtkTreeIter* iter, FMWindow* self) {
	FcPattern* pattern;
	gtk_tree_model_get(model, iter, COL_PATTERN, &pattern, -1);
	fm_preview_list_add(FM_PREVIEW_LIST(self->preview), pattern);
}

static void
font_selection_changed(FMWindow* self, GtkTreeSelection* selection) {
	fm_preview_list_clear(FM_PREVIEW_LIST(self->preview));
	if(gtk_tree_selection_count_selected_rows(selection)) {
		gtk_tree_selection_selected_foreach(selection, (GtkTreeSelectionForeachFunc)add_selected_font, self);
	}
}

static void
fw_update_preview_text(FMWindow* self, GParamSpec* pspec, GtkEntry* entry) {
	fm_preview_list_set_text(self->preview, gtk_entry_get_text(entry));
}

static void
fw_update_preview_size(FMWindow* self, GtkSpinButton* spin) {
	fm_preview_list_set_size(self->preview, gtk_spin_button_get_value(spin));
}

static void
fm_window_init(FMWindow* self) {
	GtkWidget* expander = NULL,
		 * alignment = NULL,
		 * tree = NULL,
		 * scroll = NULL;
	GtkTreeModel* sort = NULL;
	GladeXML* xml = glade_xml_new("../data/font-manager.glade", "main_content", NULL);

	gtk_window_set_title(GTK_WINDOW(self), _("Font Manager"));

	gtk_box_pack_start(GTK_BOX(HERZI_MAIN_WINDOW(self)->vbox), glade_xml_get_widget(xml, "main_content"), TRUE, TRUE, 0);

	/* preview list */
	self->preview = glade_xml_get_widget(xml, "preview_list");

	/* system font list */
	self->model = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
	sort = gtk_tree_model_sort_new_with_model(self->model);
	gtk_tree_sortable_set_sort_column_id(sort, COL_NAME, GTK_SORT_ASCENDING);

	tree = glade_xml_get_widget(xml, "treeview_system_fonts");
	gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(tree)), GTK_SELECTION_MULTIPLE);
	gtk_tree_view_set_model(GTK_TREE_VIEW(tree), sort);
	gtk_tree_view_insert_column_with_attributes(tree, -1,
					            _("Family"), gtk_cell_renderer_text_new(),
					            "text", COL_NAME,
					            NULL);
	gtk_tree_view_insert_column_with_attributes(tree, -1,
					            _("Style"), gtk_cell_renderer_text_new(),
					            "text", COL_STYLE,
					            NULL);
	g_signal_connect_swapped(gtk_tree_view_get_selection(GTK_TREE_VIEW(tree)), "changed",
				 G_CALLBACK(font_selection_changed), self);
	font_selection_changed(self, gtk_tree_view_get_selection(GTK_TREE_VIEW(tree)));

	scroll = gtk_hbox_new(FALSE, 0);
	self->progress = gtk_progress_bar_new();
	gtk_box_pack_start(scroll, self->progress, FALSE, FALSE, 0);
	self->status = gtk_statusbar_new();
	self->context_load = gtk_statusbar_get_context_id(self->status, "Loading");
	gtk_box_pack_start(scroll, self->status, TRUE, TRUE, 0);
	gtk_widget_show_all(scroll);
	gtk_box_pack_start(HERZI_MAIN_WINDOW(self)->vbox, scroll, FALSE, FALSE, 0);
	fw_prepare_lazy_init(self);

	g_signal_connect_swapped(glade_xml_get_widget(xml, "entry_preview_text"), "notify::text",
				 G_CALLBACK(fw_update_preview_text), self);
	g_signal_connect_swapped(glade_xml_get_widget(xml, "spinbutton_preview_size"), "value-changed",
				 G_CALLBACK(fw_update_preview_size), self);
	g_object_unref(xml);
}

static void
fm_window_class_init(FMWindowClass* self_class) {}

