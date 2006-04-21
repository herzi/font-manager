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
#include <gconf/gconf-client.h>
#include <glade/glade.h>
#include <libgnomevfs/gnome-vfs.h>

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
	COL_URI,
	COL_UNINSTALLABLE,
	N_COLUMNS
};

static gboolean
update_list(struct data* data) {
	gint8* s = NULL;
	FcChar8 *file;
	gchar *tmp;
	GnomeVFSURI *uri, *parent;
	GnomeVFSFileInfo info;
	gboolean uninstallable = FALSE;

	if(!FcPatternGetString(data->s->fonts[data->i], "family", 0, (FcChar8**)&s)) {
		GtkTreeIter iter;
		GtkListStore* store = GTK_LIST_STORE(data->self->model);
		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter,
				   COL_NAME, s,
				   COL_PATTERN, data->s->fonts[data->i],
				   -1);
		if(!FcPatternGetString(data->s->fonts[data->i], FC_STYLE, 0, (FcChar8**)&s)) {
			gtk_list_store_set(store, &iter,
					   COL_STYLE, s,
					   -1);
		}
		if(!FcPatternGetString(data->s->fonts[data->i], FC_FILE, 0, &file)) {
			tmp = gnome_vfs_get_uri_from_local_path((gchar *)file);
			uri = gnome_vfs_uri_new(tmp);
			g_free(tmp);

			parent = gnome_vfs_uri_get_parent(uri);
			if(!gnome_vfs_get_file_info_uri(parent, &info, 
						        GNOME_VFS_FILE_INFO_FOLLOW_LINKS | GNOME_VFS_FILE_INFO_GET_ACCESS_RIGHTS)) {
				uninstallable = (info.permissions & GNOME_VFS_PERM_ACCESS_WRITABLE) == GNOME_VFS_PERM_ACCESS_WRITABLE;
			}
			gnome_vfs_uri_unref(parent);

			gtk_list_store_set(store, &iter,
					   COL_URI, uri,
					   COL_UNINSTALLABLE, uninstallable,
					   -1);
		}
	}
	data->i++;
	gtk_progress_bar_set_fraction(data->self->progress, 1.0 * data->i / data->s->nfont);

	if(data->i >= data->s->nfont) {
		gtk_widget_hide(GTK_WIDGET(data->self->progress));
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
	gtk_widget_show(GTK_WIDGET(self->progress));
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
	fm_preview_list_set_text(FM_PREVIEW_LIST(self->preview), gtk_entry_get_text(entry));
}

static void
fw_update_preview_size(FMWindow* self, GtkSpinButton* spin) {
	fm_preview_list_set_size(FM_PREVIEW_LIST(self->preview), gtk_spin_button_get_value(spin));
}

static void
fw_install_fonts(FMWindow* self, GSList *font_uris) {
	gchar *fonts_dir, *name;
	GnomeVFSURI *fonts_uri, *source_uri, *target_uri;
	GnomeVFSResult result;
	GSList *tmp;
	GList *source_list = NULL, *target_list = NULL;

	fonts_dir = g_build_filename(g_get_home_dir(), ".fonts", NULL);
	fonts_uri = gnome_vfs_uri_new (fonts_dir);
	g_free (fonts_dir);
	result = gnome_vfs_make_directory_for_uri (fonts_uri, 0755);

	if(result != GNOME_VFS_OK && result != GNOME_VFS_ERROR_FILE_EXISTS) {
		gnome_vfs_uri_unref (fonts_uri);
		return;
	}

	for(tmp = font_uris; tmp != NULL; tmp = tmp->next) {
		source_uri = gnome_vfs_uri_new((gchar *)tmp->data);
		source_list = g_list_append(source_list, source_uri);

		name = gnome_vfs_uri_extract_short_name(source_uri);
		target_uri = gnome_vfs_uri_append_file_name (fonts_uri, name);
		g_free (name);
		target_list = g_list_append(target_list, target_uri);
	}

	result = gnome_vfs_xfer_uri_list(source_list,
					 target_list,
					 GNOME_VFS_XFER_DEFAULT,
					 GNOME_VFS_XFER_ERROR_MODE_ABORT,
					 GNOME_VFS_XFER_OVERWRITE_MODE_SKIP,
					 NULL, NULL);

	g_list_foreach(source_list, (GFunc)gnome_vfs_uri_unref, NULL);
	g_list_foreach(target_list, (GFunc)gnome_vfs_uri_unref, NULL);
	g_list_free(source_list);
	g_list_free(target_list);

	gnome_vfs_uri_unref(fonts_uri);
}

static void
fw_action_file_install(GtkAction* action, FMWindow* self) {
	GtkWidget *dialog;

	dialog = gtk_file_chooser_dialog_new("Install Fonts",
					     GTK_WINDOW(self),
					     GTK_FILE_CHOOSER_ACTION_OPEN,
					     GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					     "_Install Fonts", GTK_RESPONSE_ACCEPT,
					     NULL);
	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), TRUE);

	if(gtk_dialog_run(GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		GSList *font_uris;

		font_uris = gtk_file_chooser_get_uris(GTK_FILE_CHOOSER (dialog));
		fw_install_fonts(self, font_uris);
		g_slist_foreach(font_uris, (GFunc)g_free, NULL);
		g_slist_free(font_uris);
	}

	gtk_widget_destroy(dialog);
}

static void
fw_action_file_open_folder(GtkAction* action, FMWindow* self) {
	gnome_url_show("fonts:///", NULL);
}

static void
fw_action_help_about(GtkAction* action, FMWindow* self) {
	gchar const* authors[] = {
		"Sven Herzberg",
		NULL
	};
	gchar const* artists[] = {
		"Jörg Kress",
		NULL
	};
	gchar const* documenters[] = {
		"Sven Herzberg",
		NULL
	};
	GdkPixbuf* buf = gtk_icon_theme_load_icon(gtk_icon_theme_get_default(),
						  "font-manager", 64, 0, NULL);

	GtkWidget* dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_name   (GTK_ABOUT_DIALOG(dialog), _("Font Manager"));
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), _("0.0.1")); // FIXME: get this auto-generated and translatable (for arab numbers)
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), _("Copyright (C) 2006 Sven Herzberg"));
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), _("A font management tool for the GNOME desktop environment"));
	// FIXME: gtk_about_dialog_set_license
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), "http://font-manager.blaubeermuffin.de/");
	// FIXME: gtk_about_dialog_set_website_label
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), authors);
	gtk_about_dialog_set_artists(GTK_ABOUT_DIALOG(dialog), artists);
	gtk_about_dialog_set_documenters(GTK_ABOUT_DIALOG(dialog), documenters);
	gtk_about_dialog_set_translator_credits(GTK_ABOUT_DIALOG(dialog), _("translator-credits"));
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), buf);
	// FIXME: gtk_about_dialog_set_url_hook

	if(buf) {
		g_object_unref(buf);
	}

	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

static void
fw_action_help_index(GtkAction* action, FMWindow* self) {
	GError* error = NULL;
	gnome_help_display("font-manager.xml", NULL, &error);

	if(error) {
		// FIXME: add error dialog
		g_message("%s", error->message);
		g_error_free(error);
		error = NULL;
	}
}

static GtkActionEntry entries[] = {
	{"FileInstall", NULL,			N_("_Install Font..."),
	 NULL,		NULL,
	 G_CALLBACK(fw_action_file_install)},
	{"FileOpenFolder", GTK_STOCK_OPEN,	N_("_Open Font Folder..."),
	 NULL,		NULL, // FIXME: add action hint
	 G_CALLBACK(fw_action_file_open_folder)},
	{"Help",	NULL,			N_("_Help")},
	{"HelpAbout",	GTK_STOCK_ABOUT,	NULL,
	 NULL,		NULL, // FIXME: check for action hint
	 G_CALLBACK(fw_action_help_about)},
	{"HelpIndex",	GTK_STOCK_HELP,		N_("_User Guide"),
	 "F1",		NULL, // FIXME: check for action hint
	 G_CALLBACK(fw_action_help_index)}
};

static gchar const * const ui =
"<ui>"
"	<menubar name='menubar'>"
"		<menu action='File'>"
"			<placeholder name='FileOpen'>"
"				<menuitem action='FileInstall'/>"
"				<menuitem action='FileOpenFolder'/>"
"			</placeholder>"
"		</menu>"
"		<menu action='Help'>"
"			<menuitem action='HelpIndex'/>"
"			<separator />"
"			<menuitem action='HelpAbout'/>"
"		</menu>"
"	</menubar>"
"</ui>";

static void
fw_init_menus_and_toolbars(FMWindow* self) {
	GtkUIManager* ui_manager = HERZI_MAIN_WINDOW(self)->ui_manager;
	GtkActionGroup* actions = gtk_action_group_new("font-manager");
	GError* error = NULL;
	gtk_action_group_add_actions(actions, entries, G_N_ELEMENTS(entries), self);
	gtk_ui_manager_insert_action_group(ui_manager, actions, 0);
	gtk_ui_manager_add_ui_from_string(ui_manager, ui, -1, &error);
	g_object_unref(actions);

	if(error) {
		g_critical("%s", error->message);
		g_error_free(error);
		error = NULL;
	}

	gtk_action_set_visible(gtk_ui_manager_get_action(ui_manager, "/menubar/File/FileClose"), FALSE);
}

static void
fm_window_init(FMWindow* self) {
	GtkWidget* expander = NULL,
		 * alignment = NULL,
		 * tree = NULL,
		 * scroll = NULL;
	GtkTreeModel* sort = NULL;
	GladeXML* xml = glade_xml_new(PACKAGE_DATA_DIR "/font-manager/font-manager.glade", "main_content", NULL);
	GtkWindow* window = GTK_WINDOW(self);

	if(!xml) {
		GtkWidget* w = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", _("The user interface could not be loaded"));
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(w), _("Please make sure the font manager was installed correctly."));
		gtk_widget_show(w);
		gtk_dialog_run(GTK_DIALOG(w));
		exit(1);
	}

	gtk_window_set_default_icon_from_file(PACKAGE_DATA_DIR "/pixmaps/font-manager.svg", NULL);
	gtk_window_set_title(window, _("Font Manager"));
	gtk_window_set_icon_name(window, "font-manager");
	gtk_box_pack_start(GTK_BOX(HERZI_MAIN_WINDOW(self)->vbox), glade_xml_get_widget(xml, "main_content"), TRUE, TRUE, 0);

	fw_init_menus_and_toolbars(self);

	/* preview list */
	self->preview = glade_xml_get_widget(xml, "preview_list");

	/* system font list */
	self->model = GTK_TREE_MODEL(gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_POINTER, G_TYPE_BOOLEAN));
	sort = gtk_tree_model_sort_new_with_model(self->model);
	gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(sort), COL_NAME, GTK_SORT_ASCENDING);

	tree = glade_xml_get_widget(xml, "treeview_system_fonts");
	gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(tree)), GTK_SELECTION_MULTIPLE);
	gtk_tree_view_set_model(GTK_TREE_VIEW(tree), sort);
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree), -1,
					            _("Family"), gtk_cell_renderer_text_new(),
					            "text", COL_NAME,
					            NULL);
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree), -1,
					            _("Style"), gtk_cell_renderer_text_new(),
					            "text", COL_STYLE,
					            NULL);
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree), -1,
					            _("uninstallable"), gtk_cell_renderer_toggle_new(),
					            "active", COL_UNINSTALLABLE,
					            NULL);
	g_signal_connect_swapped(gtk_tree_view_get_selection(GTK_TREE_VIEW(tree)), "changed",
				 G_CALLBACK(font_selection_changed), self);
	font_selection_changed(self, gtk_tree_view_get_selection(GTK_TREE_VIEW(tree)));

	scroll = gtk_hbox_new(FALSE, 0);
	self->progress = GTK_PROGRESS_BAR(gtk_progress_bar_new());
	gtk_box_pack_start(GTK_BOX(scroll), GTK_WIDGET(self->progress), FALSE, FALSE, 0);
	self->status = GTK_STATUSBAR(gtk_statusbar_new());
	self->context_load = gtk_statusbar_get_context_id(self->status, "Loading");
	gtk_box_pack_start(GTK_BOX(scroll), GTK_WIDGET(self->status), TRUE, TRUE, 0);
	gtk_widget_show_all(scroll);
	gtk_box_pack_start(GTK_BOX(HERZI_MAIN_WINDOW(self)->vbox), scroll, FALSE, FALSE, 0);
	fw_prepare_lazy_init(self);

	g_signal_connect_swapped(glade_xml_get_widget(xml, "entry_preview_text"), "notify::text",
				 G_CALLBACK(fw_update_preview_text), self);
	g_signal_connect_swapped(glade_xml_get_widget(xml, "spinbutton_preview_size"), "value-changed",
				 G_CALLBACK(fw_update_preview_size), self);
	g_object_unref(xml);
}

static gchar*
fm_get_geometry(HerziMainWindow* window) {
	GConfClient* client = gconf_client_get_default();
	gchar* geometry = gconf_client_get_string(client, PACKAGE_GCONF_PATH "geometry", NULL);
	g_object_unref(client);
	return geometry;
}

static void
fm_set_geometry(HerziMainWindow* window, gchar const* geometry) {
	GConfClient* client = gconf_client_get_default();
	gconf_client_set_string(client, PACKAGE_GCONF_PATH "geometry", geometry, NULL);
	g_object_unref(client);
}

static void
fm_window_class_init(FMWindowClass* self_class) {
	HerziMainWindowClass* hmw_class = HERZI_MAIN_WINDOW_CLASS(self_class);
	hmw_class->get_geometry = fm_get_geometry;
	hmw_class->set_geometry = fm_set_geometry;
}

