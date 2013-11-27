/*
 * command.c
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
      g_strconcat(command2, " ", 
        g_hash_table_lookup(gui->hash, iterator->data), NULL);
        
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
  gint num = 1;
  GList *list = NULL;
  GList *iterator = NULL;

  list = g_hash_table_get_keys(gui->clist);
  command = g_new(gchar *, ((g_list_length(list))*2)+3);
  command[0] = g_strdup(gui->binpath);

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

#endif

void child_watch(GPid pid, gint status, guidata *gui)
{
  gpointer name;
  
  #ifdef G_OS_WIN32
    DWORD lpExitCode=0;
  
    GetExitCodeProcess( pid, &lpExitCode);
    if (lpExitCode!=0)
    {
	  GtkWidget *dialog;
	  
      dialog = gtk_message_dialog_new (GTK_WINDOW(gui->topwindow),
                                       GTK_DIALOG_DESTROY_WITH_PARENT,
                                       GTK_MESSAGE_ERROR,
                                       GTK_BUTTONS_CLOSE,
                       "Mednafen error.\nRead stdout.txt for details.");
                       
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
      printf ("[Mednaffe] Mednafen error. Read stdout.txt for details.");
    }
  #endif
  
  g_spawn_close_pid( pid );
  gui->executing = FALSE;

  printf ("\n[Mednaffe] End of execution catched\n");
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
  
  g_hash_table_remove_all(gui->clist);

  /* Always to send 'video.fs' and 'cheats' */
  name = g_strdup(g_object_get_data(gtk_builder_get_object(
                                     gui->builder, "-video.fs"), "cname"));

  g_hash_table_insert(gui->clist, name, name);
  name = g_strdup(g_object_get_data(gtk_builder_get_object(
                                     gui->builder, "-cheats"), "cname"));

  g_hash_table_insert(gui->clist, name, name);

  if (gui->state==1) gtk_window_present(GTK_WINDOW(gui->topwindow));
  if (gui->state==2) gtk_widget_show(gui->topwindow);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
void row_exec(GtkTreeView *treeview, GtkTreePath *patho,
              GtkTreeViewColumn *col, guidata *gui)
{
  if ((gui->executing == TRUE) || (gui->rompath == NULL)) 
    return;
    
  BOOL ret = FALSE;
  STARTUPINFO si;  
  PROCESS_INFORMATION pi; 
   
  ZeroMemory(&si, sizeof(STARTUPINFO));
  si.cb = sizeof(STARTUPINFO);
  ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
  
  gui->command = build_command_win(gui);
  printf ("[Mednaffe] Executing mednafen:\n\n");
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
    gui->executing = TRUE;
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

  if ((gui->executing == TRUE) || (gui->rompath == NULL)) 
    return;

  gui->command = build_command(gui);
  printf ("[Mednaffe] Executing mednafen:\n\n");
  ret = g_spawn_async_with_pipes( NULL, gui->command, NULL,
                                  G_SPAWN_DO_NOT_REAP_CHILD, NULL,
                                  NULL, &pid, NULL, NULL, NULL, NULL );
  if (!ret)
  {
    printf("[Mednaffe] Executing mednafen failed!\n");
    g_strfreev(gui->command);
    return;
  }
  
  g_child_watch_add(pid, (GChildWatchFunc)child_watch, gui);
  
  gui->executing = TRUE;
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
