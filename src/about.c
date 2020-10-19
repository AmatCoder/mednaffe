/*
 * about.c
 *
 * Copyright 2013-2020 AmatCoder
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


#include "about.h"


typedef struct _AboutWindowClass AboutWindowClass;

struct _AboutWindowClass
{
  GtkAboutDialogClass parent_class;
};


G_DEFINE_TYPE (AboutWindow, about_window, GTK_TYPE_ABOUT_DIALOG);


static void
dialog_response (GtkDialog* sender,
                 gint response_id,
                 gpointer user_data)
{
  if (response_id == -4)
    gtk_widget_destroy ((GtkWidget*) sender);
}


AboutWindow*
about_window_new (GtkWindow* parent)
{
  AboutWindow* self = (AboutWindow*) g_object_new (about_window_get_type (),  NULL);

  gtk_window_set_transient_for ((GtkWindow*) self, parent);
  gtk_window_set_destroy_with_parent ((GtkWindow*) self, TRUE);
  gtk_window_set_modal ((GtkWindow*) self, TRUE);

  GObject *app = (GObject*) gtk_window_get_application (parent);

  gtk_about_dialog_set_logo ((GtkAboutDialog*) self, g_object_get_data (app, "icon"));

  const char *authors[2] = { "AmatCoder", NULL };
  gtk_about_dialog_set_authors ((GtkAboutDialog*) self, authors);

  gtk_about_dialog_set_program_name ((GtkAboutDialog*) self, g_object_get_data (app, "name"));
  gtk_about_dialog_set_comments ((GtkAboutDialog*) self, g_object_get_data (app, "desc"));
  gtk_about_dialog_set_copyright ((GtkAboutDialog*) self, g_object_get_data (app, "copyright"));
  gtk_about_dialog_set_version ((GtkAboutDialog*) self, g_object_get_data (app, "version"));
  gtk_about_dialog_set_website ((GtkAboutDialog*) self, g_object_get_data (app, "url"));
  gtk_about_dialog_set_website_label ((GtkAboutDialog*) self, g_object_get_data (app, "url"));
  gtk_about_dialog_set_license_type ((GtkAboutDialog*) self, GTK_LICENSE_GPL_3_0);
  gtk_about_dialog_set_wrap_license ((GtkAboutDialog*) self, TRUE);

  g_signal_connect((GtkDialog*) self, "response", G_CALLBACK (dialog_response), NULL);

  return self;
}


static void
about_window_init (AboutWindow* self)
{
}

static void
about_window_class_init (AboutWindowClass* klass)
{
}
