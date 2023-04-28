/*
 * mainwindow.c
 *
 * Copyright 2013-2023 AmatCoder
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
 *
 */


#include "mainwindow.h"

#include "about.h"
#include "medprocess.h"
#include "manager.h"
#include "logbook.h"
#include "pathcombobox.h"
#include "panedlist.h"

#include "widgets/medcolorentry.h"
#include "widgets/medrange.h"
#include "widgets/medlistjoy.h"
#include "widgets/medinput.h"

#include "widgets/dialogs.h"

#include "preferences.h"
#include "bios.h"

#ifdef G_OS_WIN32
  #include "win32util.h"
#endif


typedef struct _MainWindowClass MainWindowClass;
typedef struct _MainWindowPrivate MainWindowPrivate;

struct _MainWindowClass {
  GtkApplicationWindowClass parent_class;
};

struct _MainWindowPrivate {
  MedProcess* med_process;
  PathComboBox* pathbox;
  PanedList* panedlist;
  Logbook* logbook;
  PreferencesWindow* preferences;
  MedListJoy* listjoy;
  GtkCheckMenuItem* custom_menu;
  GtkEntry* custom_entry;
  GtkButton* launch_button;
  GtkLabel* status_num;
  GtkLabel* status_name;
  GtkLabel* status_version;
  GtkTreeModelFilter* systems_filter;
  GtkBox* main_box;
  GtkBox* log_box;
  GtkBox* emb_sys;
  GtkBuilder *sysui;
  GtkBox* emb_glob;
  GtkBuilder *globui;
  gboolean show_tooltips;
  gboolean sensitive;
  gboolean treeisfocus;

  const gchar* navup;
  const gchar* navdown;
  const gchar* navpageup;
  const gchar* navpagedown;
  const gchar* navfolderup;
  const gchar* navfolderdown;
  const gchar* navlaunch;

  GSList* recent_files;
};


G_DEFINE_TYPE_WITH_PRIVATE (MainWindow, main_window, GTK_TYPE_APPLICATION_WINDOW);


static void
main_window_path_combo_box_selected (PathComboBox* sender,
                                     GtkTreeIter* iter,
                                     gpointer self)
{
  g_return_if_fail (self != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  paned_list_fill_list (priv->panedlist, priv->pathbox->combo, iter);
}


static void
main_window_paned_list_mouse_released (PanedList* sender,
                                       gint vpos,
                                       gint hpos,
                                       gpointer self)
{
  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  path_combo_box_save_panel_position (priv->pathbox, vpos, hpos);
}


static void
main_window_paned_list_filled (PanedList* sender,
                               gint n_items,
                               gpointer self)
{
  gchar* s_items;
  gchar* str;

  g_return_if_fail (self != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  s_items = g_strdup_printf ("%i", n_items);
  str = g_strconcat ("Games in list: ", s_items, NULL);

  gtk_label_set_label (priv->status_num, str);

  g_free (s_items);
  g_free (str);

  gtk_label_set_label (priv->status_name, "Game selected:");

}


static void
main_window_paned_list_item_selected (PanedList* sender,
                                      const gchar* item,
                                      gpointer self)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (item != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  gchar* str = g_strconcat ("Game selected: ", item, NULL);
  gtk_label_set_label (priv->status_name, str);
  g_free (str);
}


static GSList*
get_all_medwidgets (GtkWidget* wid)
{
  g_return_val_if_fail (wid != NULL, NULL);

  GList* children = gtk_container_get_children ((GtkContainer*) wid);
  GSList* list = NULL;
  GList* it = NULL;
  for (it = children; it != NULL; it = it->next)
  {
    if (IS_MED_WIDGET(it->data))
    {
      list = g_slist_append (list, it->data);
    }
    else if (GTK_IS_CONTAINER(it->data))
      list = g_slist_concat(list, get_all_medwidgets ((GtkWidget*) it->data));
  }
  g_list_free (children);
  return list;
}


static void
main_window_set_all_values (MainWindow* self, GtkWidget* topwidget)
{
  g_return_if_fail (self != NULL);
  MainWindowPrivate* priv = main_window_get_instance_private (self);

  GSList *visible_list = get_all_medwidgets (topwidget);

  GSList* it = NULL;
  for(it = visible_list; it != NULL; it = it->next)
  {
    const gchar* command;
    const gchar* tmp = NULL;

    command = med_widget_get_command (it->data);

    if (command)
      tmp = g_hash_table_lookup (priv->med_process->table, command);

    if (tmp)
      med_widget_set_value (it->data, tmp);
  }
  g_slist_free (visible_list);
}


static void
main_window_set_all_sensitive (MainWindow* self, gboolean b, gboolean c)
{
  g_return_if_fail (self != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  GSList *visible_list = get_all_medwidgets ((GtkWidget*)self);

  GSList* it = NULL;
  for(it = visible_list; it != NULL; it = it->next)
  {
    if (!med_widget_get_updated (it->data))
      gtk_widget_set_sensitive ((GtkWidget*) it->data, b);

    gtk_widget_set_has_tooltip ((GtkWidget*) it->data, c);
  }

  g_slist_free (visible_list);

  gtk_widget_set_sensitive (((GtkWidget*) priv->launch_button), b);

  priv->sensitive = b;
  priv->show_tooltips = c;
}


static gchar**
main_window_build_command (MainWindow* self, const gchar* game_path)
{
  g_return_val_if_fail (self != NULL, NULL);
  MainWindowPrivate* priv = main_window_get_instance_private (self);

  gchar** command;
  command = g_new0 (gchar*, 3);
  command[0] = g_strdup (priv->med_process->MedExePath);

  GHashTableIter iter;
  gpointer key, value;
  gint i = 1;

  g_hash_table_iter_init (&iter, priv->med_process->table);
  while (g_hash_table_iter_next (&iter, &key, &value))
  {
    gchar* k = (gchar*)key;
    if (k[0] == '-')
    {
      command = g_renew (gchar*, command, i+2+2);

      command[i] = g_strdup (key);
      i++;
#ifdef G_OS_WIN32
      command[i] = g_strconcat ("\"", value, "\"", NULL);
#else
      command[i] = g_strdup (value);
#endif
      i++;
    }
  }

  gchar* custom = g_strdup (gtk_entry_get_text (priv->custom_entry));
  g_strstrip (custom);

  if ( (gtk_widget_is_visible ((GtkWidget*)priv->custom_entry)) && (g_strcmp0 (custom, "") != 0) )
  {
    gchar** pch = g_strsplit (custom, " ", 0);
    guint n = g_strv_length (pch);
    command = g_renew (gchar*, command, i+n+2);
    gint j = 0;
    while (pch[j])
    {
      command[i] = g_strdup (pch[j]);
      i++;
      j++;
    }
    g_strfreev (pch);
  }

  g_free (custom);

  command[i] = g_strdup (game_path);
  i++;
  command[i] = NULL;

  return command;
}


static void
main_window_paned_list_launched (PanedList* _sender,
                                 const gchar* selection,
                                 gpointer self)
{
  g_return_if_fail (self != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  if ((selection != NULL) && (priv->sensitive))
  {
    main_window_set_all_sensitive (mw, FALSE, priv->show_tooltips);

    const gchar *action = med_widget_get_value ((MedWidget*) priv->preferences->action_launch);

    if (g_strcmp0 (action, "minimize") == 0)
      gtk_window_iconify ((GtkWindow*) self);
    else if (g_strcmp0 (action, "hide") == 0)
        gtk_widget_hide ((GtkWidget*) self);

    logbook_delete_log (priv->logbook, LOGBOOK_LOG_TAB_EMU);

    GSList *elem = g_slist_find_custom (priv->recent_files , selection, (GCompareFunc) g_strcmp0);
    if (elem != NULL)
    {
      g_free (elem->data);
      priv->recent_files = g_slist_remove (priv->recent_files, elem->data);
    }

    priv->recent_files = g_slist_prepend (priv->recent_files, g_strdup(selection));

    if (g_slist_length (priv->recent_files) > 20)
    {
      elem = g_slist_last (priv->recent_files);
      g_free (elem->data);
      priv->recent_files = g_slist_remove (priv->recent_files, elem->data);
    }

#ifdef G_OS_WIN32
    gchar* selection2 = g_strconcat("\"", selection, "\"", NULL);
    gchar** command = main_window_build_command (mw, selection2);
#else
    gchar** command = main_window_build_command (mw, selection);
#endif

    med_process_exec_emu (priv->med_process, command);

    gchar* tmp = g_strjoinv (" ", command);
    g_strfreev (command);

    gchar* f_command = g_strconcat ("Launching mednafen with command line:\n  \"", tmp, "\"\n", NULL);
    g_free (tmp);

    logbook_write_log (priv->logbook, LOGBOOK_LOG_TAB_FRONTEND, f_command);
    g_free (f_command);

#ifdef G_OS_WIN32
    g_free (selection2);
#endif
  }
}


static void
main_window_process_write_emu_log (MedProcess* sender,
                                   const gchar* line,
                                   const gchar* stream_name,
                                   gpointer self)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (line != NULL);
  //g_return_if_fail (stream_name != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  logbook_write_log (priv->logbook, LOGBOOK_LOG_TAB_EMU, line);
}


static void
main_window_show_error (MainWindow* self,
                        const gchar* msg)
{
  GtkWidget* dialog;

  g_return_if_fail (self != NULL);
  g_return_if_fail (msg != NULL);

  MainWindowPrivate* priv = main_window_get_instance_private (self);

  dialog = gtk_message_dialog_new ((GtkWindow*) self, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", msg);

  logbook_write_log (priv->logbook, LOGBOOK_LOG_TAB_FRONTEND, msg);

  gtk_dialog_run ((GtkDialog*) dialog);
  gtk_widget_destroy (dialog);
}


static void
main_window_process_exec_emu_ended (MedProcess* sender,
                                    gint status,
                                    gpointer self)
{
  g_return_if_fail (self != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

#ifdef G_OS_WIN32
  gchar* string = NULL;
  gchar* wpath = win32_get_process_directory ();
  gchar* txt = g_strconcat (wpath, "\\stdout.txt", NULL);

  if (g_file_get_contents(txt, &string, NULL, NULL))
  {
    logbook_write_log (priv->logbook, LOGBOOK_LOG_TAB_EMU, string);
    g_free (string);
  }

  g_free (txt);
  g_free (wpath);
#else
  if (status != 0)
  {
    gchar* last_line;
    gchar* err_msg;

    last_line = logbook_get_last_line (priv->logbook);
    err_msg = g_strconcat ("Mednafen error:\n  ", last_line, NULL);
    main_window_show_error (mw, err_msg);

    g_free (err_msg);
    g_free (last_line);
  }
#endif

  const gchar* action = med_widget_get_value ((MedWidget*) priv->preferences->action_launch);

  if (g_strcmp0 (action, "minimize") == 0)
    gtk_window_deiconify ((GtkWindow*) mw);
  else if (g_strcmp0 (action, "hide") == 0)
    gtk_widget_show ((GtkWidget*) mw);

  main_window_set_all_sensitive (mw, TRUE, priv->show_tooltips);

  med_process_read_conf (priv->med_process);
  main_window_set_all_values (mw, (GtkWidget*)mw);

  logbook_write_log (priv->logbook, LOGBOOK_LOG_TAB_FRONTEND, "End of execution caught\n");
}


static void
main_window_preferences_show_tooltips (PreferencesWindow* sender,
                                       gboolean b,
                                       gpointer self)
{
  g_return_if_fail (self != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  main_window_set_all_sensitive (mw, priv->sensitive, b);

  priv->show_tooltips = b;
}


static void
main_window_preferences_show_filters (PreferencesWindow* sender,
                                      gboolean b,
                                      gpointer self)
{
  g_return_if_fail (self != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  paned_list_show_filters (priv->panedlist, b);
}


static void
main_window_preferences_change_theme (PreferencesWindow* sender, gint t, gpointer self)
{
#ifdef STATIC_BUILD

  GtkSettings* settings = gtk_settings_get_default();
  char* theme;
  gboolean dark;

  switch (t)
  {
    case 1:
      theme = "win32";
      dark = FALSE;
      break;

    case 2:
      theme = "Adwaita";
      dark = FALSE;
      break;

    case 3:
      theme = "Windows10";
      dark = TRUE;
      break;

    case 4:
      theme = "Adwaita";
      dark = TRUE;
      break;

    default:
      theme = "Windows10";
      dark = FALSE;
  }

  g_object_set (settings, "gtk-theme-name", theme, "gtk-application-prefer-dark-theme", dark, NULL);

#endif
}


static void
main_window_preferences_show_systems (PreferencesWindow* sender,
                                      gint i,
                                      gboolean b,
                                      gpointer self)
{
  gint j;
  GtkTreeIter iter;
  GtkTreeModel* model;

  g_return_if_fail (self != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  model = gtk_tree_model_filter_get_model (priv->systems_filter);

  gtk_tree_model_get_iter_first (model, &iter);

  while (TRUE)
  {
    gtk_tree_model_get (model, &iter, 2, &j, -1);

    if (j == i)
    {
      gtk_list_store_set ((GtkListStore*) model, &iter, 3, b, -1);
      break;
    }

    if (!gtk_tree_model_iter_next (model, &iter))
      break;
  }
}


static void
main_window_joy_found (MedListJoy* _sender,
                       const gchar* name,
                       const gchar* id,
                       gpointer self)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (name != NULL);
  g_return_if_fail (id != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  gchar* msg = g_strconcat ("Joystick found: ID: ", id, " - ", name, "\n", NULL);

  logbook_write_log (priv->logbook, LOGBOOK_LOG_TAB_FRONTEND, msg);
  g_free (msg);
}


static void
main_window_selection_changed (GtkTreeSelection* sender, gpointer self)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (sender != NULL);

  GtkTreeModel* model;
  GtkTreeIter iter;
  gboolean valid = gtk_tree_selection_get_selected (sender, &model, &iter);

  if (valid)
  {
    MainWindow* mw = self;
    MainWindowPrivate* priv = main_window_get_instance_private (mw);

    gchar* item;
    gchar* uipath;
    GtkBox* emb;
    GtkWidget* old = NULL;

    gtk_tree_model_get (model, &iter, 1, &item, -1);
    uipath = g_strconcat ("/com/github/mednaffe/", item, ".ui", NULL);
    GtkBuilder *ui = gtk_builder_new_from_resource (uipath);
    gtk_builder_connect_signals (ui, NULL);

    GtkWidget* new = (GtkWidget*)gtk_builder_get_object (ui, "globbox");

    if (!new)
    {
      new = (GtkWidget*)gtk_builder_get_object (ui, "sysbox");
      emb = priv->emb_sys;

      if (priv->sysui)
      {
        old = (GtkWidget*)gtk_builder_get_object (priv->sysui, "sysbox");
        g_object_unref (priv->sysui);
      }

      priv->sysui = ui;
    }
    else
    {
      emb = priv->emb_glob;

      if (priv->globui)
      {
        old = (GtkWidget*)gtk_builder_get_object (priv->globui, "globbox");
        g_object_unref (priv->globui);
      }

      priv->globui = ui;
    }

    if (old)
      gtk_widget_destroy (old);

    if (new)
      gtk_box_pack_start (emb, new, TRUE, TRUE, 0);

    g_free (uipath);
    g_free (item);

   main_window_set_all_sensitive (mw, priv->sensitive, priv->show_tooltips);
  }
}


static void
main_window_menu_hide_screens (GtkCheckMenuItem *checkmenuitem, gpointer self)
{
  g_return_if_fail (self != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  paned_list_show_screens (priv->panedlist, gtk_check_menu_item_get_active (checkmenuitem));
}


static void
main_window_menu_show_bios (GtkMenuItem* sender,
                            gpointer self)
{
  g_return_if_fail (self != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  BiosWindow* bios_window = bios_window_new ((GtkWindow*) self, priv->med_process->table);
  gtk_widget_show ((GtkWidget*) bios_window);
}


static void
main_window_menu_show_about (GtkMenuItem* sender,
                             gpointer self)
{
  g_return_if_fail (self != NULL);

  AboutWindow* about_window = about_window_new ((GtkWindow*) self);
  gtk_widget_show ((GtkWidget*) about_window);
}


static void
main_window_menu_quit (GtkMenuItem* sender,
                       gpointer self)
{
  g_return_if_fail (self != NULL);

  MainWindow* mw = self;

  gtk_window_close ((GtkWindow*) mw);
}


static void
save_mods (GKeyFile *key, GHashTable* tab)
{
  GHashTableIter iter;
  gpointer command, value;

  g_hash_table_iter_init (&iter, tab);
  while (g_hash_table_iter_next (&iter, &command, &value))
  {
    gchar* com = (gchar*)command;
    if (com[0] == '-')
      g_key_file_set_string(key, "EMU", (gchar*)command, (gchar*)value);
  }
}


static void
save_preflist_func (gconstpointer data, gpointer self)
{
  const gchar* command = med_widget_get_command ((MedWidget*)data);
  const gchar* value = med_widget_get_value ((MedWidget*)data);
  g_key_file_set_string ((GKeyFile*) self, "GUI", command, value);
}


static void
main_window_save_settings (MainWindow* self)
{
  gint width;
  gint height;

  g_return_if_fail (self != NULL);
  MainWindowPrivate* priv = main_window_get_instance_private (self);

  GKeyFile *key = g_key_file_new ();

  GObject *app = (GObject*) gtk_window_get_application ((GtkWindow*)self);

  gchar* comment = g_strconcat ("Version ", g_object_get_data (app, "version"), "\nDo not modify this file!", NULL);
  g_key_file_set_comment (key, NULL, NULL, comment, NULL);
  g_free (comment);

  gsize size;
  gchar** dirs = path_combo_box_get_dirs (priv->pathbox, &size);
  g_key_file_set_string_list (key, "GUI", "Dirs", (const gchar* const*) dirs, size);
  g_strfreev (dirs);

  g_key_file_set_integer (key, "GUI", "ActiveDir", gtk_combo_box_get_active (priv->pathbox->combo));

  gtk_window_get_size ((GtkWindow*) self, &width, &height);
  g_key_file_set_integer (key, "GUI", "Width", width);
  g_key_file_set_integer (key, "GUI", "Height", height);
  g_key_file_set_boolean (key, "GUI", "Maximized", gtk_window_is_maximized ((GtkWindow*) self));

  gtk_window_get_position ((GtkWindow*) self, &width, &height);
  g_key_file_set_integer (key, "GUI", "PosX", width);
  g_key_file_set_integer (key, "GUI", "PosY", height);

  g_key_file_set_boolean (key, "GUI", "AdditionalCommandActivated", gtk_widget_is_visible ((GtkWidget*)priv->custom_entry));
  g_key_file_set_string (key, "GUI", "AdditionalCommand", gtk_entry_get_text (priv->custom_entry));

  GSList* it = NULL;
  int i = 0;
  for(it = priv->recent_files; it != NULL; it = it->next)
  {
    i++;
    gchar* recent_key = g_strdup_printf ("Recent%i", i);
    g_key_file_set_string (key, "GUI", recent_key, (gchar*)it->data);
    g_free (recent_key);
  }

  g_slist_foreach (priv->preferences->list, (GFunc) save_preflist_func, key);
  save_mods (key, priv->med_process->table);

#ifdef G_OS_WIN32
  gint theme = gtk_combo_box_get_active (GTK_COMBO_BOX(priv->preferences->change_theme));
  g_key_file_set_integer (key, "GUI", "Theme", theme);

  GtkAdjustment *adj = gtk_spin_button_get_adjustment (priv->preferences->change_font);
  gint fsize = (gint)gtk_adjustment_get_value (adj);
  g_key_file_set_integer (key, "GUI", "FontSize", fsize);

  gchar* conf_path = win32_get_process_directory ();
#else
  gchar* conf_path = g_strconcat (g_get_user_config_dir (), "/mednaffe", NULL);

  if (!g_file_test (conf_path, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR))
  {
    if (g_mkdir_with_parents (conf_path, 0700) == -1)
    {
      main_window_show_error (self, "Configuration directory can not be created!\n");
      g_free (conf_path);
      g_key_file_free (key);
      return;
    }
  }
#endif

  gchar* conf_path_full = g_strconcat (conf_path, G_DIR_SEPARATOR_S, "mednaffe.conf", NULL);
  g_free (conf_path);

  GError *err = NULL;
  gboolean valid = g_key_file_save_to_file (key, conf_path_full, &err);
  g_free (conf_path_full);

  if (valid)
  {
    gchar* bye = g_strconcat ("Exiting ", g_object_get_data (app, "name"), "\n", NULL);
    logbook_write_log (priv->logbook, LOGBOOK_LOG_TAB_FRONTEND, bye);
    g_free (bye);
  }
  else if (err != NULL)
  {
    main_window_show_error (self, err->message);
    g_error_free (err);
  }

  g_key_file_free (key);
}


static gboolean
main_window_quit (GtkWidget* sender,
                  GdkEventAny* event,
                  gpointer self)
{
  g_return_val_if_fail (self != NULL, FALSE);

  MainWindow* mw = self;

  main_window_save_settings (mw);
  return FALSE;
}


static void
main_window_menu_show_manager (GtkMenuItem* sender,
                               gpointer self)
{
  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  ManagerWindow* manager = manager_window_new ((GtkWindow*) self, priv->pathbox->combo);
  gtk_widget_show ((GtkWidget*) manager);
}


static void
main_window_launch_clicked (GtkButton* sender,
                            gpointer self)
{
  g_return_if_fail (self != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  paned_list_request_launch (priv->panedlist);
}


static void
submenu_activate_recent (GtkMenuItem* sender,
                         gpointer self)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (sender != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  const gchar* filename = gtk_menu_item_get_label (sender);

  if (g_strcmp0 (filename, "Clear History") == 0)
  {
    g_slist_free_full (priv->recent_files, g_free);
    priv->recent_files = NULL;
  }
  else
    main_window_paned_list_launched (NULL, filename, mw);
}


static void
main_window_menu_activate_recent (GtkMenuItem* sender,
                                  gpointer self)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (sender != NULL);
  
  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  gtk_menu_item_set_submenu (sender, NULL);
  GtkWidget* menu = gtk_menu_new();
  gtk_menu_item_set_submenu (sender, menu);

  GtkWidget *submenu;
  GSList* it = NULL;
  for(it = priv->recent_files; it != NULL; it = it->next)
  {
    submenu = gtk_menu_item_new_with_label (it->data);
    gtk_menu_shell_append (GTK_MENU_SHELL(menu), submenu);
    g_signal_connect_object (submenu, "activate", (GCallback) submenu_activate_recent, self, 0);
  }

  submenu = gtk_separator_menu_item_new();
  gtk_menu_shell_append (GTK_MENU_SHELL(menu), submenu);

  submenu = gtk_menu_item_new_with_label ("Clear History");
  gtk_menu_shell_append (GTK_MENU_SHELL(menu), submenu);
  g_signal_connect_object (submenu, "activate", (GCallback) submenu_activate_recent, self, 0);
  gtk_widget_set_sensitive (submenu, (priv->recent_files != NULL) ? TRUE : FALSE);

  gtk_widget_show_all (menu);
}


static void
main_window_menu_open_rom (GtkMenuItem* sender,
                           gpointer self)
{
  g_return_if_fail (self != NULL);

  MainWindow* mw = self;

  gchar* filename = select_path ((GtkWidget*) mw, FALSE);

  if (filename)
  {
    main_window_paned_list_launched (NULL, filename, mw);
    g_free (filename);
  }
}


static void
main_window_menu_show_preferences (GtkMenuItem* sender,
                                   gpointer self)
{
  g_return_if_fail (self != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  gtk_window_present ((GtkWindow*) priv->preferences);
}


static void
load_mods (GKeyFile* key, GHashTable* tab)
{
  gchar** commands = g_key_file_get_keys (key, "EMU", NULL, NULL);
  gint i = 0;

  while (commands[i])
  {
    gchar* value = g_key_file_get_string (key, "EMU", commands[i], NULL);
    g_hash_table_insert (tab, commands[i], value);
    i++;
  }

  g_free (commands);
}


static void
load_preflist_func (gconstpointer data, gpointer self)
{
  const gchar* command = med_widget_get_command ((MedWidget*)data);

  gchar* value = g_key_file_get_string((GKeyFile*)self, "GUI", command, NULL);
  if (value)
    med_widget_set_value ((MedWidget*)data, value);

  g_free(value);
}


static void
main_window_load_settings (MainWindow* self)
{
  MainWindowPrivate* priv = main_window_get_instance_private (self);

  gint width = 0;
  gint height = 0;

  g_return_if_fail (self != NULL);

  GKeyFile *key = g_key_file_new ();

#ifdef G_OS_WIN32
  gchar* dir = win32_get_process_directory ();
  gchar* conf_path = g_strconcat (dir, "\\mednaffe.conf", NULL);
  g_free(dir);
#else
  gchar* conf_path = g_strconcat (g_get_user_config_dir (), "/mednaffe/mednaffe.conf", NULL);
#endif
  gboolean valid = g_key_file_load_from_file (key, conf_path, G_KEY_FILE_NONE, NULL);
  g_free (conf_path);

  if (!valid)
  {
    gtk_widget_show ((GtkWidget*) self);
    return;
  }

  g_slist_foreach (priv->preferences->list, (GFunc)load_preflist_func, key);

  if (g_key_file_has_group (key, "EMU"))
    load_mods(key, priv->med_process->table);

  if (gtk_toggle_button_get_active ((GtkToggleButton*) priv->preferences->win_size))
  {
    width = g_key_file_get_integer (key, "GUI", "Width", NULL);
    height = g_key_file_get_integer (key, "GUI", "Height", NULL);
  }

  if ((width > 0) && ( height > 0))
    gtk_window_resize ((GtkWindow*) self, width, height);

  if (g_key_file_get_boolean (key, "GUI", "Maximized", NULL))
    gtk_window_maximize ((GtkWindow*) self);

  if (gtk_toggle_button_get_active ((GtkToggleButton*) priv->preferences->win_pos))
  {
    width = g_key_file_get_integer (key, "GUI", "PosX", NULL);
    height = g_key_file_get_integer (key, "GUI", "PosY", NULL);

    gtk_window_move ((GtkWindow*) self, width, height);
  }

  gtk_check_menu_item_set_active (priv->custom_menu, g_key_file_get_boolean (key, "GUI", "AdditionalCommandActivated", NULL));
  gchar* cc = g_key_file_get_string (key, "GUI", "AdditionalCommand", NULL);

  if (cc)
    gtk_entry_set_text (priv->custom_entry, cc);

  g_free (cc);

  gtk_widget_show ((GtkWidget*) self);

#ifdef G_OS_WIN32

  gint t = g_key_file_get_integer (key, "GUI", "Theme", NULL);
  gtk_combo_box_set_active (GTK_COMBO_BOX(priv->preferences->change_theme), t);

  gint s = g_key_file_get_integer (key, "GUI", "FontSize", NULL);

  if (s > 0)
    gtk_spin_button_set_value (priv->preferences->change_font, (gdouble)s);
  else
    gtk_spin_button_set_value (priv->preferences->change_font, 13.0);

#endif

  gsize size;
  gchar** dirs = g_key_file_get_string_list (key, "GUI", "Dirs", &size, NULL);
  path_combo_box_set_dirs (priv->pathbox, dirs, size);

  gint active = g_key_file_get_integer (key, "GUI", "ActiveDir", NULL);
  gtk_combo_box_set_active (priv->pathbox->combo, active);

  g_strfreev(dirs);

  for (int i = 1; i < 21; i++)
  {
    gchar* recent_key = g_strdup_printf ("Recent%i", i);
    gchar* recent_value = g_key_file_get_string (key, "GUI", recent_key, NULL);
    g_free (recent_key);

    if (recent_value != NULL)
      priv->recent_files = g_slist_append (priv->recent_files, recent_value);
    else
      break;
  }

  priv->navup = g_key_file_get_string (key, "GUI", "MoveUp", NULL);
  priv->navdown = g_key_file_get_string (key, "GUI", "MoveDown", NULL);
  priv->navpageup = g_key_file_get_string (key, "GUI", "MovePageUp", NULL);
  priv->navpagedown = g_key_file_get_string (key, "GUI", "MovePageDown", NULL);
  priv->navfolderup = g_key_file_get_string (key, "GUI", "FolderUp", NULL);
  priv->navfolderdown = g_key_file_get_string (key, "GUI", "FolderDown", NULL);
  priv->navlaunch = g_key_file_get_string (key, "GUI", "LaunchGame", NULL);

  g_key_file_free (key);
}


static void
main_window_joy_event (MedListJoy* sender,
                       const gchar* text,
                       const gchar* value,
                       gpointer self)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (text != NULL);
  g_return_if_fail (value != NULL);

  MainWindowPrivate* priv = main_window_get_instance_private (self);

  if ((!priv->treeisfocus) || (!priv->sensitive))
    return;

  if (g_strcmp0 (value, priv->navdown) == 0)
    paned_list_selection_nav (priv->panedlist, 1, GTK_MOVEMENT_DISPLAY_LINES);
  else if (g_strcmp0 (value, priv->navup) == 0)
    paned_list_selection_nav (priv->panedlist, -1, GTK_MOVEMENT_DISPLAY_LINES);
  else if (g_strcmp0 (value, priv->navpagedown) == 0)
    paned_list_selection_nav (priv->panedlist, 1, GTK_MOVEMENT_PAGES);
  else if (g_strcmp0 (value, priv->navpageup) == 0)
    paned_list_selection_nav (priv->panedlist, -1, GTK_MOVEMENT_PAGES);
  else if (g_strcmp0 (value, priv->navfolderdown) == 0)
    path_combo_box_move_to_item (priv->pathbox, TRUE);
  else if (g_strcmp0 (value, priv->navfolderup) == 0)
    path_combo_box_move_to_item (priv->pathbox, FALSE);
  else if (g_strcmp0 (value, priv->navlaunch) == 0)
    paned_list_request_launch (priv->panedlist);
}


static void
main_window_focus_changed (GtkWidget* sender,
                           gboolean in,
                           gpointer self)
{
  g_return_if_fail (self != NULL);
  MainWindowPrivate* priv = main_window_get_instance_private (self);

  med_list_joy_enable_all (priv->listjoy, in);
  priv->treeisfocus = in;
}


static void
main_window_preferences_hidden (GtkWidget* window, gpointer self)
{
  g_return_if_fail (self != NULL);
  MainWindowPrivate* priv = main_window_get_instance_private (self);

  GSList* it = NULL;
  for(it = priv->preferences->list; it != NULL; it = it->next)
  {
    const gchar* command = med_widget_get_command ((MedWidget*)it->data);

    if (g_strcmp0 (command, "MoveDown") == 0)
      priv->navdown = med_widget_get_value ((MedWidget*)it->data);
    else if (g_strcmp0 (command, "MoveUp") == 0)
      priv->navup = med_widget_get_value ((MedWidget*)it->data);
    else if (g_strcmp0 (command, "MovePageDown") == 0)
      priv->navpagedown = med_widget_get_value ((MedWidget*)it->data);
    else if (g_strcmp0 (command, "MovePageUp") == 0)
      priv->navpageup = med_widget_get_value ((MedWidget*)it->data);
    else if (g_strcmp0 (command, "FolderDown") == 0)
      priv->navfolderdown = med_widget_get_value ((MedWidget*)it->data);
    else if (g_strcmp0 (command, "FolderUp") == 0)
      priv->navfolderup = med_widget_get_value ((MedWidget*)it->data);
    else if (g_strcmp0 (command, "LaunchGame") == 0)
      priv->navlaunch = med_widget_get_value ((MedWidget*)it->data);
  }
}


void
main_window_start (MainWindow* self)
{
  g_return_if_fail (self != NULL);
  MainWindowPrivate* priv = main_window_get_instance_private (self);

  GObject *app = (GObject*) gtk_window_get_application ((GtkWindow*)self);

  gtk_window_set_title ((GtkWindow*) self, g_object_get_data (app, "name"));
  gchar* welcome = g_strconcat ("Starting ",
                                g_object_get_data (app, "name"),
                                " ",
                                g_object_get_data (app, "version"),
                                "\n",
                                NULL);


  logbook_write_log (priv->logbook, LOGBOOK_LOG_TAB_FRONTEND, welcome);
  g_free (welcome);


  priv->med_process = med_process_new ();

  if (priv->med_process->MedExePath == NULL)
  {
#ifdef G_OS_WIN32
    main_window_show_error (self, "Mednafen executable not found in this folder!\n");
#else
    main_window_show_error (self, "Mednafen executable not found in PATH!\n");
#endif
    main_window_set_all_sensitive (self, FALSE, priv->show_tooltips);
  }
  else
  {
    med_process_read_conf (priv->med_process);

    if (priv->med_process->MedVersion == NULL)
      main_window_show_error (self, "Mednafen configuration file (mednafen.cfg) not found!\n");
    else
    {
      gtk_label_set_label (priv->status_version, priv->med_process->MedVersion);
      if (priv->med_process->MedExePath)
      {
        gchar *tooltip = g_strconcat ("Executable: ", priv->med_process->MedExePath, "\nConfiguration: ", priv->med_process->MedConfPath, NULL);
        gtk_widget_set_tooltip_text ((GtkWidget*) priv->status_version, tooltip);
        g_free (tooltip);
      }
    }
  }

  g_signal_connect_object (priv->med_process, "exec-output", (GCallback) main_window_process_write_emu_log, self, 0);
  g_signal_connect_object (priv->med_process, "exec-emu-ended", (GCallback) main_window_process_exec_emu_ended, self, 0);

  g_object_set_data ((GObject*) self, "table", priv->med_process->table);


  priv->listjoy = med_list_joy_new ();
  g_signal_connect_object (priv->listjoy , "joy-found", (GCallback) main_window_joy_found, self, 0);
  med_list_joy_init_list_joy (priv->listjoy);

  g_object_set_data ((GObject*) self, "listjoy", priv->listjoy);
  g_object_set_data ((GObject*) priv->preferences, "listjoy", priv->listjoy);

  g_signal_connect_object (priv->listjoy, "joy-event", (GCallback) main_window_joy_event, self, 0);
  g_signal_connect_object (priv->panedlist, "focus-changed", (GCallback) main_window_focus_changed, self, 0);
  g_signal_connect_object (priv->preferences, "hide", (GCallback) main_window_preferences_hidden, self, 0);

  gtk_window_set_default_icon_list (g_object_get_data (app, "icon_list"));

#ifdef G_OS_WIN32
  gtk_entry_set_icon_from_icon_name (priv->custom_entry, GTK_ENTRY_ICON_SECONDARY, "gtk-clear");
#endif

  main_window_load_settings (self);
}


static void
main_window_finalize (GObject* obj)
{
  MainWindow * self = G_TYPE_CHECK_INSTANCE_CAST (obj, main_window_get_type(), MainWindow);
  MainWindowPrivate* priv = main_window_get_instance_private (self);

  g_slist_free_full (priv->recent_files, g_free);

  g_object_unref (priv->med_process);
  g_object_unref (priv->listjoy);

  if (priv->sysui)
    g_object_unref (priv->sysui);
  if (priv->globui)
    g_object_unref (priv->globui);

  G_OBJECT_CLASS (main_window_parent_class)->finalize (obj);
}


MainWindow*
main_window_new (GtkApplication* app)
{
  g_return_val_if_fail (app != NULL, NULL);

  g_type_ensure (med_check_button_get_type ());
  g_type_ensure (med_combo_box_get_type ());
  g_type_ensure (med_range_get_type ());
  g_type_ensure (med_entry_get_type ());
  g_type_ensure (med_dialog_entry_get_type ());
  g_type_ensure (med_bios_entry_get_type ());
  g_type_ensure (med_color_entry_get_type ());
  g_type_ensure (med_input_get_type ());

  MainWindow* self = (MainWindow*) g_object_new (main_window_get_type(), "application", app, NULL);
  MainWindowPrivate* priv = main_window_get_instance_private (self);

  priv->logbook = logbook_new ();
  gtk_box_pack_start (priv->log_box, (GtkWidget*) priv->logbook, TRUE, TRUE, 0);

  priv->pathbox = path_combo_box_new();
  g_signal_connect_object (priv->pathbox, "selected", (GCallback) main_window_path_combo_box_selected, self, 0);
  gtk_box_pack_start (priv->main_box, (GtkWidget*) priv->pathbox, FALSE, FALSE, 0);

  priv->panedlist = paned_list_new ();
  g_signal_connect_object (priv->panedlist, "filled", (GCallback) main_window_paned_list_filled, self, 0);
  g_signal_connect_object (priv->panedlist, "mouse-released", (GCallback) main_window_paned_list_mouse_released, self, 0);
  g_signal_connect_object (priv->panedlist, "item-selected", (GCallback) main_window_paned_list_item_selected, self, 0);
  g_signal_connect_object (priv->panedlist, "launched", (GCallback) main_window_paned_list_launched, self, 0);
  gtk_box_pack_start (priv->main_box, (GtkWidget*) priv->panedlist, TRUE, TRUE, 0);

  GList *list = NULL;
  list = g_list_append (list, priv->panedlist);
  list = g_list_append (list, priv->pathbox);
  gtk_container_set_focus_chain ((GtkContainer*) priv->main_box, list);
  g_list_free (list);

  priv->preferences = preferences_window_new ((GtkWindow*) self);
  g_signal_connect_object (priv->preferences, "on-show-tips", (GCallback) main_window_preferences_show_tooltips, self, 0);
  g_signal_connect_object (priv->preferences, "on-show-filters", (GCallback) main_window_preferences_show_filters, self, 0);
  g_signal_connect_object (priv->preferences, "on-change-theme", (GCallback) main_window_preferences_change_theme, self, 0);

  gtk_tree_model_filter_set_visible_column (priv->systems_filter, 3);
  gtk_tree_model_filter_refilter (priv->systems_filter);
  g_signal_connect_object (priv->preferences, "on-show-systems", (GCallback) main_window_preferences_show_systems, self, 0);

  return self;
}


static void
main_window_init (MainWindow* self)
{
  MainWindowPrivate* priv = main_window_get_instance_private (self);

  priv->globui = NULL;
  priv->sysui = NULL;
  priv->sensitive = TRUE;
  priv->show_tooltips = TRUE;
  priv->treeisfocus = FALSE;
  priv->recent_files = NULL;

  gtk_widget_init_template (GTK_WIDGET (self));
}


static void
main_window_class_init (MainWindowClass* klass)
{
  G_OBJECT_CLASS (klass)->finalize = main_window_finalize;
  gint MainWindow_private_offset = g_type_class_get_instance_private_offset (klass);

  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (klass), "/com/github/mednaffe/MainWindow.ui");

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "main_box",
                                             FALSE,
                                             MainWindow_private_offset + G_STRUCT_OFFSET (MainWindowPrivate, main_box));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "log_box",
                                             FALSE,
                                             MainWindow_private_offset + G_STRUCT_OFFSET (MainWindowPrivate, log_box));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "status_num",
                                             FALSE,
                                             MainWindow_private_offset + G_STRUCT_OFFSET (MainWindowPrivate, status_num));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "status_name",
                                             FALSE,
                                             MainWindow_private_offset + G_STRUCT_OFFSET (MainWindowPrivate, status_name));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "status_version",
                                             FALSE,
                                             MainWindow_private_offset + G_STRUCT_OFFSET (MainWindowPrivate, status_version));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "emb_sys",
                                             FALSE,
                                             MainWindow_private_offset + G_STRUCT_OFFSET (MainWindowPrivate, emb_sys));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "emb_glob",
                                             FALSE,
                                             MainWindow_private_offset + G_STRUCT_OFFSET (MainWindowPrivate, emb_glob));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "systems_filter",
                                             FALSE,
                                             MainWindow_private_offset + G_STRUCT_OFFSET (MainWindowPrivate, systems_filter));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "launch_button",
                                             FALSE,
                                             MainWindow_private_offset + G_STRUCT_OFFSET (MainWindowPrivate, launch_button));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "custom_entry",
                                             FALSE,
                                             MainWindow_private_offset + G_STRUCT_OFFSET (MainWindowPrivate, custom_entry));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "custom_menu",
                                             FALSE,
                                             MainWindow_private_offset + G_STRUCT_OFFSET (MainWindowPrivate, custom_menu));


  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "selection_changed",
                                                G_CALLBACK (main_window_selection_changed));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "show_bios_dialog",
                                                G_CALLBACK (main_window_menu_show_bios));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "show_about",
                                                G_CALLBACK (main_window_menu_show_about));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "quit",
                                                G_CALLBACK (main_window_menu_quit));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "quit_x",
                                                G_CALLBACK (main_window_quit));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "show_manager",
                                                G_CALLBACK (main_window_menu_show_manager));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "hide_screens",
                                                G_CALLBACK (main_window_menu_hide_screens));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "launch_selected",
                                                G_CALLBACK (main_window_launch_clicked));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "open_rom",
                                                G_CALLBACK (main_window_menu_open_rom));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "activate_recent",
                                                G_CALLBACK (main_window_menu_activate_recent));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "show_preferences",
                                                G_CALLBACK (main_window_menu_show_preferences));
}
