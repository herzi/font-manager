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

#ifndef FM_PREVIEW_MODEL_H
#define FM_PREVIEW_MODEL_H

#include <fontconfig/fontconfig.h>
#include <pango/pango.h>
#include <gtk/gtktreemodel.h>

G_BEGIN_DECLS

typedef struct _FMPreviewModel      FMPreviewModel;
typedef struct _FMPreviewModelClass FMPreviewModelClass;

#define FM_TYPE_PREVIEW_MODEL         (fm_preview_model_get_type())
#define FM_PREVIEW_MODEL(i)           (G_TYPE_CHECK_INSTANCE_CAST((i), FM_TYPE_PREVIEW_MODEL, FMPreviewModel))
#define FM_IS_PREVIEW_MODEL(i)        (G_TYPE_CHECK_INSTANCE_TYPE((i), FM_TYPE_PREVIEW_MODEL))

enum {
	FM_PREVIEW_MODEL_COL_TEXT,
	FM_PREVIEW_MODEL_COL_FONT,
	FM_PREVIEW_MODEL_COL_BACKGROUND,
	FM_PREVIEW_MODEL_COL_ELLIPSIZE,
	FM_PREVIEW_MODEL_COL_PATTERN,
	FM_PREVIEW_MODEL_N_COLUMNS
};

GType         fm_preview_model_get_type(void);
GtkTreeModel* fm_preview_model_new     (void);
void          fm_preview_model_add     (FMPreviewModel* self,
					FcPattern* desc);
void          fm_preview_model_clear   (FMPreviewModel* self);
void          fm_preview_model_set_text(FMPreviewModel* self,
					gchar const   * text);
void          fm_preview_model_set_size(FMPreviewModel* self,
					gdouble         size);

struct _FMPreviewModel {
	GObject   base_instance;

	gdouble   size;
	gchar   * text;

	GList   * fonts;
};

struct _FMPreviewModelClass {
	GObjectClass base_class;
};

G_END_DECLS

#endif /* !FM_PREVIEW_MODEL_H */
