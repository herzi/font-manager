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

#include "fm-preview-model.h"

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <glib/gi18n.h>
#include <pango/pango-enum-types.h>
#include <pango/pango-font.h>
#include <gdk/gdkcolor.h>

static GType columns[FM_PREVIEW_MODEL_N_COLUMNS] = {0};

/* iter format:
 * iter->user_data  = GList*<FcPattern*>
 * iter->user_data2 = gboolean is_preview
 */

GtkTreeModel*
fm_preview_model_new(void) {
	return g_object_new(FM_TYPE_PREVIEW_MODEL, NULL);
}

void
fm_preview_model_clear(FMPreviewModel* self) {
	GList* item = g_list_last(self->fonts);

	for(; item; ) {
		gint index = g_list_position(self->fonts, item);
		GtkTreePath* path = gtk_tree_path_new();
		gtk_tree_path_append_index(path, 2*index + 1);

		gtk_tree_model_row_deleted(GTK_TREE_MODEL(self), path);
		g_assert(gtk_tree_path_up(path));

		gtk_tree_path_append_index(path, 2*index);
		gtk_tree_model_row_deleted(GTK_TREE_MODEL(self), path);

		gtk_tree_path_free(path);

		if(item->prev) {
			item = item->prev;
			self->fonts = g_list_delete_link(self->fonts, item->next);
		} else {
			self->fonts = g_list_delete_link(self->fonts, item);
			item = NULL;
		}
	}
}

static void
fpm_invalidate_previews(FMPreviewModel* self) {
	gint i, l;

	l = g_list_length(self->fonts);
	for(i = 0; i < l; i++) {
		GtkTreeIter iter;
		GtkTreePath* path = gtk_tree_path_new();
		iter.user_data  = g_list_nth(self->fonts, i);
		iter.user_data2 = (gpointer)1;
		gtk_tree_path_append_index(path, 2*i+1);
		gtk_tree_model_row_changed(GTK_TREE_MODEL(self), path, &iter);
	}
}

void
fm_preview_model_set_text(FMPreviewModel* self, gchar const* text) {
	g_return_if_fail(FM_IS_PREVIEW_MODEL(self));
	g_return_if_fail(text);

	if(!strcmp(text, self->text)) {
		return;
	}

	g_free(self->text);
	self->text = g_strdup(text);

	fpm_invalidate_previews(self);
}

void
fm_preview_model_set_size(FMPreviewModel* self, gdouble size) {
	g_return_if_fail(FM_IS_PREVIEW_MODEL(self));

	if(self->size == size) {
		return;
	}

	self->size = size;

	fpm_invalidate_previews(self);
}

void
fm_preview_model_add(FMPreviewModel* self, FcPattern* pattern) {
	GtkTreePath* path = gtk_tree_path_new();
	GtkTreeIter iter;
	gint index = 2*g_list_length(self->fonts);

	gtk_tree_path_append_index(path, index);
	self->fonts = g_list_append(self->fonts, pattern);
	iter.user_data  = g_list_find(self->fonts, pattern);
	iter.user_data2 = NULL;
	gtk_tree_model_row_inserted(GTK_TREE_MODEL(self), path, &iter);
	g_assert(gtk_tree_path_up(path));
	gtk_tree_path_append_index(path, index+1);
	iter.user_data2 = (gpointer)1;
	gtk_tree_model_row_inserted(GTK_TREE_MODEL(self), path, &iter);
	gtk_tree_path_free(path);
}

/* GType */
static void fpm_init_tree_model(GtkTreeModelIface* iface);
G_DEFINE_TYPE_WITH_CODE(FMPreviewModel, fm_preview_model, G_TYPE_OBJECT,
			G_IMPLEMENT_INTERFACE(GTK_TYPE_TREE_MODEL, fpm_init_tree_model));

static void
fm_preview_model_init(FMPreviewModel* self) {
	self->text = g_strdup(_("The quick brown fox jumps over the lazy dog."));
	self->size = 12.0;
}

static void
fm_preview_model_class_init(FMPreviewModelClass* self_class) {
	columns[FM_PREVIEW_MODEL_COL_TEXT] = G_TYPE_STRING;
	columns[FM_PREVIEW_MODEL_COL_FONT] = PANGO_TYPE_FONT_DESCRIPTION;
	columns[FM_PREVIEW_MODEL_COL_BACKGROUND] = G_TYPE_STRING;
	columns[FM_PREVIEW_MODEL_COL_ELLIPSIZE] = PANGO_TYPE_ELLIPSIZE_MODE;
	columns[FM_PREVIEW_MODEL_COL_PATTERN] = G_TYPE_POINTER;
}

/* GtkTreeModelIface implementation */
static GType
fpm_get_column_type(GtkTreeModel* model, gint index) {
	return columns[index];
}

static GtkTreeModelFlags
fpm_get_flags(GtkTreeModel* model) {
	return GTK_TREE_MODEL_LIST_ONLY;
}

static gboolean
fpm_get_iter(GtkTreeModel* model, GtkTreeIter* iter, GtkTreePath* path) {
	FMPreviewModel* self = FM_PREVIEW_MODEL(model);
	gint index;
	g_return_val_if_fail(gtk_tree_path_get_depth(path) == 1, FALSE);

	index = gtk_tree_path_get_indices(path)[0];
	if(0 > index || index >= 2*g_list_length(self->fonts)) {
		return FALSE;
	}

	iter->user_data2 = (gpointer)(index % 2);

	if(iter->user_data2) {
		index--;
	}

	index = index / 2;

	iter->user_data = g_list_nth(self->fonts, index);

	return TRUE;
}

static gint
fpm_get_n_columns(GtkTreeModel* model) {
	return FM_PREVIEW_MODEL_N_COLUMNS;
}

static void
fpm_get_value(GtkTreeModel* model, GtkTreeIter* iter, gint column, GValue* value) {
	FMPreviewModel* self = FM_PREVIEW_MODEL(model);
	g_value_init(value, fpm_get_column_type(model, column));

	switch(column) {
	case FM_PREVIEW_MODEL_COL_TEXT:
		if(iter->user_data2) {
			g_value_set_string(value, self->text);
		} else {
			PangoFontDescription* new = pango_fc_font_description_from_pattern(((GList*)(iter->user_data))->data, FALSE);
			gchar* name = g_strdup_printf("%s", pango_font_description_get_family(new));
			g_value_take_string(value, name);
			pango_font_description_free(new);
		}
		break;
	case FM_PREVIEW_MODEL_COL_FONT:
		if(iter->user_data2) {
			PangoFontDescription* new = pango_fc_font_description_from_pattern(((GList*)(iter->user_data))->data, FALSE);
			pango_font_description_set_size(new, PANGO_SCALE * self->size);
			g_value_take_boxed(value, new);
		} else {
			g_value_set_boxed(value, NULL);
		}
		break;
	case FM_PREVIEW_MODEL_COL_ELLIPSIZE:
		g_value_set_enum(value, iter->user_data2 ? PANGO_ELLIPSIZE_END : PANGO_ELLIPSIZE_NONE);
		break;
	case FM_PREVIEW_MODEL_COL_PATTERN:
		g_value_set_pointer(value, ((GList*)(iter->user_data))->data);
		break;
	case FM_PREVIEW_MODEL_COL_BACKGROUND:
		g_value_set_string(value, iter->user_data2 ? NULL : "gray");
		break;
	}
}

static gboolean
fpm_iter_children(GtkTreeModel* model, GtkTreeIter* iter, GtkTreeIter* parent) {
	return gtk_tree_model_iter_nth_child(model, iter, parent, 0);
}

static gboolean
fpm_iter_next(GtkTreeModel* model, GtkTreeIter* iter) {
	iter->user_data2 = (gpointer)!iter->user_data2;
	if(iter->user_data2) {
		return TRUE;
	}

	iter->user_data = ((GList*)(iter->user_data))->next;

	return iter->user_data != NULL;
}

static gboolean
fpm_iter_nth_child(GtkTreeModel* model, GtkTreeIter* iter, GtkTreeIter* parent, gint n) {
	GtkTreePath* path = NULL;
	gboolean     retval = FALSE;
	if(parent) {
		return FALSE;
	}

	path = gtk_tree_path_new();
	gtk_tree_path_append_index(path, n);

	retval = fpm_get_iter(model, iter, path);
	gtk_tree_path_free(path);

	return retval;
}

static void
fpm_init_tree_model(GtkTreeModelIface* iface) {
	iface->get_column_type = fpm_get_column_type;
	iface->get_flags       = fpm_get_flags;
	iface->get_iter        = fpm_get_iter;
	iface->get_n_columns   = fpm_get_n_columns;
	iface->get_value       = fpm_get_value;
	iface->iter_children   = fpm_iter_children;
	iface->iter_nth_child  = fpm_iter_nth_child;
	iface->iter_next       = fpm_iter_next;
}

