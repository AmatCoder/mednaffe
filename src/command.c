/*
 * command.c
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
#include "log.h"

#ifdef G_OS_WIN32
  #include <windows.h>
#endif

#ifdef G_OS_WIN32
gchar *build_command_win(guidata *gui)
{
  gchar *command;
  gchar *command2;
  GList *list = NULL;
  GList *iterator = NULL;

  list = g_hash_table_get_keys(gui->clist);
  command = g_strconcat("\"", gui->binpath, "\"", NULL);

  for (iterator = list; iterator; iterator = iterator->next)
  {
    command2 = g_strconcat(command, " ", iterator->data, NULL);
    g_free(command);
    iterator->data = ((gchar *)iterator->data)+1;

    command =
      g_strconcat(command2, " \"",
        g_hash_table_lookup(gui->hash, iterator->data), "\"", NULL);

    g_free(command2);
  }
  g_list_free(list);

  command2 = g_strconcat(command, " -psx.dbg_level \"0\"", NULL );
  g_free(command);
  command = g_strconcat(command2, " \"", gui->fullpath, "\"", NULL );
  g_free(command2);

  return command;
}

#else

gchar **build_command(guidata *gui)
{
  gchar **command;
  gint num = 3;
  GList *list = NULL;
  GList *iterator = NULL;

  list = g_hash_table_get_keys(gui->clist);
  command = g_new(gchar *, ((g_list_length(list))*2)+5);
  command[0] = g_strdup(gui->binpath);
  command[1] = g_strdup("-psx.dbg_level");
  command[2] = g_strdup("0");
  for (iterator = list; iterator; iterator = iterator->next)
  {
    command[num] = g_strdup(iterator->data);
    num++;
    iterator->data = ((gchar *)iterator->data)+1;
    command[num] =
      g_strdup(g_hash_table_lookup(gui->hash, iterator->data));
    num++;
  }
  g_list_free(list);

  command[num] = g_strdup(gui->fullpath);

  num++;
  command[num] = NULL;

  return command;
}

gboolean out_watch( GIOChannel *channel, GIOCondition cond, guidata *gui)
{
  gsize size;

  if (cond == G_IO_HUP)
  {
    g_io_channel_unref(channel);
    return FALSE;
  }

  g_free(gui->m_error);
  gui->m_error= NULL;

  if (g_io_channel_read_line(channel, &gui->m_error, &size, NULL, NULL) != G_IO_STATUS_NORMAL)
    return TRUE;

  if (gui->m_error != NULL)
  {
      gtk_text_buffer_insert_at_cursor(gui->textout, gui->m_error, size);
  }
  return TRUE;
}

#endif

void child_watch(GPid pid, gint status, guidata *gui)
{
  #ifdef G_OS_WIN32
    DWORD lpExitCode=0;

    GetExitCodeProcess( pid, &lpExitCode);

    gchar *string;
    gchar *dir = g_path_get_dirname (gui->binpath);
    gchar *path =g_strconcat(dir, "\\stdout.txt", NULL);

    if (g_file_get_contents(path, &string, NULL, NULL))
    {
      print_log("----\n", EMU, gui);
      print_log(string, EMU, gui);

      gchar **aline = g_strsplit(string, "\n", 0);
      gint num = g_strv_length(aline);
      if (num > 2) 
        gui->m_error = g_strconcat("Mednafen error: ", aline[num-2], NULL);

      g_strfreev(aline);
    }
    g_free(string);  
    g_free(dir);
    g_free(path);

    if (lpExitCode!=0)
    {
      if (gui->m_error == NULL) 
        gui->m_error = g_strdup("Mednafen error. Read stdout.txt for details.");

      GtkWidget *dialog;

      dialog = gtk_message_dialog_new (GTK_WINDOW(gui->topwindow),
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_CLOSE,
                                        "%s", gui->m_error);

      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
      print_log("***ERROR***: ", FE, gui);
      print_log(gui->m_error, FE, gui);
    }
  #else
    if (status!=0)
    {
      GtkWidget *dialog;

      if (gui->state==1) gtk_window_present(GTK_WINDOW(gui->topwindow));
      if (gui->state==2) gtk_widget_show(gui->topwindow);

      if (gui->m_error == NULL) gui->m_error = g_strdup("Unspecified error");

      dialog = gtk_message_dialog_new (GTK_WINDOW(gui->topwindow),
                                       GTK_DIALOG_DESTROY_WITH_PARENT,
                                       GTK_MESSAGE_ERROR,
                                       GTK_BUTTONS_CLOSE,
                                       "%s", gui->m_error);

      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
      print_log("***ERROR***: ", FE, gui);
      print_log(gui->m_error, FE, gui);
    }
  #endif
    else
    {
      g_hash_table_remove_all(gui->clist);

      /* Send 'video.fs' and 'cheats' every time */
      gpointer name;
      name = g_strdup(g_object_get_data(gtk_builder_get_object(
                                  gui->builder, "-video.fs"), "cname"));

      g_hash_table_insert(gui->clist, name, name);

      name = g_strdup(g_object_get_data(gtk_builder_get_object(
                                    gui->builder, "-cheats"), "cname"));

      g_hash_table_insert(gui->clist, name, name);
    }

  g_free(gui->m_error);
  gui->m_error = NULL;

  g_spawn_close_pid(pid);
  gui->executing = FALSE;
  gui->changed = TRUE;

  #ifdef G_OS_WIN32
    g_free(gui->command);
  #else
    g_strfreev(gui->command);
  #endif

  if (gui->state==1) gtk_window_present(GTK_WINDOW(gui->topwindow));
  if (gui->state==2) gtk_widget_show(gui->topwindow);
  gtk_widget_set_sensitive (gui->launch, TRUE);
  gtk_widget_set_sensitive (GTK_WIDGET(gtk_builder_get_object(gui->builder,
                             "inputbutton")), TRUE);
  gtk_widget_set_sensitive (GTK_WIDGET(gtk_builder_get_object(gui->builder,
                             "keyinputbutton")), TRUE);

  print_log("----\n", EMU, gui);
  print_log("End of execution catched.\n", FE|EMU, gui);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
void row_exec(GtkTreeView *treeview, GtkTreePath *patho,
              GtkTreeViewColumn *col, guidata *gui)
{
  if ((gui->executing == TRUE) || (gui->fullpath == NULL))
    return;

  BOOL ret;
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(STARTUPINFO));
  si.cb = sizeof(STARTUPINFO);
  ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

  gui->command = build_command_win(gui);
  delete_log(EMU, gui);

  print_log("Executing mednafen with command line: \n", FE|EMU, gui);
  print_log(gui->command, FE|EMU, gui);
  print_log("\n", FE|EMU, gui);

  ret = CreateProcess(NULL, gui->command, NULL, NULL, FALSE, 0,
                      NULL, NULL, &si, &pi);
  if (!ret)
  {
   print_log("Executing mednafen failed!\n", FE, gui);
   g_free(gui->command);
   return;
  }
  else
  {
    g_child_watch_add(pi.hProcess, (GChildWatchFunc)child_watch, gui);
    CloseHandle(pi.hThread);
    gui->executing = TRUE;
    gtk_widget_set_sensitive(gui->launch, FALSE);
    gtk_widget_set_sensitive (GTK_WIDGET(gtk_builder_get_object(gui->builder,
                             "inputbutton")), FALSE);
    gtk_widget_set_sensitive (GTK_WIDGET(gtk_builder_get_object(gui->builder,
                             "keyinputbutton")), FALSE);

    if (gui->state==1) gtk_window_iconify(GTK_WINDOW(gui->topwindow));
    if (gui->state==2) gtk_widget_hide(gui->topwindow);
  }
}

#else

void row_exec(GtkTreeView *treeview, GtkTreePath *patho,
              GtkTreeViewColumn *col, guidata *gui)
{
  GPid pid;
  gboolean ret;
  gint out;
  GIOChannel *out_ch;

  if ((gui->executing == TRUE) || (gui->fullpath == NULL))
    return;

  gui->command = build_command(gui);
  
  delete_log(EMU, gui);
  print_log("Executing mednafen with command line: \n\"", FE|EMU, gui);

    gint i=0;
    while (gui->command[i])
    {
      print_log(gui->command[i], FE|EMU, gui);
      print_log(" ", FE|EMU, gui);
      i++;
    }
    print_log("\"\n", FE|EMU, gui);
    print_log("----\n", EMU, gui);

  ret = g_spawn_async_with_pipes( NULL, gui->command, NULL,
                                  G_SPAWN_DO_NOT_REAP_CHILD, NULL,
                                  NULL, &pid, NULL, &out, NULL, NULL );
  if (!ret)
  {
    print_log("Executing mednafen failed!\n", FE, gui);
    g_strfreev(gui->command);
    return;
  }

  g_child_watch_add(pid, (GChildWatchFunc)child_watch, gui);
  out_ch = g_io_channel_unix_new(out);
  g_io_channel_set_flags (out_ch, G_IO_FLAG_NONBLOCK, NULL);
  //g_io_channel_set_close_on_unref(out_ch, TRUE);
  g_io_add_watch(out_ch, G_IO_IN|G_IO_HUP, (GIOFunc)out_watch, gui);

  gui->executing = TRUE;
  gtk_widget_set_sensitive (gui->launch, FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET(gtk_builder_get_object(gui->builder,
                             "inputbutton")), FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET(gtk_builder_get_object(gui->builder,
                             "keyinputbutton")), FALSE);
  if (gui->state==1) gtk_window_iconify(GTK_WINDOW(gui->topwindow));
  if (gui->state==2) gtk_widget_hide(gui->topwindow);
}

#endif

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void play_exec(GtkButton *button, guidata *gui)
{
  row_exec(NULL, NULL, NULL, gui);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void open_rom(GtkWidget *sender, guidata *gui)
{
  GtkWidget *folder;

#ifdef GTK2_ENABLED
  folder = gtk_file_chooser_dialog_new(
    "Choose a ROM...", GTK_WINDOW(gui->topwindow),
    GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL,
    GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL );
#else
  folder = gtk_file_chooser_dialog_new(
    "Choose a ROM...", GTK_WINDOW(gui->topwindow),
    GTK_FILE_CHOOSER_ACTION_OPEN, ("_Cancel"),
    GTK_RESPONSE_CANCEL, ("_Open"), GTK_RESPONSE_ACCEPT, NULL);
#endif
  if (gtk_dialog_run(GTK_DIALOG(folder)) == GTK_RESPONSE_ACCEPT)
  {
    gchar *filename;

    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (folder));

    if (filename != NULL)
    {
      gchar *g_fullpath;

    g_fullpath = gui->fullpath;
    gui->fullpath = filename;

    row_exec(NULL, NULL, NULL, gui);

    g_free(gui->fullpath);
    gui->fullpath = g_fullpath;
  }
  }
  gtk_widget_destroy(folder);
}
