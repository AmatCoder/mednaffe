/*
 * joystick_dummy.c
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


#include "joystick.h"


void
close_joys (GSList *list)
{
}


gchar*
value_to_text (GSList* listjoy, const gchar* value)
{
  return g_strdup (value);
}


GSList*
init_joys (void)
{
  return NULL;
}


void
discard_read (GSList *list)
{
}


gchar*
read_joys (GSList *list)
{
  return NULL;
}
