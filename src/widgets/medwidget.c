/*
 * medwidget.c
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


#include "medwidget.h"


#define MED_WIDGET_GET_INTERFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), med_widget_get_type (), MedWidgetInterface))


G_DEFINE_INTERFACE (MedWidget, med_widget, G_TYPE_OBJECT);


const gchar*
med_widget_get_value (MedWidget* self)
{
  g_return_val_if_fail (self != NULL, NULL);

  return MED_WIDGET_GET_INTERFACE (self)->get_value (self);
}


void
med_widget_set_value (MedWidget* self,
                      const gchar* v)
{
  g_return_if_fail (self != NULL);

  MED_WIDGET_GET_INTERFACE (self)->set_value (self, v);
}


static void
med_widget_real_init (MedWidget* self,
                      GtkWidget* widget)
{
  g_return_if_fail (widget != NULL);

  med_widget_set_updated ((MedWidget*) widget, TRUE);
  med_widget_set_modified ((MedWidget*) widget, FALSE);
}


static void
med_widget_realize (GtkWidget* widget,
                    gpointer data)
{
  g_return_if_fail (widget != NULL);

  GtkWidget *toplevel = gtk_widget_get_toplevel (widget);
  if (toplevel)
  {
    GHashTable* table = g_object_get_data ((GObject*) toplevel, "table");

    if (table)
    {
      gchar* value;
      const gchar* command = med_widget_get_command ((MedWidget*) widget);
      gchar* command2 = g_strconcat ("-", command, NULL);

      if (g_hash_table_contains (table, command2))
        value = g_hash_table_lookup (table, command2);
      else
        value = g_hash_table_lookup (table, command);

      g_free (command2);

      if (value)
      {
        med_widget_set_value ((MedWidget*) widget, value);
        med_widget_set_updated ((MedWidget*) widget, FALSE);
      }
      else
        gtk_widget_set_sensitive (widget, FALSE);
    }
  }
}


void
med_widget_init (MedWidget* self,
                 GtkWidget* widget)
{
  g_return_if_fail (self != NULL);

  MED_WIDGET_GET_INTERFACE (self)->init (self, widget);
  g_signal_connect_object ((GtkWidget*) self, "realize", (GCallback) med_widget_realize, self, 0);
}


const gchar*
med_widget_get_command (MedWidget* self)
{
  g_return_val_if_fail (self != NULL, NULL);

  return MED_WIDGET_GET_INTERFACE (self)->get_command (self);
}


void
med_widget_set_command (MedWidget* self,
                        const gchar* value)
{
  g_return_if_fail (self != NULL);

  MED_WIDGET_GET_INTERFACE (self)->set_command (self, value);
}


gboolean
med_widget_get_updated (MedWidget* self)
{
  g_return_val_if_fail (self != NULL, FALSE);

  return MED_WIDGET_GET_INTERFACE (self)->get_updated (self);
}


void
med_widget_set_updated (MedWidget* self,
                        gboolean value)
{
  g_return_if_fail (self != NULL);
  MED_WIDGET_GET_INTERFACE (self)->set_updated (self, value);
}

gboolean
med_widget_get_modified (MedWidget* self)
{
  g_return_val_if_fail (self != NULL, FALSE);
  return MED_WIDGET_GET_INTERFACE (self)->get_modified (self);
}


void
med_widget_set_modified (MedWidget* self,
                        gboolean value)
{
  g_return_if_fail (self != NULL);

  MED_WIDGET_GET_INTERFACE (self)->set_modified (self, value);

  if (med_widget_get_updated(self))
    return;

  GtkWidget *toplevel = gtk_widget_get_toplevel ((GtkWidget*) self);
  if (toplevel)
  {
    GHashTable* table = g_object_get_data ((GObject*) toplevel, "table");

    if (table)
    {
      gchar* key = g_strconcat("-", med_widget_get_command (self), NULL);
      g_hash_table_insert (table, key, g_strdup(med_widget_get_value (self)));
    }
  }
}


static void
med_widget_default_init (MedWidgetInterface* iface)
{
  g_object_interface_install_property (iface,
                                       g_param_spec_string ("command",
                                                            "command",
                                                            "command",
                                                            NULL,
                                                            G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE));

  iface->init = med_widget_real_init;
}
