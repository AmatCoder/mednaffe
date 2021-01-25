/*
 * medcheckbutton.c
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


#include "medcheckbutton.h"


typedef struct _MedCheckButtonClass MedCheckButtonClass;
typedef struct _MedCheckButtonPrivate MedCheckButtonPrivate;

struct _MedCheckButtonClass {
  GtkCheckButtonClass parent_class;
};

struct _MedCheckButtonPrivate {
  gchar* _command;
  gboolean _updated;
  gboolean _modified;
};


enum  {
  MED_CHECK_BUTTON_0_PROPERTY,
  MED_CHECK_BUTTON_COMMAND_PROPERTY,
  MED_CHECK_BUTTON_NUM_PROPERTIES
};

static GParamSpec* med_check_button_properties[MED_CHECK_BUTTON_NUM_PROPERTIES];


static void med_check_button_med_widget_interface_init (MedWidgetInterface* iface);

G_DEFINE_TYPE_WITH_CODE (MedCheckButton, med_check_button,GTK_TYPE_CHECK_BUTTON, G_ADD_PRIVATE (MedCheckButton)
                         G_IMPLEMENT_INTERFACE (med_widget_get_type(), med_check_button_med_widget_interface_init));


static void
med_check_button_real_set_value (MedWidget* base,
                                 const gchar* value)
{
  g_return_if_fail (value != NULL);

  if (g_strcmp0 (value, "1") == 0)
    gtk_toggle_button_set_active ((GtkToggleButton*) base, TRUE);
  else
    gtk_toggle_button_set_active ((GtkToggleButton*) base, FALSE);
}


static const gchar*
med_check_button_real_get_value (MedWidget* base)
{
  if (gtk_toggle_button_get_active ((GtkToggleButton*) base))
    return "1";
  else
    return "0";
}


static void
med_check_button_toggled (GtkToggleButton* sender,
                          gpointer self)
{
  g_return_if_fail (self != NULL);

  med_widget_set_modified ((MedWidget*) self, TRUE);
}


static const gchar*
med_check_button_real_get_command (MedWidget* base)
{
  MedCheckButton* self = (MedCheckButton*) base;
  MedCheckButtonPrivate* priv = med_check_button_get_instance_private(self);

  return priv->_command;
}


static void
med_check_button_real_set_command (MedWidget* base,
                                   const gchar* value)
{
  MedCheckButton* self = (MedCheckButton*) base;
  MedCheckButtonPrivate* priv = med_check_button_get_instance_private(self);

  if (g_strcmp0 (value, med_check_button_real_get_command (base)) != 0)
  {
    g_free (priv->_command);
    priv->_command = g_strdup (value);

    g_object_notify_by_pspec ((GObject*) self, med_check_button_properties[MED_CHECK_BUTTON_COMMAND_PROPERTY]);
  }
}


static gboolean
med_check_button_real_get_updated (MedWidget* base)
{
  MedCheckButton* self = (MedCheckButton*) base;
  MedCheckButtonPrivate* priv = med_check_button_get_instance_private(self);

  return priv->_updated;
}


static void
med_check_button_real_set_updated (MedWidget* base,
                                   gboolean value)
{
  MedCheckButton* self = (MedCheckButton*) base;
  MedCheckButtonPrivate* priv = med_check_button_get_instance_private(self);

  priv->_updated = value;
}

static gboolean
med_check_button_real_get_modified (MedWidget* base)
{
  MedCheckButton* self = (MedCheckButton*) base;
  MedCheckButtonPrivate* priv = med_check_button_get_instance_private(self);

  return priv->_modified;
}


static void
med_check_button_real_set_modified (MedWidget* base,
                                   gboolean value)
{
  MedCheckButton* self = (MedCheckButton*) base;
  MedCheckButtonPrivate* priv = med_check_button_get_instance_private(self);

  priv->_modified = value;
}


static void
med_check_button_finalize (GObject* obj)
{
  MedCheckButton* self = G_TYPE_CHECK_INSTANCE_CAST (obj, med_check_button_get_type(), MedCheckButton);
  MedCheckButtonPrivate* priv = med_check_button_get_instance_private(self);

  g_free (priv->_command);

  G_OBJECT_CLASS (med_check_button_parent_class)->finalize (obj);
}


static GObject*
med_check_button_constructor (GType type,
                              guint n_construct_properties,
                              GObjectConstructParam* construct_properties)
{
  GObjectClass* parent_class = G_OBJECT_CLASS (med_check_button_parent_class);
  GObject* obj = parent_class->constructor (type, n_construct_properties, construct_properties);

  MedCheckButton* self = G_TYPE_CHECK_INSTANCE_CAST (obj, med_check_button_get_type(), MedCheckButton);

  g_signal_connect_object ((GtkToggleButton*) self,
                           "toggled",
                           (GCallback) med_check_button_toggled, self, 0);

  med_widget_init ((MedWidget*) self, (GtkWidget*) self);

  return obj;
}


MedCheckButton*
med_check_button_new (void)
{
  return (MedCheckButton*) g_object_new (med_check_button_get_type(), NULL);
}


static void
med_check_button_init (MedCheckButton* self)
{
}


static void
med_check_button_get_property (GObject* object,
                               guint property_id,
                               GValue* value,
                               GParamSpec* pspec)
{
  MedCheckButton* self = G_TYPE_CHECK_INSTANCE_CAST (object, med_check_button_get_type(), MedCheckButton);

  switch (property_id)
  {
    case MED_CHECK_BUTTON_COMMAND_PROPERTY:
      g_value_set_string (value, med_widget_get_command ((MedWidget*) self));
    break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
med_check_button_set_property (GObject* object,
                               guint property_id,
                               const GValue* value,
                               GParamSpec* pspec)
{
  MedCheckButton* self = G_TYPE_CHECK_INSTANCE_CAST (object, med_check_button_get_type(), MedCheckButton);

  switch (property_id)
  {
    case MED_CHECK_BUTTON_COMMAND_PROPERTY:
      med_widget_set_command ((MedWidget*) self, g_value_get_string (value));
    break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
med_check_button_class_init (MedCheckButtonClass* klass)
{
  G_OBJECT_CLASS (klass)->get_property = med_check_button_get_property;
  G_OBJECT_CLASS (klass)->set_property = med_check_button_set_property;
  G_OBJECT_CLASS (klass)->constructor = med_check_button_constructor;
  G_OBJECT_CLASS (klass)->finalize = med_check_button_finalize;

  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_CHECK_BUTTON_COMMAND_PROPERTY,
                                   med_check_button_properties[MED_CHECK_BUTTON_COMMAND_PROPERTY] = g_param_spec_string
                                   (
                                     "command",
                                     "command",
                                     "command",
                                     NULL,
                                     G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE
                                   ));
}


static void
med_check_button_med_widget_interface_init (MedWidgetInterface* iface)
{
  iface->set_value = (void (*) (MedWidget*, const gchar*)) med_check_button_real_set_value;
  iface->get_value = (const gchar* (*) (MedWidget*)) med_check_button_real_get_value;

  iface->set_modified = (void (*) (MedWidget*, gboolean)) med_check_button_real_set_modified;
  iface->get_modified = (gboolean (*) (MedWidget*)) med_check_button_real_get_modified;

  iface->set_updated  = (void (*) (MedWidget*, gboolean)) med_check_button_real_set_updated;
  iface->get_updated  = (gboolean (*) (MedWidget*)) med_check_button_real_get_updated;

  iface->get_command = med_check_button_real_get_command;
  iface->set_command = med_check_button_real_set_command;
}
