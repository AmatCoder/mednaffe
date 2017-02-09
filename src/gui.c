/*
 * gui.c
 *
 * Copyright 2013-2015 AmatCoder
 *
 * This file is part of Mednaffe.
 *
 * Mednaffe is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mednaffe is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mednaffe; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "common.h"

void display_tooltips(GtkBuilder *builder, gboolean state)
{
  gchar *text;
  GSList *list = NULL;
  GSList *iterator = NULL;
  
  list = gtk_builder_get_objects(builder);

  for (iterator = list; iterator; iterator = iterator->next)
  {
    if (GTK_IS_WIDGET(iterator->data))
    {
      text = gtk_widget_get_tooltip_text(iterator->data);
      if (text!=NULL)
        gtk_widget_set_has_tooltip(
            iterator->data, state);
          
      g_free(text);
    }
  }
  
  g_slist_free(list);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_showtooltips_toggled(GtkToggleButton *togglebutton,
                             guidata *gui)
{
  gboolean state;

  state = gtk_toggle_button_get_active(togglebutton);
  
  display_tooltips(gui->builder, state);
  display_tooltips(gui->specific, state);
  display_tooltips(gui->settings, state);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_rbnothing_activate(GtkButton *button, guidata *gui)
{
  gui->state=0;
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_rbminimize_activate(GtkButton *button, guidata *gui)
{
  gui->state=1;
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_rbhide_activate(GtkButton *button, guidata *gui)
{
  gui->state=2;
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
gboolean close_prefs(GtkWidget *widget, GdkEvent *event, guidata *gui)
{
  gtk_widget_hide_on_delete(gui->prefwindow);
  return TRUE;
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void cancel_prefs(GtkButton *button, guidata *gui)
{
  gtk_widget_hide(gui->prefwindow);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_preferences_activate(GtkMenuItem *menuitem, guidata *gui)
{
  gtk_widget_show(gui->prefwindow);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
gboolean close_folder_settings(GtkWidget *widget, GdkEvent *event, guidata *gui)
{
  gtk_widget_hide_on_delete(gui->folderwindow);
  return TRUE;
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
gboolean close_folder_list(GtkWidget *widget, GdkEvent *event, guidata *gui)
{
  gtk_widget_hide_on_delete(gui->folderlistwindow);
  return TRUE;
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_folders_list_setup(GtkMenuItem *menuitem, guidata *gui)
{
  GtkTreeSelection *selection;
  GtkTreeIter iter;

  gtk_widget_show(gui->folderlistwindow);

  gtk_combo_box_get_active_iter(GTK_COMBO_BOX(gui->cbpath), &iter);
  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(gtk_builder_get_object(gui->settings, "treeview1")));
  gtk_tree_selection_select_iter(selection, &iter);

  gtk_widget_grab_focus(GTK_WIDGET(gtk_builder_get_object(gui->settings, "treeview1")));
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void apply_folder_settings(GtkButton *button, guidata *gui)
{
  GtkTreeIter iter;
  GtkTreeModel *combomodel;
  gboolean recursive;
  gboolean hide_ext;
  const gchar *filter;
  const gchar *screen_a;
  const gchar *screen_b;

  combomodel = gtk_combo_box_get_model(GTK_COMBO_BOX(gui->cbpath));

  recursive = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(gtk_builder_get_object(gui->settings, "scan")));
  hide_ext = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(gtk_builder_get_object(gui->settings, "hide_ext")));
  filter = gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(gui->settings, "filters")));
  screen_a = gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(gui->settings, "screen_a")));
  screen_b = gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(gui->settings, "screen_b")));

  if (!gui->resetup_folder)
  {
    gtk_list_store_insert(GTK_LIST_STORE(combomodel), &iter, 0);
    gtk_list_store_set(GTK_LIST_STORE(combomodel), &iter, 0, gui->rompath, -1);
  }
  else
  {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(gtk_builder_get_object(gui->settings, "treeview1")));

    if (!gtk_tree_selection_get_selected(selection, &combomodel, &iter))
      return;
  }

  gtk_list_store_set(GTK_LIST_STORE(combomodel), &iter, 1, recursive,
                                                        2, hide_ext,
                                                        3, g_strdup (filter),
                                                        4, g_strdup (screen_a),
                                                        5, g_strdup (screen_b),
                                                        -1);

  gtk_combo_box_set_active_iter(GTK_COMBO_BOX(gui->cbpath), NULL);
  gtk_combo_box_set_active_iter(GTK_COMBO_BOX(gui->cbpath), &iter);

  gtk_widget_hide(gui->folderwindow);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void cancel_folder_settings(GtkButton *button, guidata *gui)
{
  gtk_widget_hide(gui->folderwindow);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void cancel_folder_list(GtkButton *button, guidata *gui)
{
  gtk_widget_hide(gui->folderlistwindow);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_button_snaps_folder(GtkButton *button, GtkEntry *entry)
{
  GtkWidget *folder;

#ifdef GTK2_ENABLED
  folder = gtk_file_chooser_dialog_new(
    "Choose a folder...", NULL,
    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, GTK_STOCK_CANCEL,
    GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
    NULL);
#else
  folder = gtk_file_chooser_dialog_new(
    "Choose a folder...", NULL,
    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, ("_Cancel"),
    GTK_RESPONSE_CANCEL, ("_Open"), GTK_RESPONSE_ACCEPT,
    NULL);
#endif

  if (gtk_dialog_run(GTK_DIALOG(folder)) == GTK_RESPONSE_ACCEPT)
  {
    gchar *path;

    path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (folder));
    gtk_entry_set_text(entry, path);
    g_free(path);
  }
  gtk_widget_destroy(folder);
}
