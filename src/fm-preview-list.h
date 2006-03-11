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

#ifndef FM_PREVIEW_LIST_H
#define FM_PREVIEW_LIST_H

#include <fontconfig/fontconfig.h>
#include <gtk/gtktreeview.h>

typedef struct _FMPreviewList      FMPreviewList;
typedef struct _FMPreviewListClass FMPreviewListClass;

#define FM_TYPE_PREVIEW_LIST         (fm_preview_list_get_type())
#define FM_PREVIEW_LIST(i)           (G_TYPE_CHECK_INSTANCE_CAST((i), FM_TYPE_PREVIEW_LIST, FMPreviewList))
#define FM_IS_PREVIEW_LIST(i)        (G_TYPE_CHECK_INSTANCE_TYPE((i), FM_TYPE_PREVIEW_LIST))

GType      fm_preview_list_get_type(void);
GtkWidget* fm_preview_list_new     (void);
void       fm_preview_list_add     (FMPreviewList* self,
				    FcPattern* pattern);
void       fm_preview_list_set_text(FMPreviewList* self,
				    gchar const* text);
void       fm_preview_list_set_size(FMPreviewList* self,
				    gdouble size);

struct _FMPreviewList {
	GtkTreeView base_instance;

	GtkTreeModel* model;
};

struct _FMPreviewListClass {
	GtkTreeViewClass base_class;
};

#endif /* !FM_PREVIEW_LIST_H */
