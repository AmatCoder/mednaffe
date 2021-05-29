/*
 * joystick_linux.c
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


#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <linux/joystick.h>

#include "joystick.h"


static const char*
FindSysFSInputBase (void)
{
  static const char* p[] = { "/sys/subsystem/input", "/sys/bus/input", "/sys/block/input", "/sys/class/input" };
  struct stat stat_buf;

  for (size_t i = 0; i < sizeof(p) / sizeof(p[0]); i++)
  {
    if (!stat (p[i], &stat_buf))
    {
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
    printf ("realpath(\"%s\") failed.", buf);
    return NULL;
  }

  for(;;)
  {
    char* p = strrchr (tmp, '/');
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

    if (getline (&lb, &len, fp) == -1)
    {
      printf ("Fail to read a line in \"%s\".", idpath);
      fclose (fp);
      g_free (idpath);
      g_free (sysfs_input_dev_path);
      return 0;
    }

    if (sscanf (lb, "%x", &t) != 1)
    {
      printf ("Bad data in \"%s\".", idpath);
      fclose (fp);
      g_free (idpath);
      g_free (sysfs_input_dev_path);
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
close_joys (GSList *list)
{
  GSList* it = NULL;
  for (it = list; it != NULL; it = it->next)
  {
    joy_s *joy = it->data;
    close (GPOINTER_TO_INT (joy->data1));
    g_free (joy->name);
    g_free (joy->id);
    g_free (joy);
  }
}


GSList*
init_joys (void)
{
  GSList *joy_list = NULL;
  gint js = 0;

  while (1)
  {
    gchar *number = g_strdup_printf ("js%i", js);
    gchar *path = g_strconcat ("/dev/input/", number, NULL);

    gint fd = open (path, O_RDONLY);

    g_free (path);

    if (fd == -1)
    {
      g_free (number);
      break;
    }

    if (fcntl (fd, F_SETFL, fcntl (fd, F_GETFL) | O_NONBLOCK) == -1)
      printf ("WARNING: Failed to enable O_NONBLOCK flag\n");

    unsigned char tmp;
    unsigned num_buttons = 0;
    unsigned num_axes = 0;
    char aname[128];

   if (ioctl (fd, JSIOCGBUTTONS, &tmp) == -1)
     printf ("Failed to get number of axes");
   else
     num_buttons = tmp;

   if (ioctl (fd, JSIOCGAXES, &tmp) == -1)
     printf ("Failed to get number of axes");
   else
     num_axes = tmp;

    memset (aname, 0, sizeof (aname));
    ioctl (fd, JSIOCGNAME(sizeof (aname) - 1), aname);

    joy_s *joy = g_new0 (joy_s, 1);
    joy->num = js;
    joy->type = 1;
    joy->name = g_strndup (aname, strlen (aname));
    joy->id = g_strdup_printf ("0x%016"G_GINT64_MODIFIER"x%04x%04x%08x", GetBVPV (number), num_axes, num_buttons, 0);
    joy->data1 = GINT_TO_POINTER(fd);

    g_free (number);

    joy_list = g_slist_append (joy_list, joy);

    js++;
  }

  return joy_list;
}


static const gchar*
get_name_from_id (GSList* list,
                  const gchar* id)
{
  g_return_val_if_fail (id != NULL, NULL);

  const gchar* name = "Unknown Device";

  GSList* it = NULL;
  for (it = list; it != NULL; it = it->next)
  {
    joy_s *joy = it->data;
    const gchar* joy_id = joy->id;

    if (g_strcmp0 (id, joy_id) == 0)
    {
      name = joy->name;
      break;
    }
  }

  return name;
}


gchar*
value_to_text (GSList* listjoy,
               const gchar* value)
{
  gchar* text = NULL;
  gchar** items = g_strsplit (value, " ", 4);

  if (g_strv_length (items) < 3)
  {
    g_strfreev (items);
    return text;
  }

  if (items[2][0] == 'b')
  {
    if (strlen (items[2]) > 7)
    {
      text = g_strconcat ("Button ", (items[2] + 7),
                          " (", get_name_from_id (listjoy, items[1]),
                          ")",  NULL);
    }
  }
  else if (items[2][0] == 'a')
  {
    if (strlen (items[2]) > 4)
    {
      const gchar* name = get_name_from_id (listjoy, items[1]);
      text = g_strconcat ("Axis ", (items[2] + 4), " (", name, ")", NULL);
    }
  }

  g_strfreev (items);

  return text;
}


void
discard_read (GSList *list)
{
  GSList* it = NULL;
  for (it = list; it != NULL; it = it->next)
  {
    joy_s *joy = it->data;
    struct js_event e;
    ssize_t s = 1;

    while (s > 0)
    {
      s = read (GPOINTER_TO_INT (joy->data1), &e, sizeof(e));
    }
  }
}


gchar*
read_joys (GSList *list)
{
  gchar* value = NULL;

  GSList* it = NULL;
  for (it = list; it != NULL; it = it->next)
  {
    joy_s *joy = it->data;
    struct js_event e;
    ssize_t st = read (GPOINTER_TO_INT (joy->data1), &e, sizeof(e));

    if ( (st == -1) || (e.value == 0) ) continue;

    if ((e.type == JS_EVENT_BUTTON) ||
       ((e.type == JS_EVENT_AXIS) &&
       ((e.value < -16384) || (e.value > 16384))))
    {
      if (e.type == 1)
      {
        gchar* n = g_strdup_printf ("%i", e.number);
        value = g_strconcat ("joystick ", joy->id, " button_", n, NULL);
        g_free (n);

        return value;
      }
      else if (e.type == 2)
      {
        gchar* s;

        if (e.value > 0)
          s = g_strdup ("+");
        else
          s = g_strdup ("-");

        gchar* n = g_strdup_printf ("%i", e.number);
        value = g_strconcat ("joystick ", joy->id, " abs_", n, s, NULL);
        g_free (s);
        g_free (n);

        return value;
      }
    }
  }

  return value;
}
