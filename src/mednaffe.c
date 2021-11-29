/*
 * mednaffe.c
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


#include "mainwindow.h"


typedef struct _MednaffeApp MednaffeApp;
typedef struct _MednaffeAppClass MednaffeAppClass;

struct _MednaffeApp {
  GtkApplication parent_instance;
};

struct _MednaffeAppClass {
  GtkApplicationClass parent_class;
};


G_DEFINE_TYPE (MednaffeApp, mednaffe_app, GTK_TYPE_APPLICATION);


MednaffeApp*
mednaffe_app_new (void)
{
  MednaffeApp * self = (MednaffeApp*) g_object_new (mednaffe_app_get_type(),
                                                    "application-id",
                                                    "com.github.AmatCoder.mednaffe",
                                                    "flags",
                                                    G_APPLICATION_FLAGS_NONE,
                                                    NULL);
  return self;
}


static void
mednaffe_app_real_activate (GApplication* base)
{
  MainWindow* main_window = main_window_new ((GtkApplication*) base);
  main_window_start (main_window);
}


static void
mednaffe_app_init (MednaffeApp * self)
{
}


static void
mednaffe_app_class_init (MednaffeAppClass * klass)
{
  ((GApplicationClass *) klass)->activate = (void (*) (GApplication *)) mednaffe_app_real_activate;
}


int
main (int argc,
      char ** argv)
{
  MednaffeApp* mednaffe_app = mednaffe_app_new ();

  g_object_set_data_full ((GObject*) mednaffe_app, "name", g_strdup("Mednaffe"), g_free);
  g_object_set_data_full ((GObject*) mednaffe_app, "desc", g_strdup("A front-end (GUI) for mednafen emulator"), g_free);
  g_object_set_data_full ((GObject*) mednaffe_app, "copyright", g_strdup("Copyright © 2013-2021 AmatCoder"), g_free);
  g_object_set_data_full ((GObject*) mednaffe_app, "url", g_strdup("https://github.com/AmatCoder/mednaffe"), g_free);
  g_object_set_data_full ((GObject*) mednaffe_app, "version", g_strdup("0.9.2"), g_free);

  GList *icon_list = NULL;

  GdkPixbuf* icon = gdk_pixbuf_new_from_resource ("/com/github/mednaffe/mednaffe.png", NULL);
  icon_list = g_list_append (icon_list, icon);

  GdkPixbuf* icon16 = gdk_pixbuf_new_from_resource ("/com/github/mednaffe/mednaffe16.png", NULL);
  icon_list = g_list_append (icon_list, icon16);

  GdkPixbuf* icon32 = gdk_pixbuf_new_from_resource ("/com/github/mednaffe/mednaffe32.png", NULL);
  icon_list = g_list_append (icon_list, icon32);

  GdkPixbuf* icon48 = gdk_pixbuf_new_from_resource ("/com/github/mednaffe/mednaffe48.png", NULL);
  icon_list = g_list_append (icon_list, icon48);

  g_object_set_data ((GObject*) mednaffe_app, "icon_list", icon_list);


  gint result = g_application_run ((GApplication*) mednaffe_app, argc, argv);


  g_list_free_full (icon_list, (GDestroyNotify) g_object_unref);
  g_object_unref (mednaffe_app);

  return result;
}
