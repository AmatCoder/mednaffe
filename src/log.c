/*
 * log.c
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

void delete_log(elog log, guidata *gui)
{
  GtkTextIter start, end;

  if (log & FE)
  {
    gtk_text_buffer_get_start_iter (gui->textfe, &start);
    gtk_text_buffer_get_end_iter (gui->textfe, &end);
    gtk_text_buffer_delete (gui->textfe, &start, &end);
  }

  if (log & EMU)
  {
    gtk_text_buffer_get_start_iter (gui->textout, &start);
    gtk_text_buffer_get_end_iter (gui->textout, &end);
    gtk_text_buffer_delete (gui->textout, &start, &end);
  }
}

void print_log(const gchar *text, elog log, guidata *gui)
{
  if (log & FE)
  {
    printf("%s", text);
    gtk_text_buffer_insert_at_cursor(gui->textfe, text, -1);
  }

  if (log & EMU)
  { 
    gtk_text_buffer_insert_at_cursor(gui->textout, text, -1);
  }
}
