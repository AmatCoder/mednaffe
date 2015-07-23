/*
 * mednaffe.c
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


/*
  Compile me with:

  gcc -g -std=c99 -Wall -DGTK2_ENABLED -o mednaffe about.c
  active.c command.c gui.c prefs.c list.c toggles.c
  log.c input.c joystick_linux.c md5.c mednaffe.c
  $(pkg-config --cflags --libs gtk+-2.0 gmodule-export-2.0)

*/

#include "common.h"
#include "toggles.h"
#include "log.h"
#include "prefs.h"
#include "logo.h"
#include "mednaffe_glade.h"
#include "system_glade.h"
#include "settings_glade.h"

#ifdef G_OS_WIN32
  #include <windows.h>
#endif

void show_error(const gchar *message)
{
  GtkWidget *dialog;

  dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                   GTK_MESSAGE_ERROR,
                                   GTK_BUTTONS_CLOSE,
                                   "%s", message);

  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
  printf("%s", message);
}

gchar* show_chooser(const gchar *message)
{
  GtkWidget *exe;
  gchar *filename = NULL;
  GtkWidget *dialog;

  dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                   GTK_MESSAGE_QUESTION,
                                   GTK_BUTTONS_YES_NO,
                                   "%s", message);

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_NO)
  {
    gtk_widget_destroy (dialog);
    return NULL;
  }
  gtk_widget_destroy (dialog);

  exe = gtk_file_chooser_dialog_new(
    "Choose a mednafen executable...", NULL,
    GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL,
    GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL );

  GtkFileFilter* filter = gtk_file_filter_new();
  gtk_file_filter_add_pattern(filter, "mednafen.exe");
  gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(exe), filter);

  if (gtk_dialog_run(GTK_DIALOG(exe)) == GTK_RESPONSE_ACCEPT)
  {
    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(exe));
  }
  gtk_widget_destroy(exe);

  return filename;
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

  model = gtk_tree_view_get_model(GTK_TREE_VIEW(gui->systemlist));

  if (gtk_tree_selection_get_selected(treeselection, &model, &iter))
  {
    GtkWidget *embed;
    GtkContainer *container;
    gchar *astring;

    if (gui->system)
    {
      gui->system++;
      astring = g_strconcat(gui->system, "inputbox", NULL);
      gui->system--;
      container = GTK_CONTAINER(gtk_builder_get_object(gui->builder, "inputbox"));
      embed = GTK_WIDGET(gtk_builder_get_object(gui->specific, (const char *)astring));
      g_free(astring);
      gtk_container_remove (container, embed);

      container = GTK_CONTAINER(gtk_builder_get_object(gui->builder,
                                                     "embox"));
      embed = GTK_WIDGET(gtk_builder_get_object(gui->specific,
                                            (const char *)gui->system));

      gtk_container_remove (container, embed);

      g_free(gui->system);
    }
    g_free(gui->fullsystem);
    gtk_tree_model_get(model, &iter, 0, &gui->fullsystem,
                                     1, &gui->system,
                                     2, &gui->pagesys,  -1);
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
    container = GTK_CONTAINER(gtk_builder_get_object(gui->builder,
                                                     "embox"));
    embed = GTK_WIDGET(gtk_builder_get_object(gui->specific,
                                            (const char *)gui->system));
    gtk_container_add (container, embed);
    gui->system++;
    astring = g_strconcat(gui->system, "inputbox", NULL);
    gui->system--;
    container = GTK_CONTAINER(gtk_builder_get_object(gui->builder, "inputbox"));
    embed = GTK_WIDGET(gtk_builder_get_object(gui->specific, (const char *)astring));
    gtk_container_add (container, embed);
    gtk_box_set_child_packing(GTK_BOX(container), embed, FALSE, FALSE, 0, GTK_PACK_START);
    g_free(astring);
    /*gtk_notebook_remove_page(GTK_NOTEBOOK(gui->notebook),0);
    gtk_notebook_prepend_page(GTK_NOTEBOOK(gui->notebook),
                                                  embed, gui->setlabel);*/

    gtk_notebook_set_current_page(GTK_NOTEBOOK(gui->notebook), 0);

    if (gui->pagesys==4)
      gtk_widget_show(GTK_WIDGET(
        gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui->notebook), 4)));
    else
      gtk_widget_hide(GTK_WIDGET(
        gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui->notebook), 4)));
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
void on_cell_toggled(GtkCellRendererToggle *cell_renderer,
                                    gchar *path,
                                    guidata *gui)
{
  GtkTreeIter iter, iter2, iter3;
  GtkTreeModel *model;
  GtkTreeModel *model2;
  gboolean bool;
  gint row, row2;

  model = gtk_tree_view_get_model(GTK_TREE_VIEW(gtk_builder_get_object(
                                           gui->settings,"se_treeview")));
  model2 = GTK_TREE_MODEL(gtk_builder_get_object(
                                           gui->builder, "liststore3"));

  if (gtk_tree_model_get_iter_from_string(model, &iter, path))
  {
  gtk_tree_model_get(model, &iter, 1, &bool, 2, &row, -1);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter, 1, (!bool), -1);
    gtk_tree_model_get_iter_first(model2, &iter2);

    do
    {
    gtk_tree_model_get(model2, &iter2, 2, &row2, -1);
    if (row==row2)
    {
      gtk_list_store_set(GTK_LIST_STORE(model2), &iter2, 3, (!bool), -1);
      break;
    }
    } while (gtk_tree_model_iter_next(model2, &iter2));

    if (gtk_tree_model_get_iter_first(
        gtk_tree_view_get_model(GTK_TREE_VIEW(gui->systemlist)), &iter3))
    {
      gtk_tree_selection_select_iter(
        gtk_tree_view_get_selection(GTK_TREE_VIEW(gui->systemlist)), &iter3);

        gtk_tree_model_filter_refilter(GTK_TREE_MODEL_FILTER(
          gtk_tree_view_get_model(GTK_TREE_VIEW(gui->systemlist))));
    }

    if (!gtk_tree_selection_get_selected(gtk_tree_view_get_selection(
                           GTK_TREE_VIEW(gui->systemlist)), NULL, NULL))
      gtk_widget_hide(gui->notebook); else gtk_widget_show(gui->notebook);
  }
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void quit(GtkWidget *widget, guidata *gui)
{
  save_prefs(gui);
  printf("Exiting Mednaffe...\n");

  /* To free items makes happy to Valgrind ;-) */
  g_object_unref(G_OBJECT(gui->builder));
  g_object_unref(G_OBJECT(gui->specific));
  g_object_unref(G_OBJECT(gui->settings));
  g_hash_table_destroy(gui->hash);
  g_hash_table_destroy(gui->clist);
  g_slist_free(gui->dinlist);
  g_free(gui->binpath);
  g_free(gui->fullpath);
  g_free(gui->rompath);
  g_free(gui->rom);
  g_free(gui->fullsystem);
  g_free(gui->system);
  g_free(gui->cfgfile);
  g_free(gui->port)
  ;g_free(gui->treepath);
  g_slist_free_full(gui->itemlist, g_free);

  gtk_main_quit();
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void delete(GtkWidget *widget, GdkEvent *event, guidata *gui)
{
  quit(widget,gui);
}

gchar *get_cfg(const gchar *home, guidata *gui)
{
  gchar *cfg_path;

  /* Search for mednafen configuration file */
  #ifdef G_OS_WIN32
    cfg_path = g_strconcat(home, "\\mednafen-09x.cfg", NULL);
  #else
    cfg_path = g_strconcat(home, "/.mednafen/mednafen-09x.cfg", NULL);
  #endif

  if (g_file_test (cfg_path, G_FILE_TEST_IS_REGULAR))
    print_log("Starting Mednaffe 0.7\n\
Mednafen 09x configuration file found.\n", FE, gui);
  else return NULL;

  /*{
  #ifdef G_OS_WIN32
    cfg_path = g_strconcat(home, "\\mednafen.cfg", NULL);
  #else
    cfg_path = g_strconcat(home, "/.mednafen/mednafen.cfg", NULL);
  #endif
    if (g_file_test (cfg_path, G_FILE_TEST_IS_REGULAR))
      printf(" Mednafen 08x configuration file found\n");
    else
      cfg_path = NULL;
  }*/

  return cfg_path;
}

int main(int argc, char **argv)
{
  guidata gui;
  const gchar *home = NULL;
  gchar *stout = NULL;
  GKeyFile *key_file;

  /* Init GTK+ */
  gtk_init(&argc, &argv);

  /* Create new GtkBuilder objects */
  gui.builder = gtk_builder_new();
  if (!gtk_builder_add_from_string(gui.builder, mednaffe_glade, -1, NULL))
  {
    printf("Error reading mednaffe glade file!\n");
    return 1;
  }

  gui.specific = gtk_builder_new();
  if (!gtk_builder_add_from_string(gui.specific, system_glade, -1, NULL))
  {
    show_error("Error reading system glade file!\n");
    return 1;
  }

  gui.settings = gtk_builder_new();
  if (!gtk_builder_add_from_string(gui.settings, settings_glade, -1, NULL))
  {
    show_error("Error reading settings glade file!\n");
    return 1;
  }

  /* Get widget pointers from UI */
  gui.topwindow = GTK_WIDGET(gtk_builder_get_object(gui.builder,
                             "topwindow"));

  gui.prefwindow = GTK_WIDGET(gtk_builder_get_object(gui.settings,
                             "dialog1"));

  gui.inputwindow = GTK_WIDGET(gtk_builder_get_object(gui.specific,
                             "inputdialog"));

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

  gui.launch = GTK_WIDGET(gtk_builder_get_object(gui.builder,
                             "button1"));

  gui.textfe = GTK_TEXT_BUFFER(gtk_builder_get_object(gui.builder,
                              "textbufferfe"));

  gui.textout = GTK_TEXT_BUFFER(gtk_builder_get_object(gui.builder,
                                "textbufferout"));

  /*gui.setlabel = GTK_WIDGET(gtk_builder_get_object(gui.builder,
                            "settings_label"));
  g_object_ref(gui.setlabel);*/

  /* Connect signals */
  gtk_builder_connect_signals(gui.builder, &gui);
  gtk_builder_connect_signals(gui.specific, &gui);
  gtk_builder_connect_signals(gui.settings, &gui);
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

  gtk_tree_model_filter_set_visible_column(GTK_TREE_MODEL_FILTER(
    gtk_tree_view_get_model(GTK_TREE_VIEW(gui.systemlist))), 3);

  /* Windows-Only options */
  #ifdef G_OS_WIN32
  gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(gui.builder,
                         "-video.disable_composition")), TRUE);
  #endif

  /* Set initial values */
  gui.listmode = 0;
  gui.filter = 0;
  gui.itemlist = NULL;
  gui.state = 0;
  gui.executing = FALSE;
  gui.changed = FALSE;
  gui.binpath = NULL;
  gui.fullpath = NULL;
  gui.rompath = NULL;
  gui.rom = NULL;
  gui.command = NULL;
  gui.changing = FALSE;
  gui.dinlist = NULL;
  gui.fullsystem = NULL;
  gui.system = NULL;
  gui.port = NULL;
  gui.treepath = NULL;
  gui.editable = NULL;
  gui.inputedited = TRUE;
  gui.m_error = NULL;

  gui.clist = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
  gui.hash = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(gui.notebook2),FALSE);
  gtk_window_set_transient_for(GTK_WINDOW(gui.prefwindow), GTK_WINDOW(gui.topwindow));
  gtk_window_set_transient_for(GTK_WINDOW(gui.inputwindow), GTK_WINDOW(gui.topwindow));

  /* Set statusbar messages */
  #ifdef GTK2_ENABLED
    gtk_statusbar_set_has_resize_grip(GTK_STATUSBAR(gui.sbname),FALSE);
    gtk_statusbar_set_has_resize_grip(GTK_STATUSBAR(gui.sbnumber),FALSE);
  #endif

  gtk_statusbar_push
    (GTK_STATUSBAR(gui.sbname), 1, " Game selected: None");
  gtk_statusbar_push
    (GTK_STATUSBAR(gui.sbnumber), 1, " Games in list: 0");

  /* Set cellrenderertoogle (bug in gtk-win?) */
  GtkCellRenderer *celltoggle = gtk_cell_renderer_toggle_new();
  gtk_tree_view_column_pack_end((GTK_TREE_VIEW_COLUMN(
                                 gtk_builder_get_object(gui.settings,
                                 "Visible"))), celltoggle, TRUE);

  gtk_tree_view_column_add_attribute((GTK_TREE_VIEW_COLUMN(
                                      gtk_builder_get_object(gui.settings,
                                      "Visible"))),
                                      celltoggle, "active", 1);

  g_signal_connect(celltoggle, "toggled", G_CALLBACK(on_cell_toggled), &gui);

  /* Load preferences */
  key_file = load_prefs(&gui);

 /* Search for mednafen executable */
  if (gui.binpath==NULL)
  {
    gui.binpath = g_find_program_in_path("mednafen");
  }
  else
  {
    if (!g_file_test(gui.binpath, G_FILE_TEST_IS_EXECUTABLE))
    {
      g_free(gui.binpath);
      gui.binpath = NULL;
    }
  }
  if (gui.binpath==NULL)
  {
    gui.binpath = show_chooser(
      "Warning: Mednafen executable is not installed in path.\nDo you \
want to select the file manually?\n");
    if (gui.binpath==NULL)
    {
      show_error(
      "Error: Mednafen executable not found.\n");
      return 1;
    }
  }

  /* Search for HOME variable */
  #ifdef G_OS_WIN32
    home = g_path_get_dirname(gui.binpath);
  #else
    home = g_getenv ("HOME");
    if (!home) home = g_get_home_dir();
  #endif
    if (!home)
    {
      show_error("Error searching for home variable.\n");
      return 1;
    }

  #ifdef G_OS_WIN32
    gchar *path = g_strconcat(home, "\\stdout.txt", NULL);
    gchar *qbin = g_strconcat("\"", gui.binpath, "\"", NULL);
    gchar *cfg_path = g_strconcat(home, "\\mednafen-09x.cfg", NULL);

    if ((g_file_get_contents(path, &stout, NULL, NULL)) &&
        (g_file_test(cfg_path, G_FILE_TEST_IS_REGULAR)))
    {}
    else
    {
    system(qbin);
      //Sleep(1000); /* race condition? */
      g_file_get_contents(path, &stout, NULL, NULL);
    }
    g_free(cfg_path);
    g_free(qbin);
    g_free(path);

  #else
    g_spawn_command_line_sync(gui.binpath, &stout, NULL, NULL, NULL);
    //sleep (1); /* race condition? */
  #endif

  /* Search mednafen configuration file */
  gui.cfgfile = get_cfg(home, &gui);
  if (!gui.cfgfile)
  {
    show_error("Error: No mednafen configuration file found.\n");
    return 1;
  }

  /* Check mednafen version */
  if (!check_version(stout, &gui))
   {
    show_error(
    "Error: Mednafen version is not compatible.\nYou need 0.9.36.2 \
version or above.\n");
    return 1; /* Items are not freed here */
  }
  g_free(stout);

  /* Read configuration from mednafen-09x.cfg */
  if (!read_cfg(&gui))
  {
    show_error(
    "Error parsing mednafen configuration file.\n");
    return 1; /* Items are not freed here */
  }

  /* Set values into gui */
  if (key_file) load_emu_options(key_file, &gui);
  set_values(gui.builder, &gui);
  set_values(gui.specific, &gui);
  select_rows(&gui);

  /* Create icon */
  gtk_window_set_icon(GTK_WINDOW(gui.topwindow),
                    gdk_pixbuf_new_from_inline (-1, logo, FALSE, NULL));

  /* Show window and set focus */
  gtk_widget_show(gui.topwindow);
  gtk_widget_grab_focus(gui.gamelist);

  /* Start main loop */
  gtk_main();

  return 0;
}
