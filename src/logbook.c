/*
 * logbook.c
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


#include "logbook.h"

typedef struct _LogbookClass LogbookClass;
typedef struct _LogbookPrivate LogbookPrivate;


struct _LogbookClass {
  GtkNotebookClass parent_class;
};

struct _LogbookPrivate {
  GtkTextView* fe_log;
  GtkTextView* emu_log;
};


G_DEFINE_TYPE_WITH_PRIVATE (Logbook, logbook, GTK_TYPE_NOTEBOOK);


void
logbook_write_log (Logbook* self,
                   LogbookLogTab tab,
                   const gchar* line)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (line != NULL);

  LogbookPrivate* priv = logbook_get_instance_private (self);

  if (tab == LOGBOOK_LOG_TAB_FRONTEND)
  {
    GDateTime* dt = g_date_time_new_now_local ();

    gchar* s = g_date_time_format (dt, "%H:%M:%S");
    gchar* s2 = g_strconcat ("[", s, "] ", line, NULL);
    g_free (s);

    printf ("%s", s2);

    GtkTextBuffer* tb = gtk_text_view_get_buffer (priv->fe_log);
    gtk_text_buffer_insert_at_cursor (tb, s2, -1);

    g_free (s2);
    g_date_time_unref (dt);

  }
  else
  {
    GtkTextBuffer* bt = gtk_text_view_get_buffer (priv->emu_log);
    gtk_text_buffer_insert_at_cursor (bt, line, -1);
  }
}

gchar*
logbook_get_last_line (Logbook* self)
{
  g_return_val_if_fail (self != NULL, NULL);

  LogbookPrivate* priv = logbook_get_instance_private (self);

  GtkTextBuffer* buffer;
  GtkTextIter start;
  GtkTextIter end;
  gint count;
  gchar* result = NULL;

  buffer = gtk_text_view_get_buffer (priv->emu_log);
  count = gtk_text_buffer_get_line_count (buffer);

  if (count > 1)
  {
    gtk_text_buffer_get_iter_at_line (buffer, &start, count - 2);
    gtk_text_buffer_get_iter_at_line (buffer, &end, count - 1);
    result = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
  }

  return result;
}


void
logbook_delete_log (Logbook* self,
                    LogbookLogTab tab)
{
  g_return_if_fail (self != NULL);

  LogbookPrivate* priv = logbook_get_instance_private (self);

  GtkTextBuffer* buffer;
  GtkTextIter start;
  GtkTextIter end;

  if (tab == LOGBOOK_LOG_TAB_FRONTEND)
    buffer = gtk_text_view_get_buffer (priv->fe_log);
  else
    buffer = gtk_text_view_get_buffer (priv->emu_log);

  gtk_text_buffer_get_start_iter (buffer, &start);
  gtk_text_buffer_get_end_iter (buffer, &end);

  gtk_text_buffer_delete (buffer, &start, &end);
}


Logbook*
logbook_new (void)
{
  Logbook* self = (Logbook*) g_object_new (logbook_get_type (), NULL);

  LogbookPrivate* priv = logbook_get_instance_private (self);

  GtkLabel* title = (GtkLabel*) gtk_label_new ("FrontEnd");
  GtkScrolledWindow* scrolled = (GtkScrolledWindow*) gtk_scrolled_window_new (NULL, NULL);

  gtk_notebook_append_page ((GtkNotebook*) self, (GtkWidget*) scrolled, (GtkWidget*) title);

  priv->fe_log = (GtkTextView*) gtk_text_view_new ();
  gtk_container_add ((GtkContainer*) scrolled, (GtkWidget*) priv->fe_log);

  GtkLabel* title2 = (GtkLabel*) gtk_label_new ("Emulator");
  GtkScrolledWindow* scrolled2 = (GtkScrolledWindow*) gtk_scrolled_window_new (NULL, NULL);

  gtk_notebook_append_page ((GtkNotebook*) self, (GtkWidget*) scrolled2, (GtkWidget*) title2);

  priv->emu_log  = (GtkTextView*) gtk_text_view_new ();
  gtk_container_add ((GtkContainer*) scrolled2, (GtkWidget*) priv->emu_log);

  gtk_widget_show_all ((GtkWidget*) self);

  return self;
}


static void
logbook_init (Logbook * self)
{
}


static void
logbook_class_init (LogbookClass * klass)
{
}
