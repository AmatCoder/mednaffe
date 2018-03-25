/*
 * joystick_linux.c
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
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#include <linux/joystick.h>

#include "common.h"

void CheckDuplicates(guint js, guidata *gui)
{
  int i;
  for (i=0; i<9; i++)
  {
    if (i == js)
      break;

    if (g_strcmp0 (gui->joy[js].id, gui->joy[i].id) == 0)
    {
      gui->joy[js].id[33]++;
      CheckDuplicates(js, gui);
    }
  }
}

static const char* FindSysFSInputBase()
{
  static const char* p[] = { "/sys/subsystem/input", "/sys/bus/input", "/sys/block/input", "/sys/class/input" };
  struct stat stat_buf;

  for(size_t i = 0; i < sizeof(p) / sizeof(p[0]); i++)
  {
    if(!stat(p[i], &stat_buf))
    {
      //printf("%s\n", p[i]);
      return p[i];
    }
    else if(errno != ENOENT && errno != ENOTDIR)
    {
      printf("stat() failed.");
      return NULL;
    }
  }
 printf("Couldn't find input subsystem under /sys.");
 return NULL;
}

static gchar* FindSysFSInputDeviceByJSDev(const gchar* jsdev_name)
{
  gchar* ret;
  const char* input_subsys_path = FindSysFSInputBase();
  char buf[256];
  char* tmp;

  if (input_subsys_path == NULL)
    return NULL;

  snprintf(buf, sizeof(buf), "%s/%s", input_subsys_path, jsdev_name);

  if(!(tmp = realpath(buf, NULL)))
  {
    printf("realpath(\"%s\") failed.", buf);
    return NULL;
  }

  for(;;)
  {
    char* p = strrchr(tmp, '/');
    unsigned idx;

    if(!p)
    {
      printf("Couldn't find parent input subsystem device of joystick device \"%s\".", jsdev_name);
      return NULL;
    }

    if(sscanf(p, "/input%u", &idx) == 1)
      break;

    *p = 0;
  }
  ret = tmp;

  return ret;
}

guint64 GetBVPV(const gchar* jsdev_name)
{
  guint64 ret = 0;
  FILE *fp;
  char *lb = NULL;
  size_t len = 0;

  gchar* sysfs_input_dev_path = FindSysFSInputDeviceByJSDev(jsdev_name);

  if (sysfs_input_dev_path == NULL) return 0;

  for(unsigned i = 0; i < 4; i++)
  {
    unsigned t = 0;
    static const gchar* fns[4] = { "/id/bustype", "/id/vendor", "/id/product", "/id/version" };
    gchar* idpath = g_strconcat(sysfs_input_dev_path, fns[i], NULL);

    fp = fopen(idpath, "r");

    if (fp == NULL)
      return 0;

    getline(&lb, &len, fp);

    if(sscanf(lb, "%x", &t) != 1)
    {
      printf("Bad data in \"%s\".", idpath);
      return 0;
    }

    if(t > 0xFFFF)
    {
      printf("Data read from \"%s\" has value(0x%x) larger than 0xFFFF\n", idpath, t);
      t &= 0xFFFF;
    }
    ret <<= 16;
    ret |= t;

    g_free(idpath);
    fclose(fp);
 }
 g_free(sysfs_input_dev_path);
 free(lb);

 return ret;
}

gint GetJoy(guint js, guidata *gui)
{
 unsigned num_axes = 0;
 unsigned num_buttons = 0;
 unsigned char tmp;

 gui->joy[js].channel= NULL;
 memset(gui->joy[js].name, 0, sizeof(gui->joy[js].name));
 gui->joy[js].name[0]= '\0';
 gui->joy[js].id = NULL;
 gui->joy[js].ev_fd = -1;
 
 gchar *number = g_strdup_printf("js%i",js);
 gchar *path = g_strconcat("/dev/input/", number, NULL);
 
 gui->joy[js].js_fd = open(path, O_RDONLY);
 
 g_free(path); 
 
 if(gui->joy[js].js_fd == -1)
 {
  g_free(number);
  return 0;
 }
 
 if (fcntl(gui->joy[js].js_fd, F_SETFL, fcntl(gui->joy[js].js_fd, F_GETFL) | O_NONBLOCK) == -1)
   printf("WARNING: Failed to enable O_NONBLOCK flag\n");

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

 gui->joy[js].id = g_strdup_printf("0x%016lx%04x%04x%08x", GetBVPV(number), num_axes, num_buttons, 0);
 g_free(number);

 CheckDuplicates(js, gui);

 return 1;
}
