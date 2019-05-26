/*
 * mainwindow.c
 *
 * Copyright 2013-2019 AmatCoder
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


#include <sys/stat.h>

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


typedef struct _MainWindowClass MainWindowClass;
typedef struct _MainWindowPrivate MainWindowPrivate;

struct _MainWindowClass {
  GtkApplicationWindowClass parent_class;
};

struct _MainWindowPrivate {
  GSList* list;
  GSList* map_list;
  MedProcess* med_process;
  AboutWindow* about_window;
  BiosWindow* bios_window;
  PathComboBox* pathbox;
  PanedList* panedlist;
  Logbook* logbook;
  ManagerWindow* manager;
  PreferencesWindow* preferences;
  MedListJoy* listjoy;
  GtkBox* main_box;
  GtkBox* log_box;
  GtkLabel* status_num;
  GtkLabel* status_name;
  GtkLabel* status_version;
  GtkNotebook* global_notebook;
  GtkNotebook* system_notebook;
  GtkTreeModelFilter* systems_filter;
  GtkButton* launch_button;
  GtkEntry* custom_entry;
  MedDialogEntry* path_firmware;
  MedBiosEntry* lynx_bios;
};


G_DEFINE_TYPE_WITH_PRIVATE (MainWindow, main_window, GTK_TYPE_APPLICATION_WINDOW);


static void
main_window_update_bios (MainWindow* self,
                         MedBiosEntry* entry)
{
  gchar* path;
  gchar* mh;
  const gchar* text;
  const gchar* firmware;

  g_return_if_fail (self != NULL);
  g_return_if_fail (entry != NULL);

  MainWindowPrivate* priv = main_window_get_instance_private (self);

  text = med_widget_get_value ((MedWidget*) entry);
  firmware= med_widget_get_value ((MedWidget*) priv->path_firmware);

#ifdef G_OS_WIN32
  mh = g_win32_get_package_installation_directory_of_module (NULL);
#else
  const gchar* home = g_getenv ("MEDNAFEN_HOME");

  if (home == NULL)
    mh = g_strconcat (g_get_home_dir (), "/.mednafen", NULL);
  else
    mh = g_strconcat (home, "/", NULL);
#endif

  if (!g_path_is_absolute (text))
  {
    if (!g_path_is_absolute (firmware))
    {
      path = g_strconcat (mh, G_DIR_SEPARATOR_S, firmware, G_DIR_SEPARATOR_S, text, NULL);

      if (!g_file_test (path, G_FILE_TEST_IS_REGULAR))
      {
        gchar* tmp;
        tmp = g_strconcat (mh, G_DIR_SEPARATOR_S, text, NULL);
        g_free (path);
        path = tmp;
      }

    }
    else path = g_strconcat (firmware, G_DIR_SEPARATOR_S, text, NULL);

  }
  else path = g_strdup (text);

  med_bios_entry_update_check (entry, path);

  g_free (mh);
  g_free (path);
}


static void
main_window_firmware_entry_emit_change (MedDialogEntry* sender,
                                        gpointer self)
{
  GSList* it = NULL;

  g_return_if_fail (self != NULL);
  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  for(it = priv->list; it != NULL; it = it->next)
  {
    if (IS_MED_BIOS_ENTRY (it->data))
      main_window_update_bios (self, (MedBiosEntry*) it->data);
  }
}


static void
main_window_path_combo_box_selected (PathComboBox* sender,
                                     GtkTreeIter* iter,
                                     gpointer self)
{
  g_return_if_fail (self != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  paned_list_fill_list (priv->panedlist, priv->pathbox->path_store, iter);
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


static void
main_window_medwid_update_all (MainWindow* self)
{
  g_return_if_fail (self != NULL);

  MainWindowPrivate* priv = main_window_get_instance_private (self);

  GSList* it = NULL;

  for(it = priv->list; it != NULL; it = it->next)
  {
    med_widget_set_modified ((MedWidget*) it->data, FALSE);
    med_widget_set_updated ((MedWidget*) it->data, FALSE);
    gtk_widget_set_sensitive ((GtkWidget*) it->data, FALSE);
  }
}


static const gchar**
main_window_build_command (MainWindow* self, const gchar* game_path)
{
  const gchar** command;
  gint i = 1;

  g_return_val_if_fail (self != NULL, NULL);
  MainWindowPrivate* priv = main_window_get_instance_private (self);

  command = g_new(const gchar*, 4);
  command[0] = priv->med_process->MedPath;

  GSList* it = NULL;

  for(it = priv->list; it != NULL; it = it->next)
  {
    if (med_widget_get_modified (it->data))
    {
      command = g_renew(const gchar*, command, i+5);

      command[i] = med_widget_get_command (it->data);
      i++;
      command[i] = med_widget_get_value (it->data);
      i++;
    }
  }

  //const gchar* custom = gtk_entry_get_text (priv->custom_entry);

  //if (g_strcmp0 (custom, "") != 0)
  //{
  //  command = g_renew(gchar*, command, i+1);
  //  command[i] = custom;
  //  i++;
  //}

  command[i] = game_path;
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

  if ((selection != NULL) || (gtk_widget_get_sensitive ((GtkWidget*) priv->launch_button)))
  {
    gtk_widget_set_sensitive (((GtkWidget*) priv->launch_button), FALSE);

    const gchar *action = med_widget_get_value ((MedWidget*) priv->preferences->action_launch);

    if (g_strcmp0 (action, "minimize") == 0)
      gtk_window_iconify ((GtkWindow*) self);
    else if (g_strcmp0 (action, "hide") == 0)
        gtk_widget_hide ((GtkWidget*) self);

    logbook_delete_log (priv->logbook, LOGBOOK_LOG_TAB_EMU);

#ifdef G_OS_WIN32
    gchar* selection2 = g_strconcat("\"", selection, "\"", NULL);
    const gchar** command = main_window_build_command (mw, selection2);
#else
    const gchar** command = main_window_build_command (mw, selection);
#endif

    med_process_exec_emu (priv->med_process, (gchar**) command);

    gchar* tmp = g_strjoinv (" ", (gchar**) command);
    g_free (command);

    gchar* f_command = g_strconcat ("Launching mednafen with command line:\n  \"", tmp, "\"\n", NULL);
    g_free (tmp);

    logbook_write_log (priv->logbook, LOGBOOK_LOG_TAB_FRONTEND, f_command);
    g_free (f_command);

#ifdef G_OS_WIN32
    g_free (selection2);
#endif

    main_window_medwid_update_all (mw);
  }
}


static void
main_window_preferences_show_tooltips (PreferencesWindow* sender,
                                       gboolean b,
                                       gpointer self)
{
  GSList* it = NULL;

  g_return_if_fail (self != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  for(it = priv->list; it != NULL; it = it->next)
  {
    gtk_widget_set_has_tooltip ((GtkWidget*) it->data, b);
  }
}


static void
main_window_preferences_show_filters (PreferencesWindow* sender,
                                      gboolean b,
                                      gpointer self)
{
  g_return_if_fail (self != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  priv->panedlist->filters_header = b;
  paned_list_show_filters (priv->panedlist, b);
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
    gtk_tree_model_get (model, &iter, 1, &j, -1);

    if (j == i)
    {
      gtk_list_store_set ((GtkListStore*) model, &iter, 2, b, -1);
      break;
    }

    if (!gtk_tree_model_iter_next (model, &iter))
      break;
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
main_window_medcombo_changed (MedComboBox* combo,
                              const gchar* value,
                              gpointer stack)
{
  gtk_stack_set_visible_child_name ((GtkStack*) stack, value);
}


static void
main_window_set_values (MainWindow* self)
{
  GSList* it = NULL;

  g_return_if_fail (self != NULL);
  MainWindowPrivate* priv = main_window_get_instance_private (self);

  for(it = priv->map_list; it != NULL; it = it->next)
  {
    const gchar* command;
    const gchar* tmp = NULL;

    command = med_widget_get_command (it->data);

    if (command)
    {
      command++;
      tmp = g_hash_table_lookup (priv->med_process->table, command);
    }

    if (tmp != NULL)
    {
      med_widget_set_value (it->data, tmp);
      med_widget_set_modified (it->data, FALSE);
      med_widget_set_updated (it->data, TRUE);
      gtk_widget_set_sensitive (((GtkWidget*) it->data), TRUE);
    }
  }
}


static void
main_window_process_exec_emu_ended (MedProcess* sender,
                                    gint status,
                                    gpointer self)
{
  g_return_if_fail (self != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  if (status != 0)
  {
    gchar* last_line;
    gchar* err_msg;

    last_line = logbook_get_last_line (priv->logbook);
    err_msg = g_strconcat ("Mednafen error:\n  ", last_line, NULL);
#ifdef G_OS_WIN32
  //TODO
#else
    main_window_show_error (mw, err_msg);
#endif

    g_free (err_msg);
    g_free (last_line);
  }

  const gchar* action = med_widget_get_value ((MedWidget*) priv->preferences->action_launch);

  if (g_strcmp0 (action, "minimize") == 0)
    gtk_window_deiconify ((GtkWindow*) mw);
  else if (g_strcmp0 (action, "hide") == 0)
    gtk_widget_show ((GtkWidget*) mw);

  gtk_widget_set_sensitive (((GtkWidget*) priv->launch_button), TRUE);

  med_process_read_conf (priv->med_process);
  main_window_set_values (mw);

  logbook_write_log (priv->logbook, LOGBOOK_LOG_TAB_FRONTEND, "End of execution catched\n");
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
main_window_bios_entry_emit_change (MedDialogEntry* sender,
                                    gpointer self)
{
  main_window_update_bios ((MainWindow*) self, (MedBiosEntry*) sender);
}


static void
main_window_medwid_map (GtkWidget* sender, gpointer self)
{
  MainWindowPrivate* priv = main_window_get_instance_private (self);
  const gchar* command;
  const gchar* tmp = NULL;

  command = med_widget_get_command ((MedWidget*) sender);

  if (command)
  {
    command++;
    tmp = g_hash_table_lookup (priv->med_process->table, command);
  }

  if (tmp != NULL)
  {
    priv->map_list = g_slist_prepend (priv->map_list, sender);

    if (!med_widget_get_updated ((MedWidget*) sender))
    {
      med_widget_set_value ((MedWidget*) sender, tmp);
      med_widget_set_modified ((MedWidget*) sender, FALSE);
      med_widget_set_updated ((MedWidget*) sender, TRUE);
      gtk_widget_set_sensitive (sender, TRUE);
    }
  }
  else
    gtk_widget_set_sensitive (sender, FALSE);
}


static void
main_window_medwid_unmap (GtkWidget* sender, gpointer self)
{
  MainWindowPrivate* priv = main_window_get_instance_private (self);

  priv->map_list = g_slist_remove (priv->map_list, sender);
}


static void
main_window_get_widgets (MainWindow* self,
                         GtkWidget* wid)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (wid != NULL);

  MainWindowPrivate* priv = main_window_get_instance_private (self);

  GList* children = gtk_container_get_children ((GtkContainer*) wid);
  GList* it = NULL;

  for (it = children; it != NULL; it = it->next)
  {
    if (IS_MED_WIDGET(it->data))
    {
      priv->list = g_slist_append (priv->list, it->data);

      g_signal_connect_object ((GtkWidget*) it->data, "map", (GCallback) main_window_medwid_map, self, 0);
      g_signal_connect_object ((GtkWidget*) it->data, "unmap", (GCallback) main_window_medwid_unmap, self, 0);

      if (IS_MED_BIOS_ENTRY(it->data))
        g_signal_connect_object ((MedDialogEntry*) it->data, "emit-change", (GCallback) main_window_bios_entry_emit_change, self, 0);

    }
    else if (GTK_IS_CONTAINER(it->data))
      main_window_get_widgets (self, (GtkWidget*) it->data);
  }
  g_list_free (children);
}


static void
main_window_global_selection_changed (GtkTreeSelection* sender,
                                      gpointer self)
{
  GtkTreeModel* model;
  GtkTreeIter iter;
  gboolean valid;

  g_return_if_fail (self != NULL);
  g_return_if_fail (sender != NULL);

  valid = gtk_tree_selection_get_selected (sender, &model, &iter);

  if (valid)
  {
    gint num;
    MainWindow* mw = self;
    MainWindowPrivate* priv = main_window_get_instance_private (mw);

    gtk_tree_model_get (model, &iter, 1, &num, -1);
    gtk_notebook_set_current_page (priv->global_notebook, num);
  }
}


static void
main_window_system_selection_changed (GtkTreeSelection* sender,
                                      gpointer self)
{
  GtkTreeModel* model;
  GtkTreeIter iter;
  gboolean valid;

  g_return_if_fail (self != NULL);
  g_return_if_fail (sender != NULL);

  valid = gtk_tree_selection_get_selected (sender, &model, &iter);

  if (valid)
  {
    gint num;
    MainWindow* mw = self;
    MainWindowPrivate* priv = main_window_get_instance_private (mw);

    gtk_tree_model_get (model, &iter, 1, &num, -1);
    gtk_notebook_set_current_page (priv->system_notebook, num);
  }
}


static void
main_window_menu_show_bios (GtkMenuItem* sender,
                            gpointer self)
{
  g_return_if_fail (self != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  main_window_firmware_entry_emit_change (NULL, mw);
  bios_window_update (priv->bios_window, priv->list);
  gtk_window_present ((GtkWindow*) priv->bios_window);
}


static void
main_window_menu_show_about (GtkMenuItem* sender,
                             gpointer self)
{
  g_return_if_fail (self != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  gtk_window_present ((GtkWindow*) priv->about_window);
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
save_list_func (gconstpointer data, gpointer self)
{
  if (med_widget_get_modified ((MedWidget*)data))
  {
    const gchar* command = med_widget_get_command ((MedWidget*)data);
    const gchar* value = med_widget_get_value ((MedWidget*)data);
    g_key_file_set_string((GKeyFile*)self, "EMU", command, value);
  }
}


static void
save_preflist_func (gconstpointer data, gpointer self)
{
  const gchar* command = med_widget_get_command ((MedWidget*)data);
  const gchar* value = med_widget_get_value ((MedWidget*)data);
  g_key_file_set_string((GKeyFile*)self, "GUI", command, value);
}


static void
main_window_save_settings (MainWindow* self)
{
  gint width = 0;
  gint height = 0;
  gboolean maximized;

  g_return_if_fail (self != NULL);
  MainWindowPrivate* priv = main_window_get_instance_private (self);

  GKeyFile *key = g_key_file_new ();

  gchar* comment = g_strconcat ("Version ", gtk_about_dialog_get_version ((GtkAboutDialog*) priv->about_window), "\nDo not modify this file!", NULL);
  g_key_file_set_comment (key, NULL, NULL, comment, NULL);
  g_free (comment);

  gint size;
  gchar** dirs = path_combo_box_get_dirs (priv->pathbox, &size);
  g_key_file_set_string_list (key, "GUI", "Dirs", (const gchar* const*) dirs, size);
  g_strfreev(dirs);

  g_key_file_set_integer (key, "GUI", "ActiveDir", gtk_combo_box_get_active (priv->pathbox->combo));

  g_object_get ((GtkWindow*) self, "is-maximized", &maximized, NULL);

  if (!maximized)
    gtk_window_get_size ((GtkWindow*) self, &width, &height);

  g_key_file_set_integer (key, "GUI", "Width", width);
  g_key_file_set_integer (key, "GUI", "Height", height);

  gtk_window_get_position ((GtkWindow*) self, &width, &height);
  g_key_file_set_integer (key, "GUI", "PosX", width);
  g_key_file_set_integer (key, "GUI", "PosY", height);

  g_slist_foreach (priv->preferences->list, (GFunc) save_preflist_func, key);
  g_slist_foreach (priv->list, (GFunc) save_list_func, key);

#ifdef G_OS_WIN32
  gchar* conf_path = g_win32_get_package_installation_directory_of_module (NULL);
#else
  gchar* conf_path = g_strconcat (g_get_user_config_dir (), "/mednaffe", NULL);
  mkdir (conf_path, S_IRWXU);
#endif

  gchar* conf_path_full = g_strconcat (conf_path, G_DIR_SEPARATOR_S, "mednaffe.conf", NULL);
  g_free (conf_path);

  gboolean valid = g_key_file_save_to_file (key, conf_path_full, NULL);
  g_free (conf_path_full);

  if (valid)
  {
    gchar* bye = g_strconcat ("Exiting ", gtk_about_dialog_get_program_name ((GtkAboutDialog*) priv->about_window), "\n", NULL);
    logbook_write_log (priv->logbook, LOGBOOK_LOG_TAB_FRONTEND, bye);
    g_free (bye);
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

  gtk_window_present ((GtkWindow*) priv->manager);
}


static void
main_window_launch_clicked (GtkButton* sender,
                            gpointer self)
{
  g_return_if_fail (self != NULL);

  MainWindow* mw = self;
  MainWindowPrivate* priv = main_window_get_instance_private (mw);

  main_window_paned_list_launched (NULL, priv->panedlist->selected, mw);
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
load_list_func (gconstpointer data, gpointer self)
{
  const gchar* command = med_widget_get_command ((MedWidget*)data);

  gchar* value = g_key_file_get_string((GKeyFile*)self, "EMU", command, NULL);
  if (value)
    med_widget_set_value ((MedWidget*)data, value);

  g_free(value);
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
  gchar* dir = g_win32_get_package_installation_directory_of_module (NULL);
  gchar* conf_path = g_strconcat (dir, "\\mednaffe.conf", NULL);
  g_free(dir);
#else
  gchar* conf_path = g_strconcat (g_get_user_config_dir (), "/mednaffe/mednaffe.conf", NULL);
#endif
  gboolean valid = g_key_file_load_from_file (key, conf_path, G_KEY_FILE_NONE, NULL);
  g_free (conf_path);

  if (!valid)
    return;

  g_slist_foreach (priv->preferences->list, (GFunc)load_preflist_func, key);

  if (g_key_file_has_group (key, "EMU"))
    g_slist_foreach (priv->list, (GFunc)load_list_func, key);  //TODO: slow

  if (gtk_toggle_button_get_active ((GtkToggleButton*) priv->preferences->win_size))
  {
    width = g_key_file_get_integer (key, "GUI", "Width", NULL);
    height = g_key_file_get_integer (key, "GUI", "Height", NULL);
  }

  if ((width > 0) && ( height > 0))
    gtk_window_resize ((GtkWindow*) self, width, height);
  else
    gtk_window_maximize ((GtkWindow*) self);

  if (gtk_toggle_button_get_active ((GtkToggleButton*) priv->preferences->win_pos))
  {
    width = g_key_file_get_integer (key, "GUI", "PosX", NULL);
    height = g_key_file_get_integer (key, "GUI", "PosY", NULL);

    gtk_window_move ((GtkWindow*) self, width, height);
  }

  gtk_widget_show ((GtkWidget*) self);

  gsize size;
  gchar** dirs = g_key_file_get_string_list (key, "GUI", "Dirs", &size, NULL);
  path_combo_box_set_dirs (priv->pathbox, dirs, size);

  gint active = g_key_file_get_integer (key, "GUI", "ActiveDir", NULL);
  gtk_combo_box_set_active (priv->pathbox->combo, active);

  g_strfreev(dirs);
  g_key_file_free (key);
}


void
main_window_start (MainWindow* self)
{
  g_return_if_fail (self != NULL);
  MainWindowPrivate* priv = main_window_get_instance_private (self);

  gchar* welcome = g_strconcat ("Starting ",
                                gtk_about_dialog_get_program_name ((GtkAboutDialog*) priv->about_window),
                                " ",
                                gtk_about_dialog_get_version ((GtkAboutDialog*) priv->about_window),
                                "\n",
                                NULL);

  logbook_write_log (priv->logbook, LOGBOOK_LOG_TAB_FRONTEND, welcome);
  g_free (welcome);

  priv->med_process = med_process_new ();

  if (priv->med_process->MedPath == NULL)
  {
    main_window_show_error (self, "Mednafen executable not found!\n");
    gtk_widget_set_sensitive ((GtkWidget*) priv->launch_button, FALSE);
  }
  else
    gtk_widget_set_tooltip_text ((GtkWidget*) priv->status_version, priv->med_process->MedPath);

  med_process_read_conf (priv->med_process);

  if (priv->med_process->MedVersion == NULL)
    main_window_show_error (self, "Mednafen configuration file (mednafen.cfg) not found!\n");
  else
  {
    gtk_label_set_label (priv->status_version, priv->med_process->MedVersion);
    gtk_widget_set_tooltip_text ((GtkWidget*) priv->status_version, priv->med_process->MedPath);
  }

  g_signal_connect_object (priv->med_process, "exec-output", (GCallback) main_window_process_write_emu_log, self, 0);
  g_signal_connect_object (priv->med_process, "exec-emu-ended", (GCallback) main_window_process_exec_emu_ended, self, 0);

  priv->listjoy = med_list_joy_new ();
  g_signal_connect_object (priv->listjoy , "joy-found", (GCallback) main_window_joy_found, self, 0);
  med_list_joy_init_list_joy (priv->listjoy);

  g_object_set_data ((GObject*) self, "listjoy", priv->listjoy);

  main_window_get_widgets (self, (GtkWidget*) self);
  main_window_load_settings (self);
}


static void
main_window_finalize (GObject * obj)
{
  MainWindow * self = G_TYPE_CHECK_INSTANCE_CAST (obj, main_window_get_type (), MainWindow);
  MainWindowPrivate* priv = main_window_get_instance_private (self);

  g_object_unref (priv->med_process);
  g_object_unref (priv->listjoy);

  g_slist_free (priv->list);
  g_slist_free (priv->map_list);

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

  MainWindow* self = (MainWindow*) g_object_new (main_window_get_type (), "application", app, NULL);
  MainWindowPrivate* priv = main_window_get_instance_private (self);

  priv->logbook = logbook_new ();
  gtk_box_pack_start (priv->log_box, (GtkWidget*) priv->logbook, TRUE, TRUE, 0);

  g_signal_connect_object (priv->path_firmware, "emit-change", (GCallback) main_window_firmware_entry_emit_change, self, 0);

  med_widget_set_value ((MedWidget*) priv->lynx_bios, "lynxboot.img");
  med_widget_set_modified ((MedWidget*) priv->lynx_bios, FALSE);
  gtk_widget_set_visible ((GtkWidget*) priv->lynx_bios, FALSE);

  priv->pathbox = path_combo_box_new();
  g_signal_connect_object (priv->pathbox, "selected", (GCallback) main_window_path_combo_box_selected, self, 0);
  gtk_box_pack_start (priv->main_box, (GtkWidget*) priv->pathbox, FALSE, FALSE, 0);

  priv->panedlist = paned_list_new ();
  g_signal_connect_object (priv->panedlist, "filled", (GCallback) main_window_paned_list_filled, self, 0);
  g_signal_connect_object (priv->panedlist, "mouse-released", (GCallback) main_window_paned_list_mouse_released, self, 0);
  g_signal_connect_object (priv->panedlist, "item-selected", (GCallback) main_window_paned_list_item_selected, self, 0);
  g_signal_connect_object (priv->panedlist, "launched", (GCallback) main_window_paned_list_launched, self, 0);
  gtk_box_pack_start (priv->main_box, (GtkWidget*) priv->panedlist, TRUE, TRUE, 0);

  priv->bios_window = bios_window_new ((GtkWindow*) self);

  priv->manager = manager_window_new ((GtkWindow*) self, priv->pathbox->combo);

  priv->preferences = preferences_window_new ((GtkWindow*) self);
  g_signal_connect_object (priv->preferences, "on-show-tips", (GCallback) main_window_preferences_show_tooltips, self, 0);
  g_signal_connect_object (priv->preferences, "on-show-filters", (GCallback) main_window_preferences_show_filters, self, 0);

  gtk_tree_model_filter_set_visible_column (priv->systems_filter, 2);
  g_signal_connect_object (priv->preferences, "on-show-systems", (GCallback) main_window_preferences_show_systems, self, 0);

  GdkPixbuf* icon = gdk_pixbuf_new_from_resource ("/com/github/mednaffe/mednaffe.png", NULL);
  gtk_window_set_icon ((GtkWindow*) self, icon);
  g_object_unref (icon);

  priv->about_window = about_window_new ((GtkWindow*) self);
  gtk_window_set_title ((GtkWindow*) self, gtk_about_dialog_get_program_name ((GtkAboutDialog*) priv->about_window));

  return self;
}


static void
main_window_init (MainWindow * self)
{
  MainWindowPrivate* priv = main_window_get_instance_private (self);

  priv->list = NULL;
  priv->map_list = NULL;

  gtk_widget_init_template (GTK_WIDGET (self));
}


static void
main_window_class_init (MainWindowClass * klass)
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
                                             "global_notebook",
                                             FALSE,
                                             MainWindow_private_offset + G_STRUCT_OFFSET (MainWindowPrivate, global_notebook));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "system_notebook",
                                             FALSE,
                                             MainWindow_private_offset + G_STRUCT_OFFSET (MainWindowPrivate, system_notebook));

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
                                             "path_firmware",
                                             FALSE,
                                             MainWindow_private_offset + G_STRUCT_OFFSET (MainWindowPrivate, path_firmware));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "lynx_bios",
                                             FALSE,
                                             MainWindow_private_offset + G_STRUCT_OFFSET (MainWindowPrivate, lynx_bios));


  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "global_selection_changed",
                                                G_CALLBACK (main_window_global_selection_changed));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "system_selection_changed",
                                                G_CALLBACK (main_window_system_selection_changed));

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
                                                "launch_selected",
                                                G_CALLBACK (main_window_launch_clicked));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "open_rom",
                                                G_CALLBACK (main_window_menu_open_rom));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "show_preferences",
                                                G_CALLBACK (main_window_menu_show_preferences));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "medcombo_changed",
                                                G_CALLBACK (main_window_medcombo_changed));
}
