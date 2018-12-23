/*
 * joystick_windows.h
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


#ifndef __JOYSTICK_WINDOWS_H__
#define __JOYSTICK_WINDOWS_H__

#include <glib.h>


G_BEGIN_DECLS


gint get_joy_fd (guint js);
guint get_num_buttons (gint fd);
guint get_num_axes (gint fd);
gchar* get_name (gint fd);
gchar* get_id (guint js, guint num_axes, guint num_buttons);
ssize_t read_joy (gint fd, gint *type, gint *value, gint *number);
void close_fd (gint fd);
void discard_read (gint fd);

G_END_DECLS

#endif
