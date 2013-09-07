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
#include <stdlib.h>

void change_list (guidata *gui)
{
  GtkAdjustment *adjustament;
  gchar *buff, *total;

  buff=g_strdup_printf("%i", gui->n_items);
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

}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_radiomenuzip_activate(GtkMenuItem *menuitem, guidata *gui)
{

}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_radiomenucue_activate(GtkMenuItem *menuitem, guidata *gui)
{

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

int descend_sort(const void * a, const void * b)
{ 
  const char *c = *(const char **) a;
  const char *d = *(const char **) b;
  
  return g_strcmp0(d, c);
}

int ascend_sort(const void * a, const void * b)
{ 
  const char *c = *(const char **) a;
  const char *d = *(const char **) b;
  
  return g_strcmp0(c, d);
}

gint count_items(gchar *romdir, gint n_items, gboolean recursive)
{
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
        n_items++;
      else 
        if (recursive) 
          n_items = count_items(testdir, n_items, TRUE);
          
      g_free(testdir);
    }
  g_dir_close(dir); 
  }
  
  return n_items;
}

gint scan_files(gchar *romdir, gchar **list, gint i, gboolean recursive)
{
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
        list[i] = g_strconcat(file,G_DIR_SEPARATOR_S,testdir, NULL);
        i++;                             
      }
      else
      {
        if (recursive) 
          i = scan_files(testdir, list, i, TRUE);
      }
      g_free(testdir);
    }
    g_dir_close(dir);
  }
  return i;
}

void populate_list(guidata *gui)
{
  GtkTreeIter iter;
  gboolean filterzip, filtercue;
  gint i = 0;
  
  if (gtk_tree_view_column_get_sort_order(gui->column) == GTK_SORT_ASCENDING)
    qsort (gui->list, gui->n_items, sizeof(char*), ascend_sort);
  else
    qsort (gui->list, gui->n_items, sizeof(char*), descend_sort);

  while (gui->list[i])
  {
    gchar **str;
	  
	str = g_strsplit (gui->list[i], G_DIR_SEPARATOR_S, 2);
	
        filterzip = (g_str_has_suffix(str[0], ".zip") || 
                     g_str_has_suffix(str[0], ".ZIP"));
      if (filterzip)
        filtercue = FALSE;
      else                 
        filtercue = (g_str_has_suffix(str[0], ".cue") || 
                     g_str_has_suffix(str[0], ".toc") || 
                     g_str_has_suffix(str[0], ".m3u") ||
                     g_str_has_suffix(str[0], ".CUE") || 
                     g_str_has_suffix(str[0], ".TOC") || 
                     g_str_has_suffix(str[0], ".M3U"));
                      
    gtk_list_store_insert_with_values(gui->store, &iter, -1,  
                           0, str[0], 1, filterzip, 
                           2, filtercue, 3,str[1], -1);
    i++;
    g_strfreev(str);
  }
}

void scan_dir(gchar *romdir, guidata *gui)
{
  gint i;
 
  gui->n_items = count_items(romdir, 0, gui->recursive);
  if (gui->list != NULL) g_strfreev(gui->list);
  gui->list = g_new(gchar*, gui->n_items+1);
  i = scan_files(romdir, gui->list, 0, gui->recursive);
  gui->list [i]= NULL;
  populate_list(gui);
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
  populate_list(gui);
  gtk_tree_view_set_model(GTK_TREE_VIEW(gui->gamelist), 
                          GTK_TREE_MODEL(gui->store));
                          
  change_list(gui);
  gtk_tree_view_column_set_sort_indicator(gui->column, TRUE);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void fill_list(GtkComboBox *combobox, guidata *gui)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  clock_t       start, end;

  start = clock();
    
  model=gtk_combo_box_get_model(GTK_COMBO_BOX(gui->cbpath));
  gtk_tree_view_set_model(GTK_TREE_VIEW(gui->gamelist), NULL);
  gtk_list_store_clear(gui->store);
  
  if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(gui->cbpath), &iter))
  {
    g_free(gui->rompath);
    gtk_tree_model_get(model, &iter, 0 ,&gui->rompath, -1);

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
  
  end = clock();

  printf( "CPU time taken to populate list: %f\n", 
          ( (gdouble)( end - start ) ) / CLOCKS_PER_SEC );
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_recursivemenuitem_toggled(GtkCheckMenuItem *menuitem, guidata *gui)
{
  gui->recursive = gtk_check_menu_item_get_active(menuitem);
  fill_list(NULL,gui);
}
