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

  command2 = g_strconcat(command, " \"", gui->fullpath, "\"", NULL );

  return command2;
}

#else

gchar **build_command(guidata *gui)
{
  gchar **command;
  gint num = 5;
  GList *list = NULL;
  GList *iterator = NULL;

  list = g_hash_table_get_keys(gui->clist);
  command = g_new(gchar *, ((g_list_length(list))*2)+7);
  command[0] = g_strdup(gui->binpath);
  command[1] = g_strdup("-remote");
  command[2] = g_strdup("Mednafen_");
  command[3] = g_strdup("-psx.dbg_level");
  command[4] = g_strdup("0");
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

gchar* format_err(gchar *string, gsize len)
{
  gchar *copy;

  //memmove(string, string+24, len-27);
  string[len-3] = ' ';
  string[8]=' ';string[14]=' ';string[22]=':';string[23]='\n';
  copy = g_strcompress(string);
  g_free(string);
  return copy;
}

gboolean out_watch( GIOChannel *channel, GIOCondition cond, guidata *gui)
{
  gchar *string;
  gsize  size;

  if(cond == G_IO_HUP)
  {
    g_io_channel_unref(channel);
    return FALSE;
  }

  g_io_channel_read_line(channel, &string, &size, NULL, NULL);

  if (string)
  {
    if (size>9)
    {
      if (string[9]=='e')
      {
        GtkWidget *dialog;

        gchar *err = format_err(string, size);

         gui->m_error = TRUE;
         if (gui->state==1) gtk_window_present(GTK_WINDOW(gui->topwindow));
         if (gui->state==2) gtk_widget_show(gui->topwindow);
         dialog = gtk_message_dialog_new (GTK_WINDOW(gui->topwindow),
                                          GTK_DIALOG_DESTROY_WITH_PARENT,
                                          GTK_MESSAGE_ERROR,
                                          GTK_BUTTONS_CLOSE,
                                          "%s", err);

         gtk_dialog_run (GTK_DIALOG (dialog));
         gtk_widget_destroy (dialog);
         printf ("[Mednaffe] ***ERROR***\n%s", err);
         g_free(err);
         g_io_channel_unref(channel);

         return FALSE;
       }

       if (string[0]!='M')
       {
         g_io_channel_unref(channel);
         g_free(string);
         return FALSE;
       }
    }
  }
  else
  {
    g_io_channel_unref(channel);
    return FALSE;
  }

  g_free(string);
  return TRUE;
}

#endif

void child_watch(GPid pid, gint status, guidata *gui)
{
  #ifdef G_OS_WIN32
    DWORD lpExitCode=0;


    GetExitCodeProcess( pid, &lpExitCode);
    if (lpExitCode!=0)
    {
      gchar *string;
      gchar *err = NULL;

      gui->m_error = TRUE;

      gchar *dir = g_win32_get_package_installation_directory_of_module(NULL);
      gchar *path =g_strconcat(dir, "\\stdout.txt", NULL);

      if (g_file_get_contents(path, &string, NULL, NULL))
      {
        gchar **aline = g_strsplit(string, "\n", 0);
        gint num = g_strv_length(aline);
        if (num > 2) err = g_strconcat("Mednafen error:\n", aline[num-2], NULL);
        g_free(string);
        g_strfreev(aline);
      }
      g_free(dir);
      g_free(path);

      if (!err) err = g_strdup("Mednafen error.\nRead stdout.txt for details.");
    GtkWidget *dialog;

      dialog = gtk_message_dialog_new (GTK_WINDOW(gui->topwindow),
                                       GTK_DIALOG_DESTROY_WITH_PARENT,
                                       GTK_MESSAGE_ERROR,
                                       GTK_BUTTONS_CLOSE,
                                       "%s", err);

      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
      printf ("[Mednaffe] ***ERROR***\n%s", err);
      g_free(err);
    }
  #endif

  g_spawn_close_pid(pid);
  gui->executing = FALSE;
  gui->changed = TRUE;

  printf ("[Mednaffe] End of execution catched\n");
  printf ("[Mednaffe] Command line used: '");

  #ifdef G_OS_WIN32
    printf("%s\n", gui->command);
    g_free(gui->command);
  #else
    gint i=0;
    while (gui->command[i])
    {
      printf("%s ",gui->command[i]);
      i++;
    }
    printf ("'\n");
    g_strfreev(gui->command);
  #endif

  if (gui->m_error == FALSE)
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

  gui->m_error = FALSE;

  if (gui->state==1) gtk_window_present(GTK_WINDOW(gui->topwindow));
  if (gui->state==2) gtk_widget_show(gui->topwindow);
  gtk_widget_set_sensitive (gui->launch, TRUE);
  gtk_widget_set_sensitive (GTK_WIDGET(gtk_builder_get_object(gui->builder,
                             "inputbutton")), TRUE);
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
  printf ("[Mednaffe] Executing mednafen...\n");
  ret = CreateProcess(NULL, gui->command, NULL, NULL, FALSE, 0,
                      NULL, NULL, &si, &pi);
  if (!ret)
  {
   printf("[Mednaffe] Executing mednafen failed!\n");
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
  printf ("[Mednaffe] Executing mednafen...\n");
  ret = g_spawn_async_with_pipes( NULL, gui->command, NULL,
                                  G_SPAWN_DO_NOT_REAP_CHILD, NULL,
                                  NULL, &pid, NULL, &out, NULL, NULL );
  if (!ret)
  {
    printf("[Mednaffe] Executing mednafen failed!\n");
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

  folder = gtk_file_chooser_dialog_new(
    "Choose a ROM...", NULL,
    GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL,
    GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL );

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
