/*
 * joystick_win.c
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
 */

#include "common.h"

void CheckDuplicates(guint js, guidata *gui)
{
  int a;
  for (a=0;a<9;a++)
  {
    if (a==js)
      break;

    if (g_strcmp0 (gui->joy[js].id, gui->joy[a].id) == 0)
    {
      gui->joy[a].id[33]++;
      CheckDuplicates(a, gui);
    }

  }
}

void CheckDuplicatesXInput(guint js, guidata *gui)
{
  int a;
  for (a=0;a<9;a++)
  {
    if (a==js)
      break;

    if (g_strcmp0 (gui->joy[js].id, gui->joy[a].id) == 0)
    {
      gui->joy[js].id[33]++;
      CheckDuplicatesXInput(js, gui);
    }

  }
}
