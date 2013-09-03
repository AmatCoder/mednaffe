/*
 * list.c
 *
 * Copyright 2013 AmatCoder
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

void change_list (guidata *gui)
{
  GtkAdjustment *adjustament;
  gchar *buff, *total;
  gint n_roms = 0;

  gtk_tree_view_column_set_sort_indicator(gui->column, TRUE);
  gtk_tree_view_column_set_sort_order(gui->column,GTK_SORT_ASCENDING);

  if (gui->filtermode == 0)
  {
    n_roms = 
      gtk_tree_model_iter_n_children(GTK_TREE_MODEL(gui->modelsort), NULL);
      
    gtk_tree_view_column_set_title(gui->column, "Games");
  }
  else if (gui->filtermode == 1)
  {
    n_roms = 
      gtk_tree_model_iter_n_children(GTK_TREE_MODEL(gui->zipmodelsort), NULL);
      
    gtk_tree_view_column_set_title(gui->column, "Games (zip)");
  }
  else if (gui->filtermode == 2)
  {
    n_roms = 
      gtk_tree_model_iter_n_children(GTK_TREE_MODEL(gui->cuemodelsort), NULL);
      
    gtk_tree_view_column_set_title(gui->column, "Games (cue/toc/m3u)");
  }

  buff=g_strdup_printf("%i", n_roms);
  total = g_strconcat(" Games in list: ", buff, NULL);
  gtk_statusbar_pop(GTK_STATUSBAR(gui->sbnumber), 1);
  gtk_statusbar_push(GTK_STATUSBAR(gui->sbnumber), 1, total);
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
void on_radiomenuall_activate(GtkMenuItem *menuitem, guidata *gui)
{
  if (gui->filtermode != 0)
  {
    gtk_tree_view_set_model(
      GTK_TREE_VIEW(gui->gamelist), GTK_TREE_MODEL(gui->modelsort));
      
    gtk_tree_view_column_set_sort_indicator(gui->column, TRUE);
    gui->filtermode = 0;
    change_list(gui);
  }
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_radiomenuzip_activate(GtkMenuItem *menuitem, guidata *gui)
{
  if (gui->filtermode != 1)
  {
    gtk_tree_view_set_model(
      GTK_TREE_VIEW(gui->gamelist),GTK_TREE_MODEL(gui->zipmodelsort));
      
    gtk_tree_view_column_set_sort_indicator(gui->column, TRUE);
    gui->filtermode = 1;
    change_list(gui);
  }
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_radiomenucue_activate(GtkMenuItem *menuitem, guidata *gui)
{
  if (gui->filtermode != 2)
  {
    gtk_tree_view_set_model(
      GTK_TREE_VIEW(gui->gamelist), GTK_TREE_MODEL(gui->cuemodelsort));
    
    gtk_tree_view_column_set_sort_indicator(gui->column, TRUE);
    gui->filtermode = 2;
    change_list(gui);
  }
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void header_clicked(GtkTreeViewColumn *treeviewcolumn, guidata *gui)
{
  GtkTreeModel *sorteable;

  sorteable = gtk_tree_view_get_model(GTK_TREE_VIEW(gui->gamelist));

  if (gtk_tree_view_column_get_sort_order(gui->column) == GTK_SORT_ASCENDING)
  {
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(sorteable), 
                                                 0,GTK_SORT_DESCENDING);
                                         
    gtk_tree_view_column_set_sort_order(gui->column, GTK_SORT_DESCENDING);
  }
  else
  {
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(sorteable), 
                                                  0,GTK_SORT_ASCENDING);
                                                  
    gtk_tree_view_column_set_sort_order(gui->column, GTK_SORT_ASCENDING);
  }
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void remove_folder(GtkWidget *sender, guidata *gui)
{
  GtkTreeIter iter, iter2;
  GtkListStore *combostore;

  combostore = 
    GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(gui->cbpath)));
    
  if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(gui->cbpath), &iter))
  {
    if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(combostore), &iter2))
      gtk_list_store_remove(combostore, &iter);

    gtk_statusbar_pop(GTK_STATUSBAR(gui->sbnumber), 1);
    gtk_statusbar_push(GTK_STATUSBAR(gui->sbnumber), 1,
                                                  " Games in list: 0 ");
                                                  
    gtk_statusbar_pop(GTK_STATUSBAR(gui->sbname), 1);
    gtk_statusbar_push(GTK_STATUSBAR(gui->sbname), 1,
                                                " Game selected: None");
                                                
    g_free(gui->rompath);
    g_free(gui->rom);
    gui->rompath=NULL;
    gui->rom=NULL;
  }
}

void add_combo(GtkComboBox *combopath, const char *string)
{
  GtkTreeIter iter;
  GtkListStore *combostore;

  combostore = GTK_LIST_STORE(gtk_combo_box_get_model(combopath));

  gtk_list_store_prepend(combostore, &iter);
  gtk_list_store_set(combostore, &iter, 0, string, -1);

  gtk_combo_box_set_active(combopath, 0);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void open_folder(GtkWidget *sender, guidata *gui)
{
  GtkWidget *folder;

  folder = gtk_file_chooser_dialog_new(
    "Choose a ROM folder...", NULL,
    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, GTK_STOCK_CANCEL,
    GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL );

  if (gtk_dialog_run(GTK_DIALOG(folder)) == GTK_RESPONSE_ACCEPT)
  {
    gchar *path;
    
    path = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (folder));
    add_combo(GTK_COMBO_BOX(gui->cbpath), path);
    g_free(path);
  }
  gtk_widget_destroy(folder);
}

void scan_dir(gchar *romdir, guidata *gui)
{
  GtkTreeIter iter;
  gboolean filterzip, filtercue;
  GDir *dir = NULL;
  
  dir = g_dir_open(romdir, 0, NULL);

  if (dir != NULL)
  {
    const gchar *file = NULL;
        
    while ((file=g_dir_read_name(dir)) != NULL)
    {
      gchar *testdir = NULL;
          
      testdir = g_strconcat(romdir, G_DIR_SEPARATOR_S, file, NULL);
          
      if (!g_file_test (testdir, G_FILE_TEST_IS_DIR))
      {
        filterzip = (g_str_has_suffix(file, ".zip") || 
                     g_str_has_suffix(file, ".ZIP"));
                         
        filtercue = (g_str_has_suffix(file, ".cue") || 
                     g_str_has_suffix(file, ".toc") || 
                     g_str_has_suffix(file, ".m3u") ||
                     g_str_has_suffix(file, ".CUE") || 
                     g_str_has_suffix(file, ".TOC") || 
                     g_str_has_suffix(file, ".M3U"));
                         
        gtk_list_store_append(gui->store, &iter);
        gtk_list_store_set(gui->store, &iter, 
                           0, file, 
                           1, filterzip, 
                           2, filtercue, 
                           3, testdir, -1);
                                        
      }
      else
      {
        if (gui->recursive) 
          scan_dir(testdir,gui);
      }
      g_free(testdir);
    }
    g_dir_close(dir);
  }
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void fill_list(GtkComboBox *combobox, guidata *gui)
{
  GtkTreeModel *model;
  GtkTreeIter iter;

  model=gtk_combo_box_get_model(GTK_COMBO_BOX(gui->cbpath));
  gtk_list_store_clear(gui->store);
  
  if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(gui->cbpath), &iter))
  {
    g_free(gui->rompath);
    gtk_tree_model_get(model, &iter, 0 ,&gui->rompath, -1);

    gtk_tree_sortable_set_sort_column_id(
      GTK_TREE_SORTABLE(gui->modelsort), -1, GTK_SORT_ASCENDING);
      
    gtk_tree_sortable_set_sort_column_id(
      GTK_TREE_SORTABLE(gui->zipmodelsort), -1, GTK_SORT_ASCENDING);
      
    gtk_tree_sortable_set_sort_column_id(
      GTK_TREE_SORTABLE(gui->cuemodelsort), -1, GTK_SORT_ASCENDING);

    if (gui->rompath!=NULL)
    {
      scan_dir(gui->rompath, gui);
      
      gtk_tree_sortable_set_sort_column_id(
        GTK_TREE_SORTABLE(gui->modelsort), 0, GTK_SORT_ASCENDING);
        
      gtk_tree_sortable_set_sort_column_id(
        GTK_TREE_SORTABLE(gui->zipmodelsort), 0, GTK_SORT_ASCENDING);
        
      gtk_tree_sortable_set_sort_column_id(
        GTK_TREE_SORTABLE(gui->cuemodelsort), 0, GTK_SORT_ASCENDING);
        
    }

    if (gtk_tree_model_get_iter_first(gtk_tree_view_get_model(
                                  GTK_TREE_VIEW(gui->gamelist)), &iter))
    {  
      gtk_tree_selection_select_iter(gtk_tree_view_get_selection(
                                  GTK_TREE_VIEW(gui->gamelist)), &iter);
    }
    change_list(gui);
  }
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_recursivemenuitem_toggled(GtkCheckMenuItem *menuitem, guidata *gui)
{
  gui->recursive = gtk_check_menu_item_get_active(menuitem);
  fill_list(NULL,gui);
}
