/*
 * menuinput.c
 *
 * Copyright 2013-20121 AmatCoder
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


#include "menuinput.h"


typedef struct _MenuInputClass MenuInputClass;

struct _MenuInputClass {
  GtkMenuClass parent_class;
};


G_DEFINE_TYPE (MenuInput, menu_input, GTK_TYPE_MENU);


void
menu_input_enable_all (MenuInput* self,
                       gboolean b)
{
  g_return_if_fail (self != NULL);

  gtk_widget_set_sensitive ((GtkWidget*) self->clear, b);
  gtk_widget_set_sensitive ((GtkWidget*) self->or, b);
  gtk_widget_set_sensitive ((GtkWidget*) self->and, b);
  gtk_widget_set_sensitive ((GtkWidget*) self->and_not, b);
}


void
menu_input_enable_clear (MenuInput* self,
                         gboolean b)
{
  g_return_if_fail (self != NULL);

  gtk_widget_set_sensitive ((GtkWidget*) self->clear, b);
  gtk_widget_set_sensitive ((GtkWidget*) self->or, FALSE);
  gtk_widget_set_sensitive ((GtkWidget*) self->and, FALSE);
  gtk_widget_set_sensitive ((GtkWidget*) self->and_not, FALSE);
}


MenuInput*
menu_input_new (void)
{
  MenuInput* self = (MenuInput*) g_object_new (menu_input_get_type (), NULL);
  return self;
}


static GObject *
menu_input_constructor (GType type,
                        guint n_construct_properties,
                        GObjectConstructParam * construct_properties)
{
  GObjectClass* parent_class = G_OBJECT_CLASS (menu_input_parent_class);
  GObject* obj = parent_class->constructor (type, n_construct_properties, construct_properties);
  MenuInput* self = G_TYPE_CHECK_INSTANCE_CAST (obj, menu_input_get_type (), MenuInput);

  GtkWidget* separator = gtk_separator_menu_item_new ();

  self->clear = (GtkMenuItem*) gtk_menu_item_new_with_label ("Clear");
  self->or = (GtkMenuItem*) gtk_menu_item_new_with_label ("Add another input (OR)");
  self->and = (GtkMenuItem*) gtk_menu_item_new_with_label ("Add another input (AND)");
  self->and_not = (GtkMenuItem*) gtk_menu_item_new_with_label ("Add another input (AND NOT)");

  g_object_set_data_full ((GObject*) self->or, "menu_text", g_strdup(" or "), g_free);
  g_object_set_data_full ((GObject*) self->or, "menu_op", g_strdup(" || "), g_free);
  g_object_set_data_full ((GObject*) self->and, "menu_text", g_strdup(" and "), g_free);
  g_object_set_data_full ((GObject*) self->and, "menu_op", g_strdup(" && "), g_free);
  g_object_set_data_full ((GObject*) self->and_not, "menu_text", g_strdup(" and not "), g_free);
  g_object_set_data_full ((GObject*) self->and_not, "menu_op", g_strdup(" &! "), g_free);


  gtk_container_add ((GtkContainer*) self, (GtkWidget*) self->clear);
  gtk_container_add ((GtkContainer*) self, separator);
  gtk_container_add ((GtkContainer*) self, (GtkWidget*) self->or);
  gtk_container_add ((GtkContainer*) self, (GtkWidget*) self->and);
  gtk_container_add ((GtkContainer*) self, (GtkWidget*) self->and_not);

  gtk_widget_show_all ((GtkWidget*) self);

  return obj;
}


static void
menu_input_init (MenuInput* self)
{
}


static void
menu_input_class_init (MenuInputClass* klass)
{
  G_OBJECT_CLASS (klass)->constructor = menu_input_constructor;
}
