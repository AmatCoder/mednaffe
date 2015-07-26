/*
 * joystick_win.c
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
#include "md5.h"
#include "stdint.h"
#include "string.h"

uint64_t CalcOldStyleID(unsigned arg_num_axes, unsigned arg_num_balls,
                        unsigned arg_num_hats, unsigned arg_num_buttons)
{
  uint8 digest[16];
  int tohash[4];
  md5_context hashie;
  uint64_t ret = 0;

 tohash[0] = arg_num_axes;
 tohash[1] = arg_num_balls;
 tohash[2] = arg_num_hats;
 tohash[3] = arg_num_buttons;

 md5_starts(&hashie);
 md5_update(&hashie, (uint8 *)tohash, sizeof(tohash));
 md5_finish(&hashie, digest);

 int x;
 for(x = 0; x < 16; x++)
 {
   ret ^= (uint64_t)digest[x] << ((x & 7) * 8);
 }
 return ret;
}

void CheckDuplicates(guint js, guidata *gui)
{
  int a;
  for (a=0;a<9;a++)
  {
    if (a==js) break;
    if (gui->joy[js].id == gui->joy[a].id) {
      gui->joy[a].id++;
      CheckDuplicates(a, gui);
    }
  }
}

void CheckDuplicatesXInput(guint js, guidata *gui)
{
  int a;
  for (a=0;a<9;a++)
  {
    if (a==js) break;
    if (gui->joy[js].id == gui->joy[a].id) {
      gui->joy[js].id++;
      CheckDuplicates(js, gui);
    }
  }
}

gint GetJoy(guint js, guidata *gui)
{
  //printf("Number of Axes: %d\n", SDL_JoystickNumAxes(gui->joy[js].sdljoy));
  //printf("Number of Hats: %d\n", SDL_JoystickNumHats(gui->joy[js].sdljoy));
  //printf("Number of Buttons: %d\n", SDL_JoystickNumButtons(gui->joy[js].sdljoy));
  //printf("Number of Balls: %d\n", SDL_JoystickNumBalls(gui->joy[js].sdljoy));

  gui->joy[js].id = CalcOldStyleID(SDL_JoystickNumAxes(gui->joy[js].sdljoy),
                                   0,
                                   SDL_JoystickNumHats(gui->joy[js].sdljoy),
                                   SDL_JoystickNumButtons(gui->joy[js].sdljoy));

  return 1;

}
