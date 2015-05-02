/*
 * joystick_linux.c
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
#include <linux/joystick.h>
#include <fcntl.h>

gchar* FindEv(int js)
{
  gchar *path = NULL;
  gchar *syspath;
  
  gchar *number = g_strdup_printf("%i",js);
  syspath = g_strconcat("/sys/class/input/js", number, "/device/", NULL);
  g_free(number);
  
  GDir *dir =g_dir_open (syspath, 0, NULL);
  if (dir)
  { 
    const gchar* file = NULL;	  
    do {
         file = g_dir_read_name(dir);
         if ((g_ascii_strncasecmp("event",file,5) == 0)) break;
       } while (file != NULL);
  
    if (file) path = g_strconcat("/dev/input/", file, NULL);
 
    g_dir_close(dir);
  }
  g_free(syspath);
  return path;
}

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
    if (gui->joy[js].id == gui->joy[a].id){
      gui->joy[js].id++;
      CheckDuplicates(js, gui);
    }  
  } 
}

gint GetJoy(guint js, guidata *gui)
{
 unsigned num_axes = 0;
 unsigned num_buttons = 0;
 unsigned char tmp;

 gui->joy[js].channel= NULL;
 
 gchar *number = g_strdup_printf("%i",js);
 gchar *path = g_strconcat("/dev/input/js", number, NULL);
 
 gui->joy[js].js_fd = open(path, O_RDONLY);
 
 g_free(number);
 g_free(path); 
 
 if(gui->joy[js].js_fd == -1)
 {
  return 0;
 }
 
 gchar *evdev_path = FindEv(js);
 
 fcntl(gui->joy[js].js_fd, F_SETFL, fcntl(gui->joy[js].js_fd, F_GETFL) | O_NONBLOCK);

 if(evdev_path != NULL)
 {
   gui->joy[js].ev_fd = open(evdev_path, O_RDWR);
   
   if(gui->joy[js].ev_fd == -1)
   {
     gui->joy[js].ev_fd = open(evdev_path, O_RDONLY);
     if(gui->joy[js].ev_fd == -1)
     {
       printf("WARNING: Failed to open event device \"%s\" --- !!!!! BASE JOYSTICK FUNCTIONALITY WILL BE AVAILABLE, BUT FORCE-FEEDBACK(E.G. RUMBLE) WILL BE UNAVAILABLE, AND THE CALCULATED JOYSTICK ID WILL BE DIFFERENT. !!!!!\n", evdev_path);
     }
     else
     {
       printf("WARNING: Could only open event device \"%s\" for reading, and not reading+writing --- !!!!! FORCE-FEEDBACK(E.G. RUMBLE) WILL BE UNAVAILABLE. !!!!!\n", evdev_path);
     }
   }
 }
 else
   printf("WARNING: Failed to find a valid corresponding event device to joystick device \"s\" --- !!!!! BASE JOYSTICK FUNCTIONALITY WILL BE AVAILABLE, BUT FORCE-FEEDBACK(E.G. RUMBLE) WILL BE UNAVAILABLE, AND THE CALCULATED JOYSTICK ID WILL BE DIFFERENT. !!!!!\n");

 if(gui->joy[js].ev_fd != -1)
  fcntl(gui->joy[js].ev_fd, F_SETFL, fcntl(gui->joy[js].ev_fd, F_GETFL) | O_NONBLOCK);

 if(ioctl(gui->joy[js].js_fd, JSIOCGAXES, &tmp) == -1)
 {
   printf("Failed to get number of axes");
 }
 else
   num_axes = tmp;

 if(ioctl(gui->joy[js].js_fd, JSIOCGBUTTONS, &tmp) == -1)
 {
   printf("Failed to get number of buttons");
 }
 else
   num_buttons = tmp;

 ioctl(gui->joy[js].js_fd, JSIOCGNAME(sizeof(gui->joy[js].name)),gui->joy[js].name);

 if(gui->joy[js].ev_fd != -1)
 {
   uint8 absaxbits[(ABS_CNT + 7) / 8];
   unsigned ev_abs_count = 0;
   unsigned ev_hat_count = 0;
   unsigned aat;
   
   memset(absaxbits, 0, sizeof(absaxbits));
   ioctl(gui->joy[js].ev_fd, EVIOCGBIT(EV_ABS, sizeof(absaxbits)), absaxbits);

  for(aat = 0; aat < ABS_CNT; aat++)
  {
    if(absaxbits[(aat >> 3)] & (1U << (aat & 0x7)))
    {
      if(aat >= ABS_HAT0X && aat <= ABS_HAT3Y)
      {
        ev_hat_count++;
      }
      ev_abs_count++;
    }
  }
  close(gui->joy[js].ev_fd);
  g_free(evdev_path);
  gui->joy[js].id = CalcOldStyleID(ev_abs_count - ev_hat_count, 0, ev_hat_count / 2, num_buttons);
  CheckDuplicates(js, gui);
  return 2;
  }
  else
  {
  g_free(evdev_path);
  gui->joy[js].id = CalcOldStyleID(num_axes, 0, 0, num_buttons);
  CheckDuplicates(js, gui);
  return 1;
  }
}
