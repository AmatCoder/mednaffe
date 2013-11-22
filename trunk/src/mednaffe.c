/*
 * mednaffe.c
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


/*
  Compile me with:

  gcc -O2 -std=c99 -Wall -DGTK2_ENABLED -o mednaffe about.c 
  active.c command.c gui.c list.c toggles.c mednaffe.c 
  $(pkg-config --cflags --libs gtk+-2.0 gmodule-export-2.0)

*/

#include "common.h"
#include "toggles.h"
#include "logo.h"
#include "mednaffe_glade.h"
#include "system_glade.h"

#ifdef G_OS_WIN32
  #include <windows.h>
#endif

void show_error(const gchar *message)
{
  GtkWidget *dialog;

  dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                   GTK_MESSAGE_ERROR,
                                   GTK_BUTTONS_CLOSE,
                                   message);
                                   
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
  printf(message);
}

void global_selected(GtkTreeSelection *treeselection, guidata *gui)
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  gint page;

  model=gtk_tree_view_get_model(GTK_TREE_VIEW(gui->globalist));

  if (gtk_tree_selection_get_selected(treeselection, &model, &iter))
  {
    gtk_tree_model_get(model, &iter, 1, &page, -1);
    gtk_notebook_set_current_page (GTK_NOTEBOOK(gui->notebook2), page);
  }
}

void system_selected(GtkTreeSelection *treeselection, guidata *gui)
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  GSList *iterator;
  gint page;
  
  model=gtk_tree_view_get_model(GTK_TREE_VIEW(gui->systemlist));

  if (gtk_tree_selection_get_selected(treeselection, &model, &iter))
  {
    GtkWidget *embed;

    g_free(gui->system);
    gtk_tree_model_get(model, &iter, 1, &gui->system,  -1);
    gui->changing = TRUE;
    
    for (iterator = gui->dinlist; iterator; iterator = iterator->next)
    {
      if (GTK_IS_TOGGLE_BUTTON(iterator->data))
        set_toogle(iterator->data, gui);
      else if (GTK_IS_COMBO_BOX(iterator->data))
        set_combo(iterator->data, gui);
      else
        set_spin(iterator->data, gui);
    }
    g_slist_free(iterator);
    
    gui->changing = FALSE;
    embed = GTK_WIDGET(gtk_builder_get_object(gui->specific,
                                            (const char *)gui->system));

    gtk_notebook_remove_page(GTK_NOTEBOOK(gui->notebook),0);
    gtk_notebook_prepend_page(GTK_NOTEBOOK(gui->notebook),
                                                  embed, gui->setlabel);

    gtk_notebook_set_current_page(GTK_NOTEBOOK(gui->notebook), 0);
    
    gtk_tree_model_get(model, &iter, 2, &page, -1);
    if (page==4)
      gtk_widget_show(GTK_WIDGET(
        gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui->notebook), 3)));
    else
      gtk_widget_hide(GTK_WIDGET(
        gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui->notebook), 3)));
  }
}

void game_selected(GtkTreeSelection *treeselection, guidata *gui)
{
  GtkTreeIter iter;
  GtkTreeModel *model;

  model=gtk_tree_view_get_model(GTK_TREE_VIEW(gui->gamelist));

  if (gtk_tree_selection_get_selected(treeselection, &model, &iter))
  {
    gchar *selected = NULL;
    
    g_free(gui->rom);
    g_free(gui->fullpath);
    gtk_tree_model_get(model, &iter, 0, &gui->rom, 1, &gui->fullpath, -1);
    gtk_statusbar_pop(GTK_STATUSBAR(gui->sbname), 1);
    selected = g_strconcat(" Game selected: ", gui->rom, NULL);
    gtk_statusbar_push(GTK_STATUSBAR(gui->sbname), 1, selected);
    g_free(selected);
  }
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void quit(GtkWidget *widget, guidata *gui)
{
  GtkTreeIter iter;
  GtkTreeModel *combostore;
  gchar *conf_file;
  gchar *conf;
  FILE *file;
  GKeyFile *key_file;
  gchar **array;
  gint n_items, a_item;
  gboolean valid;
  GtkWidget *option;
  GList *list = NULL;
  GList *iterator = NULL;
  gint i=0;

  printf("[Mednaffe] Exiting Mednaffe\n");

  a_item=gtk_combo_box_get_active(GTK_COMBO_BOX(gui->cbpath));
  combostore = gtk_combo_box_get_model(GTK_COMBO_BOX(gui->cbpath));
  
  #ifdef G_OS_WIN32
    conf_file=g_strconcat(g_path_get_dirname(gui->binpath), "\\mednaffe.ini", NULL);
  #else
    conf_file=g_strconcat(g_get_user_config_dir(),"/mednaffe.conf", NULL);
  #endif
  
  n_items = gtk_tree_model_iter_n_children(combostore, NULL);

  /*const gchar* array[n_items];*/
  array = g_new(gchar *, n_items+1);
  array[n_items]=NULL;

  key_file=g_key_file_new();
  /*g_key_file_set_list_separator(key_file,  0x0D);*/

  valid = gtk_tree_model_get_iter_first (combostore, &iter);
  while (valid)
  {
    gtk_tree_model_get (combostore, &iter, 0, &array[i], -1);
    i++;
    valid = gtk_tree_model_iter_next (combostore, &iter);
  }

  g_key_file_set_string_list(key_file, "GUI", "Folders",
                                        (const gchar **)array, n_items);
                                        
  g_key_file_set_integer(key_file, "GUI", "Last Folder", a_item);
                         
  option = GTK_WIDGET(gtk_builder_get_object(gui->builder,"showtooltips"));         
  g_key_file_set_boolean(key_file, "GUI", "Tooltips",
                         gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(option)));
                         
  option = GTK_WIDGET(gtk_builder_get_object(gui->builder,"remembersize"));                         
  g_key_file_set_boolean(key_file, "GUI", "RememberSize",
                         gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(option))); 
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(option))) 
    {
	  gint width;
	  gint height;
	  
	  gtk_window_get_size(GTK_WINDOW(gui->topwindow), &width, &height);
	  if (width && height)
	  {
	    g_key_file_set_integer(key_file, "GUI", "Width", width);                   
        g_key_file_set_integer(key_file, "GUI", "Height", height);
	  }
	} 
	                     
  g_key_file_set_integer(key_file, "GUI", "Filter", gui->filter);                   
  g_key_file_set_integer(key_file, "GUI", "View Mode", gui->listmode);                     
  g_key_file_set_integer(key_file, "GUI", "ActionLaunch", gui->state);

  if (gtk_tree_view_column_get_sort_order(gui->column) == GTK_SORT_DESCENDING)                                  
    g_key_file_set_boolean(key_file, "GUI", "Reverse Sort", TRUE);

  list = g_hash_table_get_keys(gui->clist);

  for (iterator = list; iterator; iterator = iterator->next)
  {
    gchar *value;
    
    iterator->data = ((gchar *)iterator->data)+1;
    value=g_strdup(g_hash_table_lookup(gui->hash, iterator->data));
    iterator->data = ((gchar *)iterator->data)-1;
    g_key_file_set_string(key_file, "Emulator", iterator->data, value);
    g_free(value);
  }
  g_list_free(list);

  conf=g_key_file_to_data(key_file, NULL, NULL);

  file=fopen(conf_file, "w");
  fputs(conf, file);
  fclose(file);

  g_key_file_free(key_file);
  g_free(conf);
  g_strfreev(array);
  g_free(conf_file);

  /*while(n_items>0)
  {
    n_items--;
  //g_print("%s\n", array[n_items]);*/
  /*g_free((gpointer *)array[n_items]);
  }*/

  /* To free items makes happy to Valgrind ;-) */
  g_object_unref(G_OBJECT(gui->builder));
  g_object_unref(G_OBJECT(gui->specific));
  g_hash_table_destroy(gui->hash);
  g_hash_table_destroy(gui->clist);
  g_slist_free(gui->dinlist);
  g_free(gui->binpath);
  g_free(gui->fullpath);
  g_free(gui->rompath);
  g_free(gui->rom);
  g_free(gui->system);
  g_slist_free_full(gui->itemlist, g_free);

  gtk_main_quit();
}

void load_conf(guidata *gui)
{
  GtkTreeIter iter;
  GtkListStore *combostore;
  gchar *conf_file;
  gchar **folders = NULL;
  GKeyFile *key_file;
  gboolean value;
  gsize n_items = 0;
  gint a_item = -1;
  gint state;
  GError *err = NULL;
  gsize length = 0;

  combostore = GTK_LIST_STORE(
                   gtk_combo_box_get_model(GTK_COMBO_BOX(gui->cbpath)));
  #ifdef G_OS_WIN32
    conf_file=g_strconcat(g_path_get_dirname(gui->binpath), "\\mednaffe.ini", NULL);
  #else
    conf_file=g_strconcat(g_get_user_config_dir(), "/mednaffe.conf", NULL);
  #endif

  key_file=g_key_file_new();
  /*g_key_file_set_list_separator(key_file,  0x0D);*/

  if (g_key_file_load_from_file(key_file, conf_file,
                                G_KEY_FILE_NONE, NULL))
  {
    GtkWidget *option;
    gchar **ffekeys;

    folders=g_key_file_get_string_list(key_file, "GUI", "Folders",
                                                        &n_items, NULL);
                                                        
    a_item=g_key_file_get_integer(key_file, "GUI", "Last Folder", NULL);

    if (folders!=NULL)
    {
      while (n_items>0)
      {
        n_items--;
        gtk_list_store_prepend(combostore, &iter);
        gtk_list_store_set(combostore, &iter, 0, folders[n_items], -1);
      }
    }
                                        
    value = g_key_file_get_boolean(key_file, "GUI", "Reverse Sort", &err);
    
     if (err==NULL)
     {
      if (value) 
        gtk_tree_view_column_set_sort_order(gui->column, GTK_SORT_DESCENDING);
     }
    else
    {
      g_error_free (err);
      err=NULL;
    }
    
    option = GTK_WIDGET(gtk_builder_get_object(gui->builder,
                                               "showtooltips"));      
    value = g_key_file_get_boolean(key_file, "GUI", "Tooltips", &err);    
    if (err==NULL)
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(option),value);
    else
    {
      g_error_free (err);
      err=NULL;
    }
    
    option = GTK_WIDGET(gtk_builder_get_object(gui->builder,
                                               "remembersize"));    
    value = g_key_file_get_boolean(key_file, "GUI", "RememberSize", &err); 
    if (err==NULL)
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(option),value);
    else
    {
      g_error_free (err);
      err=NULL;
    }
    if (value)
    {
	  gint width;
	  gint height;
	  
	  width = g_key_file_get_integer(key_file, "GUI", "Width", NULL);
	  height = g_key_file_get_integer(key_file, "GUI", "Height", NULL);
	  
	  if (width && height)
	    gtk_window_resize(GTK_WINDOW(gui->topwindow), width, height);
	}
    
    option = GTK_WIDGET(gtk_builder_get_object(gui->builder,
                                               "recursivemenuitem"));
    value = g_key_file_get_integer(key_file, "GUI", "Recursive", &err);
    
    if (err==NULL)
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(option),value);
    else
    {
      g_error_free (err);
      err=NULL;
    }

    state=g_key_file_get_integer(key_file, "GUI", "Filter", NULL);

    switch (state)
    {
      case 1:
        option = GTK_WIDGET(gtk_builder_get_object(gui->builder, "radiomenuzip"));            
        gtk_menu_item_activate (GTK_MENU_ITEM(option));
      break;
      
      case 2:
        option = GTK_WIDGET(gtk_builder_get_object(gui->builder, "radiomenucue"));            
        gtk_menu_item_activate (GTK_MENU_ITEM(option));
      break;
      
      default:
      break;
    }
    
    state=g_key_file_get_integer(key_file, "GUI", "ActionLaunch", NULL);

    switch (state)
    {
      case 2:
        option = GTK_WIDGET(gtk_builder_get_object(gui->builder, "rbhide"));
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(option), TRUE);
      break;
      
      case 1:
        option = GTK_WIDGET(gtk_builder_get_object(gui->builder, "rbminimize"));            
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(option), TRUE);
      break;
      
      default:
      break;
    }
    
    state=g_key_file_get_integer(key_file, "GUI", "View Mode", NULL);

    switch (state)
    {
      case 1:
        option = GTK_WIDGET(gtk_builder_get_object(gui->builder, "recursivemenu"));            
        gtk_menu_item_activate (GTK_MENU_ITEM(option));
      break;
      
      default:
      break;
    }

    ffekeys = g_key_file_get_keys(key_file, "Emulator", &length, NULL);

    if (length>0)
    {
      gint i=0;

      while (ffekeys[i])
      {
        gchar *ffecopy;

        ffecopy = g_strdup((ffekeys[i])+1);
        g_hash_table_insert(gui->hash, ffecopy,
                            g_key_file_get_string(key_file, "Emulator", 
                            ffekeys[i], NULL));

        g_hash_table_replace(gui->clist, ffekeys[i], ffekeys[i]);
        i++;
      }
    }
    g_free(ffekeys);
  }
  
  g_strfreev(folders);
  g_key_file_free(key_file);
  g_free(conf_file);

  if (a_item > -1)
    gtk_combo_box_set_active(GTK_COMBO_BOX(gui->cbpath), a_item);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void delete(GtkWidget *widget, GdkEvent *event, guidata *gui)
{
  quit(widget,gui);
}

gchar *get_cfg(const gchar *home)
{
  gchar *cfg_path = NULL;

  /* Search for mednafen configuration file */
  #ifdef G_OS_WIN32
    cfg_path = g_strconcat(home, "\\mednafen-09x.cfg", NULL);
  #else
    cfg_path = g_strconcat(home, "/.mednafen/mednafen-09x.cfg", NULL);
  #endif

  if (g_file_test (cfg_path, G_FILE_TEST_IS_REGULAR))
    printf("[Mednaffe] Mednafen 09x configuration file found\n");
  else
  {
  #ifdef G_OS_WIN32
    cfg_path = g_strconcat(home, "\\mednafen.cfg", NULL);
  #else
    cfg_path = g_strconcat(home, "/.mednafen/mednafen.cfg", NULL);
  #endif  
    if (g_file_test (cfg_path, G_FILE_TEST_IS_REGULAR))
      printf("[Mednaffe] Mednafen 08x configuration file found\n");
    else
      cfg_path = NULL;
  }
  
  return cfg_path;
}

int main(int argc, char **argv)
{
  guidata gui;
  gchar *cfg_path;
  const gchar *home = NULL;

  /* Init GTK+ */
  gtk_init(&argc, &argv);
  printf("[Mednaffe] Starting Mednaffe 0.4\n");

  /* Search for HOME variable*/
  #ifndef G_OS_WIN32
    home = g_getenv ("HOME");
    if (!home) home = g_get_home_dir();
    if (!home)
    {
      show_error("Error searching for home variable!\n");
      return 1;
    }
  #endif
  
  /* Try to search mednafen bin & cfg */
  gui.binpath = g_find_program_in_path("mednafen");
  if (gui.binpath==NULL)
  {
    show_error(
      "Error: Mednafen executable is not installed in path!\n");
    return 1;
  }
  
  #ifdef G_OS_WIN32
    home = g_path_get_dirname(gui.binpath);
  #endif
  cfg_path = get_cfg(home);

  if (!cfg_path)
  {
    #ifdef G_OS_WIN32
      gchar *wincommand[2] = {gui.binpath, NULL};
      g_spawn_sync( NULL, wincommand, NULL,
                    G_SPAWN_STDOUT_TO_DEV_NULL, NULL,
                    NULL, NULL, NULL, NULL, NULL );
      Sleep(1000); /* race condition? */
    #else
      g_spawn_command_line_sync(gui.binpath, NULL, NULL, NULL, NULL);
      sleep (1); /* race condition? */
    #endif
    cfg_path = get_cfg(home);
    if (!cfg_path)
    {
      show_error("No mednafen configuration file found...\n");
      return 1;
    }
  }

  /* Create new GtkBuilder objects */
  gui.builder = gtk_builder_new();
  
  if (!gtk_builder_add_from_string(gui.builder, mednaffe_glade, -1, NULL))
  {
    g_object_unref(G_OBJECT(gui.builder));
    show_error("Error reading glade file!\n");
    return 1;
  }

  gui.specific = gtk_builder_new();
  
  if (!gtk_builder_add_from_string(gui.specific, system_glade, -1, NULL))
  {
    g_object_unref(G_OBJECT(gui.specific));
    show_error("Error reading glade file!\n");
    return 1;
  }

  /* Get widget pointers from UI */
  gui.topwindow = GTK_WIDGET(gtk_builder_get_object(gui.builder,
                             "topwindow"));
                             
  gui.cbpath = GTK_WIDGET(gtk_builder_get_object(gui.builder, 
                          "cbpath"));
                          
  gui.sbname = GTK_WIDGET(gtk_builder_get_object(gui.builder, 
                          "sbname"));
                          
  gui.sbnumber = GTK_WIDGET(gtk_builder_get_object(gui.builder,
                            "sbnumber"));
                            
  gui.gamelist = GTK_WIDGET(gtk_builder_get_object(gui.builder,
                            "gamelist"));
                            
  gui.scrollwindow = GTK_WIDGET(gtk_builder_get_object(gui.builder,
                                "scrollwindow"));
                                
  gui.systemlist = GTK_WIDGET(gtk_builder_get_object(gui.builder,
                              "systemlist"));
                              
  gui.globalist = GTK_WIDGET(gtk_builder_get_object(gui.builder,
                             "globalist"));
                             
  gui.notebook = GTK_WIDGET(gtk_builder_get_object(gui.builder,
                            "notebook3"));
                            
  gui.notebook2 = GTK_WIDGET(gtk_builder_get_object(gui.builder,
                             "notebook2"));  
                                                            
  gui.setlabel = GTK_WIDGET(gtk_builder_get_object(gui.builder,
                            "settings_label"));                   
  g_object_ref(gui.setlabel);
  
  /* Connect signals */
  gtk_builder_connect_signals(gui.builder, &gui);
  gtk_builder_connect_signals(gui.specific, &gui);

  g_signal_connect(
    gtk_tree_view_get_selection(GTK_TREE_VIEW(gui.gamelist)), 
        "changed", G_CALLBACK(game_selected), &gui);
        
  g_signal_connect(
    gtk_tree_view_get_selection(GTK_TREE_VIEW(gui.systemlist)), 
        "changed", G_CALLBACK(system_selected), &gui);
        
  g_signal_connect(
    gtk_tree_view_get_selection(GTK_TREE_VIEW(gui.globalist)), 
        "changed", G_CALLBACK(global_selected), &gui);

  /* Create store and models */
  gui.store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

  gtk_tree_view_set_model(
    GTK_TREE_VIEW(gui.gamelist), GTK_TREE_MODEL(gui.store));

  gui.column = gtk_tree_view_get_column(GTK_TREE_VIEW(gui.gamelist), 0);
  gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(gui.store), 
    GTK_TREE_SORTABLE_UNSORTED_SORT_COLUMN_ID, GTK_SORT_ASCENDING);


  /* Set initial values */
  gui.listmode = 0;
  gui.filter = 0;
  gui.itemlist = NULL;
  gui.state = 0;
  gui.executing = FALSE;
  gui.fullpath = NULL;
  gui.rompath = NULL;
  gui.rom = NULL;
  gui.command = NULL;
  gui.changing = FALSE;
  gui.dinlist = NULL;
  gui.system = NULL;
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(gui.notebook2),FALSE);

  /* Set statusbar messages */
  if (GTK_MAJOR_VERSION==2)
  {
    gtk_statusbar_set_has_resize_grip(GTK_STATUSBAR(gui.sbname),FALSE);
    gtk_statusbar_set_has_resize_grip(GTK_STATUSBAR(gui.sbnumber),FALSE);
  }
  
  gtk_statusbar_push
    (GTK_STATUSBAR(gui.sbname), 1, " Game selected: None");
  gtk_statusbar_push
    (GTK_STATUSBAR(gui.sbnumber), 1, " Games in list: 0");

  /* Read configuration from mednafen*.cfg */
  if (!read_cfg(cfg_path, &gui))
  {
    show_error(
    "Mednafen version is not compatible.\nYou need 0.9.28/29/31/32-WIP version.\n");
    return 1; /* Items are not freed here */
  }
  g_free(cfg_path);

  /* Load mednaffe config file */
  load_conf(&gui);
  set_values(gui.builder, &gui);
  set_values(gui.specific, &gui);
  select_rows(&gui);

  /* Create icon */
  gtk_window_set_icon(GTK_WINDOW(gui.topwindow), 
                    gdk_pixbuf_new_from_inline (-1, logo, FALSE, NULL));
                    
  /* Show window */
  gtk_widget_show(gui.topwindow);

  /* Start main loop */
  gtk_main();

  return 0;
}
