/*
 * medprocess.c
 *
 * Copyright 2013-2021 AmatCoder
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


#include <gio/gio.h>
#include "widgets/marshallers.h"
#include "medprocess.h"

#ifdef G_OS_WIN32
  #include <windows.h>
  #include "win32util.h"
#endif


typedef struct _MedProcessClass MedProcessClass;
typedef struct _MedProcessPrivate MedProcessPrivate;

struct _MedProcessClass {
  GObjectClass parent_class;
};

struct _MedProcessPrivate {
  GString* buffer;
};


G_DEFINE_TYPE_WITH_PRIVATE (MedProcess, med_process, G_TYPE_OBJECT);


enum  {
  MED_PROCESS_EXEC_EMU_ENDED_SIGNAL,
  MED_PROCESS_EXEC_OUTPUT_SIGNAL,
  MED_PROCESS_NUM_SIGNALS
};

static guint med_process_signals[MED_PROCESS_NUM_SIGNALS] = {0};


static GFile*
med_process_get_conf_path (MedProcess* self)
{
  if (self->MedConfPath == NULL)
  {
#ifdef G_OS_WIN32
    gchar* dir = win32_get_process_directory ();
    self->MedConfPath = g_strconcat (dir, "\\mednafen.cfg\\", NULL);
    g_free(dir);
#else
    const gchar* mh = g_getenv ("MEDNAFEN_HOME");

    if (mh == NULL)
      self->MedConfPath = g_strconcat (g_get_home_dir (), "/.mednafen/mednafen.cfg", NULL);
    else
      self->MedConfPath = g_strconcat (mh, "/mednafen.cfg", NULL);
#endif
  }

  GFile* file = g_file_new_for_path (self->MedConfPath);

  if (!g_file_query_exists (file, NULL))
  {
    g_free (self->MedVersion);
    self->MedVersion = NULL;
    g_free (self->MedConfPath);
    self->MedConfPath = NULL;
    g_object_unref (file);

    return NULL;
  }

  return file;
}


void
med_process_read_conf (MedProcess* self)
{
  g_return_if_fail (self != NULL);

  g_hash_table_remove_all (self->table);

  GFile* file = med_process_get_conf_path (self);

  if (!file)
    return;

  GFileInputStream* fis;
  GDataInputStream* dis = NULL;
  gchar* line = NULL;

  fis = g_file_read (file, NULL, NULL);

  if (fis)
    dis = g_data_input_stream_new ((GInputStream*) fis);
  else return;

  if (dis)
    line = g_data_input_stream_read_line (dis, NULL, NULL, NULL);
  else return;

  if (line)
  {
    g_free (self->MedVersion);
    self->MedVersion = g_strdup (g_strstr_len(line, -1, "1."));
    g_free (line);
  }

  while (TRUE)
  {
    line = g_data_input_stream_read_line (dis, NULL, NULL, NULL);

    if (line == NULL)
    {
     g_free (line);
      break;
    }

    gint len = strlen (line);

    if (len > 0)
    {
      if (line[0] != ';')
      {
        gchar **elem = g_strsplit (line, " ", 2);
        g_hash_table_insert (self->table, elem[0], elem[1]);
        g_free(elem);
      }
    }
    g_free (line);
  }

  g_object_unref (dis);
  g_object_unref (fis);
  g_object_unref (file);
}


#ifdef G_OS_UNIX

static gboolean
med_process_process_line (MedProcess* self,
                          GIOChannel* channel,
                          GIOCondition condition,
                          const gchar* stream_name)
{
  if (condition != G_IO_IN)
    return FALSE;

  MedProcessPrivate* priv = med_process_get_instance_private (self);

  gchar c[1] = {0};
  gsize pos;

  g_io_channel_read_chars (channel, c, 1, &pos, NULL);
  g_string_append_c (priv->buffer, c[0]);

  if (c[0] == '\n')
  {
    if (g_utf8_validate (priv->buffer->str, -1, NULL))
      g_signal_emit (self, med_process_signals[MED_PROCESS_EXEC_OUTPUT_SIGNAL], 0, priv->buffer->str, stream_name);

    g_string_erase (priv->buffer, 0, -1);
  }

  return TRUE;
}


static gboolean
io_func (GIOChannel* channel,
         GIOCondition condition,
         gpointer self)
{
  g_return_val_if_fail (channel != NULL, FALSE);

  return med_process_process_line ((MedProcess*)self, channel, condition, "stdout");
}

#endif

static void
child_watch_func (GPid pid,
                  gint status,
                  gpointer self)
{
  MedProcess* mp = self;
  MedProcessPrivate* priv = med_process_get_instance_private (mp);

  g_spawn_close_pid (pid);
  g_string_erase (priv->buffer, 0, -1);
  g_signal_emit (mp, med_process_signals[MED_PROCESS_EXEC_EMU_ENDED_SIGNAL], 0, status);
}


void
med_process_exec_emu (MedProcess* self,
                      gchar** command)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (command != NULL);


#ifdef G_OS_WIN32

  STARTUPINFOW si;
  PROCESS_INFORMATION pi;

  ZeroMemory (&si, sizeof(STARTUPINFOW));
  si.cb = sizeof(STARTUPINFOW);
  ZeroMemory (&pi, sizeof(PROCESS_INFORMATION));

  gchar* command_win = g_strjoinv (" ", command);
  gunichar2* command_win_utf16 = g_utf8_to_utf16 (command_win, -1, NULL, NULL, NULL);

  CreateProcessW (NULL, command_win_utf16, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

  g_free (command_win_utf16);
  g_free (command_win);

  g_child_watch_add (pi.hProcess, (GChildWatchFunc)child_watch_func, self);
  CloseHandle (pi.hThread);
#else

  GPid child_pid;
  gint standard_output;
  GIOChannel* output;

  g_spawn_async_with_pipes (NULL,
                            command,
                            NULL,
                            G_SPAWN_DO_NOT_REAP_CHILD,
                            NULL,
                            NULL,
                            &child_pid,
                            NULL,
                            &standard_output,
                            NULL,
                            NULL);

  output = g_io_channel_unix_new (standard_output);

  g_io_channel_set_encoding(output, NULL, NULL);
  g_io_channel_set_flags (output, G_IO_FLAG_NONBLOCK, NULL);

  g_io_add_watch (output, G_IO_IN | G_IO_HUP, io_func, self);
  g_child_watch_add_full (G_PRIORITY_DEFAULT_IDLE, child_pid, child_watch_func, self, NULL);

  g_io_channel_unref (output);

#endif
}


MedProcess*
med_process_new (void)
{
  MedProcess *self = (MedProcess*) g_object_new (med_process_get_type(), NULL);

  gboolean b = FALSE;
#ifdef G_OS_WIN32
  gchar *bin =  g_find_program_in_path ("mednafen.exe");

  if (bin != NULL)
    self->MedExePath = g_strconcat("\"", bin, "\"", NULL);

  g_free (bin);
#else
  self->MedExePath = g_find_program_in_path ("mednafen");
#endif
  if (self->MedExePath != NULL)
  {
#ifdef G_OS_WIN32
    GFile* test = med_process_get_conf_path (self);

    bin = g_strconcat(self->MedExePath, " --help", NULL);

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory (&si, sizeof(STARTUPINFOW));
    si.cb = sizeof(STARTUPINFOW);
    ZeroMemory (&pi, sizeof(PROCESS_INFORMATION));

    gunichar2* command_win_utf16 = g_utf8_to_utf16 (bin, -1, NULL, NULL, NULL);

    b = CreateProcessW (NULL, command_win_utf16, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

    DWORD ret = WaitForSingleObject (pi.hProcess, 2500);

    if (b)
    {
      CloseHandle (pi.hProcess);
      CloseHandle (pi.hThread);
    }

    if (ret != WAIT_OBJECT_0)
      b = FALSE;

    if (test != NULL)
      g_object_unref (test);
    else
      Sleep (2500);

    g_free (bin);
#else
    gchar* stdout;
    gchar* stderr;

    gchar *cl = g_shell_quote (self->MedExePath);
    b = g_spawn_command_line_sync (cl, &stdout, &stderr, NULL, NULL);

    g_free (stdout);
    g_free (stderr);
    g_free (cl);
#endif

    if (!b)
    {
      g_free (self->MedExePath);
      self->MedExePath = NULL;
    }
  }

  return self;
}


static void
med_process_finalize (GObject* obj)
{
  MedProcess * self = G_TYPE_CHECK_INSTANCE_CAST (obj, med_process_get_type(), MedProcess);
  MedProcessPrivate* priv = med_process_get_instance_private (self);

  g_free (self->MedVersion);
  g_free (self->MedExePath);
  g_free (self->MedConfPath);

  g_hash_table_unref (self->table);
  g_string_free (priv->buffer, TRUE);

  G_OBJECT_CLASS (med_process_parent_class)->finalize (obj);
}


static void
med_process_init (MedProcess* self)
{
  MedProcessPrivate* priv = med_process_get_instance_private (self);

  self->MedVersion = NULL;
  self->MedExePath = NULL;
  self->MedConfPath = NULL;

  self->table = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
  priv->buffer = g_string_sized_new (512);
}


static void
med_process_class_init (MedProcessClass* klass)
{
  G_OBJECT_CLASS (klass)->finalize = med_process_finalize;

  med_process_signals[MED_PROCESS_EXEC_EMU_ENDED_SIGNAL] = g_signal_new ("exec-emu-ended",
                                                                         med_process_get_type(),
                                                                         G_SIGNAL_RUN_LAST,
                                                                         0,
                                                                         NULL,
                                                                         NULL,
                                                                         g_cclosure_marshal_VOID__INT,
                                                                         G_TYPE_NONE,
                                                                         1,
                                                                         G_TYPE_INT);

  med_process_signals[MED_PROCESS_EXEC_OUTPUT_SIGNAL] = g_signal_new ("exec-output",
                                                                       med_process_get_type(),
                                                                       G_SIGNAL_RUN_LAST,
                                                                       0,
                                                                       NULL,
                                                                       NULL,
                                                                       g_cclosure_user_marshal_VOID__STRING_STRING,
                                                                       G_TYPE_NONE,
                                                                       2,
                                                                       G_TYPE_STRING,
                                                                       G_TYPE_STRING);
}
