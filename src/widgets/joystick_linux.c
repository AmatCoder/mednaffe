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
 * 
 */


#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#include <linux/joystick.h>
#include "joystick_linux.h"


static const char*
FindSysFSInputBase()
{
  static const char* p[] = { "/sys/subsystem/input", "/sys/bus/input", "/sys/block/input", "/sys/class/input" };
  struct stat stat_buf;

  for(size_t i = 0; i < sizeof(p) / sizeof(p[0]); i++)
  {
    if(!stat (p[i], &stat_buf))
    {
      //printf ("%s\n", p[i]);
      return p[i];
    }
    else if (errno != ENOENT && errno != ENOTDIR)
    {
      printf ("stat() failed.");
      return NULL;
    }
  }
  printf ("Couldn't find input subsystem under /sys.");
  return NULL;
}


static char*
FindSysFSInputDeviceByJSDev (const char* jsdev_name)
{
  gchar* ret;
  const char* input_subsys_path = FindSysFSInputBase();
  char buf[256];
  char* tmp;

  if (input_subsys_path == NULL)
    return NULL;

  snprintf (buf, sizeof(buf), "%s/%s", input_subsys_path, jsdev_name);

  if (!(tmp = realpath (buf, NULL)))
  {
    printf("realpath(\"%s\") failed.", buf);
    return NULL;
  }

  for(;;)
  {
    char* p = strrchr(tmp, '/');
    unsigned idx;

    if (!p)
    {
      printf ("Couldn't find parent input subsystem device of joystick device \"%s\".", jsdev_name);
      return NULL;
    }

    if (sscanf(p, "/input%u", &idx) == 1)
      break;

    *p = 0;
  }
  ret = tmp;

  return ret;
}


static guint64
GetBVPV (const gchar* jsdev_name)
{
  guint64 ret = 0;
  char *lb = NULL;
  size_t len = 0;

  gchar* sysfs_input_dev_path = FindSysFSInputDeviceByJSDev (jsdev_name);

  if (sysfs_input_dev_path == NULL) return 0;

  for (unsigned i = 0; i < 4; i++)
  {
    unsigned t = 0;
    static const gchar* fns[4] = { "/id/bustype", "/id/vendor", "/id/product", "/id/version" };
    gchar* idpath = g_strconcat (sysfs_input_dev_path, fns[i], NULL);

    FILE *fp = fopen (idpath, "r");

    if (fp == NULL)
      return 0;

    getline (&lb, &len, fp);

    if (sscanf (lb, "%x", &t) != 1)
    {
      printf ("Bad data in \"%s\".", idpath);
      return 0;
    }

    if(t > 0xFFFF)
    {
      printf ("Data read from \"%s\" has value(0x%x) larger than 0xFFFF\n", idpath, t);
      t &= 0xFFFF;
    }
    ret <<= 16;
    ret |= t;

    g_free (idpath);
    fclose (fp);
  }
  g_free (sysfs_input_dev_path);
  free (lb);

  return ret;
}


void
close_fd (gint fd)
{
  if (fd > -1)
    close (fd);
}


gchar*
get_id (guint js,
        guint num_axes,
        guint num_buttons)
{
  gchar *number = g_strdup_printf ("js%i", js);
  gchar* id = g_strdup_printf ("0x%016lx%04x%04x%08x", GetBVPV (number), num_axes, num_buttons, 0);
  g_free(number);
  return id;
}


gchar*
get_name (gint fd)
{
  char name[128];

  memset(name, 0, sizeof (name));
  ioctl(fd, JSIOCGNAME(sizeof (name) - 1), name);

  return g_strndup(name, strlen (name));
}


guint
get_num_axes (gint fd)
{
  unsigned num_axes = 0;
  unsigned char tmp;

 if(ioctl(fd, JSIOCGAXES, &tmp) == -1)
   printf("Failed to get number of axes");
 else
   num_axes = tmp;

  return num_axes;
}


guint
get_num_buttons (gint fd)
{
  unsigned num_buttons = 0;
  unsigned char tmp;

 if(ioctl(fd, JSIOCGBUTTONS, &tmp) == -1)
   printf("Failed to get number of axes");
 else
   num_buttons = tmp;

  return num_buttons;
}


gint
get_joy_fd (guint js)
{
  gchar *number = g_strdup_printf ("js%i", js);

  gchar *path = g_strconcat ("/dev/input/", number, NULL);
 
  gint fd = open (path, O_RDONLY);

  g_free (path); 
  g_free (number);

  if (fd == -1)
    return -1;
 
  if (fcntl(fd, F_SETFL, fcntl (fd, F_GETFL) | O_NONBLOCK) == -1)
    printf ("WARNING: Failed to enable O_NONBLOCK flag\n");

  return fd;
}


void
discard_read (gint fd)
{
  struct js_event e;
  ssize_t s = 1;

  while (s > 0)
  {
    s = read (fd, &e, sizeof(e));
  }
}


ssize_t
read_joy (gint fd,
          gint *type,
          gint *value,
          gint *number)
{
  struct js_event e;
  ssize_t s = read (fd, &e, sizeof(e));

  if (s == -1) return s;
  if (e.value == 0) return -1;

  if ((e.type == JS_EVENT_BUTTON) ||
     ((e.type == JS_EVENT_AXIS) &&
     ((e.value < -16384) || (e.value > 16384))))
  {
    *type = e.type;
    *value = e.value;
    *number = e.number;
  }
  else return -1;

  return s;
}
