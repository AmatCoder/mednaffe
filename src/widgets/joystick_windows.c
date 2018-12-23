/*
 * joystick_windows.c
 * 
 * Copyright 2013-2018 AmatCoder
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

#include "joystick_windows.h"


void
close_fd (gint fd)
{
}


gchar*
get_id (guint js,
        guint num_axes,
        guint num_buttons)
{
  gchar* id = g_strdup ("0x0000000000000000");
  return id;
}


gchar*
get_name (gint fd)
{
  return g_strdup("");
}


guint
get_num_axes (gint fd)
{
  return 0;
}


guint
get_num_buttons (gint fd)
{
  return 0;
}


gint
get_joy_fd (guint js)
{
  return -1;
}


void
discard_read (gint fd)
{
}


ssize_t
read_joy (gint fd,
          gint *type,
          gint *value,
          gint *number)
{
  return -1;
}
