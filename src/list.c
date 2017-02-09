/*
 * list.c
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
#include <string.h>

#ifdef G_OS_WIN32
  #include <windows.h>
#endif

void change_list (guidata *gui)
{
  GtkAdjustment *adjustament;
  gchar *buff, *total;

  buff=g_strdup_printf("%i",
    gtk_tree_model_iter_n_children(GTK_TREE_MODEL(gui->store), NULL));

  total = g_strconcat(" Games in list: ", buff, NULL);
  gtk_statusbar_pop(GTK_STATUSBAR(gui->sbnumber), 1);
  gtk_statusbar_pop(GTK_STATUSBAR(gui->sbname), 1);
  gtk_statusbar_push(GTK_STATUSBAR(gui->sbnumber), 1, total);
  gtk_statusbar_push(GTK_STATUSBAR(gui->sbname), 1, " Game selected: None");
  g_free(gui->rom);
  g_free(gui->fullpath);
  gui->rom = NULL;
  gui->fullpath = NULL;
  g_free(total);
  g_free(buff);

  adjustament =
    gtk_scrolled_window_get_vadjustment(
                                GTK_SCROLLED_WINDOW(gui->scrollwindow));

  gtk_adjustment_set_value (adjustament, 0);
  gtk_widget_grab_focus(gui->gamelist);

}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void remove_folder(GtkWidget *sender, guidata *gui)
{
  GtkTreeIter iter, iter2, iter3;
  GtkTreeModel *treemodel;
  GtkTreeSelection *selection;

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(gtk_builder_get_object(gui->settings, "treeview1")));
  if (gtk_tree_selection_get_selected(selection, &treemodel, &iter))
  {
    if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(gui->cbpath), &iter2))
    {
      gchar *str_a;
      gchar *str_b;
  
      gtk_tree_model_get (treemodel, &iter, 0, &str_a, -1);
      gtk_tree_model_get (treemodel, &iter2, 0, &str_b, -1);
    
      if (g_strcmp0(str_a, str_b) == 0)
      {
        gtk_list_store_clear(gui->store);
        gtk_statusbar_pop(GTK_STATUSBAR(gui->sbnumber), 1);
        gtk_statusbar_push(GTK_STATUSBAR(gui->sbnumber), 1, " Games in list: 0 ");
        gtk_statusbar_pop(GTK_STATUSBAR(gui->sbname), 1);
        gtk_statusbar_push(GTK_STATUSBAR(gui->sbname), 1, " Game selected: None");

        g_free(gui->fullpath);
        g_free(gui->rompath);
        g_free(gui->rom);
        gui->fullpath=NULL;
        gui->rompath=NULL;
        gui->rom=NULL;
      }
      g_free (str_a);
      g_free (str_b);
    }
    if (gtk_tree_model_get_iter_first(treemodel, &iter3))
      gtk_list_store_remove(GTK_LIST_STORE(treemodel), &iter);
  }
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_folder_setup(GtkButton *button, guidata *gui)
{
  gchar *text;
  gchar *path;
  gboolean recursive = FALSE;
  gboolean hide_ext = FALSE;
  gchar *filter = NULL;
  gchar *screen_a = NULL;
  gchar *screen_b = NULL;

  if (button != NULL)
  {
    GtkTreeModel *model;
    GtkTreeSelection *selection;
    GtkTreeIter iter;

    gui->resetup_folder = TRUE;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(gtk_builder_get_object(gui->settings, "treeview1")));
    if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {

      gtk_tree_model_get (model, &iter, 0, &path,
                                        1, &recursive,
                                        2, &hide_ext,
                                        3, &filter,
                                        4, &screen_a,
                                        5, &screen_b,
                                        -1);
      text = g_strconcat("<b>Folder Setup:   <i>", path, " </i></b>", NULL);
    }
    else return;
  }
  else
  {
    gui->resetup_folder = FALSE;
    text = g_strconcat("<b>Folder Setup:   <i>", gui->rompath, " </i></b>", NULL);
  }

  gtk_label_set_markup(GTK_LABEL(gtk_builder_get_object(gui->settings, "folder_label")), text);
  g_free(text);

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(gtk_builder_get_object(gui->settings, "scan")), recursive);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(gtk_builder_get_object(gui->settings, "hide_ext")), hide_ext);

  if (filter != NULL)
    gtk_entry_set_text (GTK_ENTRY(gtk_builder_get_object(gui->settings, "filters")), filter);
  else
    gtk_entry_set_text (GTK_ENTRY(gtk_builder_get_object(gui->settings, "filters")), "");

  if (screen_a != NULL)
    gtk_entry_set_text (GTK_ENTRY(gtk_builder_get_object(gui->settings, "screen_a")), screen_a);
  else
    gtk_entry_set_text (GTK_ENTRY(gtk_builder_get_object(gui->settings, "screen_a")), "");

  if (screen_b != NULL)
    gtk_entry_set_text (GTK_ENTRY(gtk_builder_get_object(gui->settings, "screen_b")), screen_b);
  else
    gtk_entry_set_text (GTK_ENTRY(gtk_builder_get_object(gui->settings, "screen_b")), "");

  g_free(filter);
  g_free(screen_a);
  g_free(screen_b);  

  gtk_widget_show(gui->folderwindow);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void open_folder(GtkWidget *sender, guidata *gui)
{
  GtkWidget *folder;

#ifdef GTK2_ENABLED
  folder = gtk_file_chooser_dialog_new(
    "Choose a ROM folder...", GTK_WINDOW(gui->topwindow),
    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, GTK_STOCK_CANCEL,
    GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL );
#else
  folder = gtk_file_chooser_dialog_new(
    "Choose a ROM folder...", GTK_WINDOW(gui->topwindow),
    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, ("_Cancel"),
    GTK_RESPONSE_CANCEL, ("_Open"), GTK_RESPONSE_ACCEPT, NULL);
#endif

  if (gtk_dialog_run(GTK_DIALOG(folder)) == GTK_RESPONSE_ACCEPT)
  {
    g_free(gui->rompath);
    gui->rompath = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (folder));

    on_folder_setup(NULL, gui);
  }
  gtk_widget_destroy(folder);
}

int descend_sort(const void * a, const void * b)
{
  return g_strcmp0(strrchr(a, G_DIR_SEPARATOR), strrchr(b, G_DIR_SEPARATOR));
}

int ascend_sort(const void * a, const void * b)
{
  return g_strcmp0(strrchr(b, G_DIR_SEPARATOR), strrchr(a, G_DIR_SEPARATOR));
}

#ifdef G_OS_WIN32 /* g_file_test is too slow on Windows */

void scan_files(gchar *romdir, guidata *gui)
{
  WIN32_FIND_DATA FindFileData;

  gchar *romdir2 = g_strconcat(romdir, G_DIR_SEPARATOR_S, "*", NULL);
  HANDLE hFind = FindFirstFile(romdir2, &FindFileData);

  while (hFind != INVALID_HANDLE_VALUE)
  {
    gchar *filepath = g_strconcat(romdir, G_DIR_SEPARATOR_S, FindFileData.cFileName, NULL);
    if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
      if (gui->listmode == TRUE && (0 != strcmp(FindFileData.cFileName, ".")
        && 0 != strcmp (FindFileData.cFileName, "..")))
      {
        scan_files(filepath, gui);
      }
    }
    else
    {
    if (g_strcmp0(gui->filters, "") != 0)
    {
      gchar **flt;
      guint i = 0;

      flt = g_strsplit (gui->filters, ",", -1);

      while (flt[i] != NULL)
      {
        gchar *ext;

        ext = g_strconcat(".", g_strstrip(flt[i]), NULL);

        if (g_str_has_suffix(FindFileData.cFileName, ext))
          gui->itemlist = g_slist_prepend(gui->itemlist, g_strdup(filepath));

        g_free(ext);
        i++;
        }
        g_strfreev(flt);
      }
      else
        gui->itemlist = g_slist_prepend(gui->itemlist, g_strdup(filepath));
    }
    g_free(filepath);

    if (!FindNextFile(hFind, &FindFileData))
    {
      FindClose(hFind);
      hFind = INVALID_HANDLE_VALUE;
    }
  }
  g_free(romdir2);
}

#else

void scan_files(gchar *romdir, guidata *gui)
{
  GDir *dir = NULL;

  dir = g_dir_open(romdir, 0, NULL);

  if (dir != NULL)
  {
    const gchar *file = NULL;

    while ((file=g_dir_read_name(dir)) != NULL)
    {
      gchar *filepath = g_strconcat(romdir, G_DIR_SEPARATOR_S, file, NULL);

      if (!g_file_test (filepath, G_FILE_TEST_IS_DIR))
      {
        if (g_strcmp0(gui->filters, "") != 0)
        {
          gchar **flt;
          guint i = 0;

          flt = g_strsplit (gui->filters, ",", -1);

          while (flt[i] != NULL)
          {
            gchar *ext;

            ext = g_strconcat(".", g_strstrip(flt[i]), NULL);

            if (g_str_has_suffix(file, ext))
              gui->itemlist = g_slist_prepend(gui->itemlist, g_strdup(filepath));

            g_free(ext);
            i++;
          }
          g_strfreev(flt);
        }
        else
          gui->itemlist = g_slist_prepend(gui->itemlist, g_strdup(filepath));
      }
      else
      {
        if (gui->listmode == TRUE)
          scan_files(filepath, gui);
      }
      g_free(filepath);
    }
    g_dir_close(dir);
  }
}

#endif

void sort_items(guidata *gui)
{
  if (gtk_tree_view_column_get_sort_order(gui->column) == GTK_SORT_ASCENDING)
    gui->itemlist = g_slist_sort(gui->itemlist, (GCompareFunc)descend_sort);
  else
    gui->itemlist = g_slist_sort(gui->itemlist, (GCompareFunc)ascend_sort);
}

void populate_list(guidata *gui)
{
  GtkTreeIter iter;
  GSList *iterator = NULL;

  for (iterator = gui->itemlist; iterator; iterator = iterator->next)
  {
    char *file;
    char *ext;
    gchar *base;

    file = (strrchr(iterator->data, G_DIR_SEPARATOR)+1);
    ext = strrchr(file, '.');

    if (ext == NULL)
      base = g_strdup(file);
    else
      base = g_strndup(file, ext-file);

    gtk_list_store_insert_with_values(gui->store, &iter, -1,
                           0, file, 1, iterator->data, 2, base, 3, ext, -1);

    g_free(base);
  }
}

void scan_dir(gchar *romdir, guidata *gui)
{
  g_slist_free_full(gui->itemlist, g_free);
  gui->itemlist = NULL;
  scan_files(romdir, gui);
  sort_items(gui);
  populate_list(gui);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void fill_list(GtkComboBox *combobox, guidata *gui)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  gboolean hide_ext;
  gint column;

  model=gtk_combo_box_get_model(GTK_COMBO_BOX(gui->cbpath));

  if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(gui->cbpath), &iter))
  {
    g_free(gui->rompath);
    g_free(gui->filters);

    gtk_tree_model_get(model, &iter, 0 ,&gui->rompath,
                                     1, &gui->listmode,
                                     2, &hide_ext,
                                     3, &gui->filters,
                                     4, &gui->path_screen_a,
                                     5, &gui->path_screen_b,
                                     -1);
    gtk_tree_view_set_model(GTK_TREE_VIEW(gui->gamelist), NULL);
    gtk_list_store_clear(gui->store);

    if (hide_ext) column = 2;
      else column = 0;

    gtk_tree_view_column_set_attributes(gui->column,
                                        GTK_CELL_RENDERER(gtk_builder_get_object(gui->builder, "cellrenderertext12")),
                                        "text", column,
                                        NULL);

    gtk_image_set_from_file (gui->screen_a, NULL);
    gtk_image_set_from_file (gui->screen_b, NULL);

    if ((g_strcmp0(gui->path_screen_a, "") == 0) && (g_strcmp0(gui->path_screen_b, "") == 0))
      gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(gui->builder, "vbox4")));
    else
      gtk_widget_show(GTK_WIDGET(gtk_builder_get_object(gui->builder, "vbox4")));

    if (gui->rompath!=NULL)
      scan_dir(gui->rompath, gui);

    gtk_tree_view_set_model(GTK_TREE_VIEW(gui->gamelist),
                            GTK_TREE_MODEL(gui->store));
    change_list(gui);

    if (gtk_tree_model_get_iter_first(gtk_tree_view_get_model(
                                  GTK_TREE_VIEW(gui->gamelist)), &iter))
    {
      gtk_tree_selection_select_iter(gtk_tree_view_get_selection(
                                  GTK_TREE_VIEW(gui->gamelist)), &iter);
    }
  }
  gtk_tree_view_column_set_sort_indicator(gui->column, TRUE);


}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void header_clicked(GtkTreeViewColumn *treeviewcolumn, guidata *gui)
{
  if (gtk_tree_view_column_get_sort_order(gui->column) == GTK_SORT_ASCENDING)
    gtk_tree_view_column_set_sort_order(gui->column, GTK_SORT_DESCENDING);
  else
    gtk_tree_view_column_set_sort_order(gui->column, GTK_SORT_ASCENDING);

   gtk_tree_view_set_model(GTK_TREE_VIEW(gui->gamelist), NULL);
  gtk_list_store_clear(gui->store);
  gui->itemlist = g_slist_reverse(gui->itemlist);
  populate_list(gui);
  gtk_tree_view_set_model(GTK_TREE_VIEW(gui->gamelist),
                          GTK_TREE_MODEL(gui->store));

  gtk_tree_view_column_set_sort_indicator(gui->column, TRUE);
}
