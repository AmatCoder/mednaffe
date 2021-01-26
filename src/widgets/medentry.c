/*
 * medentry.c
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


#include "medentry.h"


typedef struct _MedEntryPrivate MedEntryPrivate;

struct _MedEntryPrivate {
  GtkLabel* entry_label;
  gchar* _command;
  gboolean _updated;
  gboolean _modified;
  gchar* _label;
  gchar* internal_value;
};


enum  {
  MED_ENTRY_0_PROPERTY,
  MED_ENTRY_COMMAND_PROPERTY,
  MED_ENTRY_LABEL_PROPERTY,
  MED_ENTRY_LABELWIDTH_PROPERTY,
  MED_ENTRY_NUM_PROPERTIES
};

static GParamSpec* med_entry_properties[MED_ENTRY_NUM_PROPERTIES];


static void med_entry_med_widget_interface_init (MedWidgetInterface* iface);

G_DEFINE_TYPE_WITH_CODE (MedEntry, med_entry, GTK_TYPE_BOX, G_ADD_PRIVATE (MedEntry)
                         G_IMPLEMENT_INTERFACE (med_widget_get_type(), med_entry_med_widget_interface_init));


static void
med_entry_real_set_value (MedWidget* base,
                          const gchar* value)
{
  g_return_if_fail (value != NULL);

  MedEntry * self = (MedEntry*) base;
  gtk_entry_set_text (self->entry, value);
}


static const gchar*
med_entry_real_get_value (MedWidget* base)
{
  MedEntry * self = (MedEntry*) base;
  MedEntryPrivate* priv = med_entry_get_instance_private (self);

  g_free (priv->internal_value);

  priv->internal_value = g_strdup (gtk_entry_get_text (self->entry));

  return priv->internal_value;
}


static void
med_entry_real_entry_changed (MedEntry* self)
{
  med_widget_set_modified ((MedWidget*) self, TRUE);
}


static const gchar*
med_entry_real_get_command (MedWidget* base)
{

  MedEntry* self = (MedEntry*) base;
  MedEntryPrivate* priv = med_entry_get_instance_private (self);

  return priv->_command;
}


static void
med_entry_real_set_command (MedWidget* base,
                            const gchar* value)
{
  MedEntry* self = (MedEntry*) base;
  MedEntryPrivate* priv = med_entry_get_instance_private (self);

  if (g_strcmp0 (value, med_entry_real_get_command (base)) != 0)
  {
    g_free (priv->_command);
    priv->_command = g_strdup (value);

    g_object_notify_by_pspec ((GObject*) self, med_entry_properties[MED_ENTRY_COMMAND_PROPERTY]);
  }
}


static gboolean
med_entry_real_get_updated (MedWidget* base)
{
  MedEntry* self = (MedEntry*) base;
  MedEntryPrivate* priv = med_entry_get_instance_private (self);

  return priv->_updated;
}


static void
med_entry_real_set_updated (MedWidget* base,
                            gboolean value)
{
  MedEntry* self = (MedEntry*) base;
  MedEntryPrivate* priv = med_entry_get_instance_private (self);

  priv->_updated = value;
}


static gboolean
med_entry_real_get_modified (MedWidget* base)
{
  MedEntry* self = (MedEntry*) base;
  MedEntryPrivate* priv = med_entry_get_instance_private (self);

  return priv->_modified;
}


static void
med_entry_real_set_modified (MedWidget* base,
                             gboolean value)
{
  MedEntry* self = (MedEntry*) base;
  MedEntryPrivate* priv = med_entry_get_instance_private (self);

  priv->_modified = value;
}


static const gchar*
med_entry_get_label (MedEntry* self)
{
  g_return_val_if_fail (self != NULL, NULL);

  MedEntryPrivate* priv = med_entry_get_instance_private (self);

  return priv->_label;
}


static void
med_entry_set_label (MedEntry* self,
                     const gchar* value)
{
  g_return_if_fail (self != NULL);
  MedEntryPrivate* priv = med_entry_get_instance_private (self);

  if (g_strcmp0 (value, med_entry_get_label (self)) != 0)
  {
    g_free (priv->_label);
    priv->_label = g_strdup (value);

    g_object_notify_by_pspec ((GObject*) self, med_entry_properties[MED_ENTRY_LABEL_PROPERTY]);
  }
}


static void
med_entry_set_label_width (MedEntry* self,
                           gint value)
{
  g_return_if_fail (self != NULL);

  MedEntryPrivate* priv = med_entry_get_instance_private (self);
  g_object_set ((GtkWidget*) priv->entry_label, "width-chars", value, NULL);
}


static void
med_entry_changed (GtkEditable* sender,
                   gpointer self)
{
  g_return_if_fail (self != NULL);

  MedEntryClass* klass = (G_TYPE_INSTANCE_GET_CLASS ((self), med_entry_get_type(), MedEntryClass));

  klass->entry_changed (self);
}


static void
med_entry_finalize (GObject* obj)
{
  MedEntry * self = G_TYPE_CHECK_INSTANCE_CAST (obj, med_entry_get_type(), MedEntry);
  MedEntryPrivate* priv = med_entry_get_instance_private (self);

  g_free (priv->_command);
  g_free (priv->_label);
  g_free (priv->internal_value);

  G_OBJECT_CLASS (med_entry_parent_class)->finalize (obj);
}


MedEntry*
med_entry_construct (GType object_type)
{
  MedEntry* self = (MedEntry*) g_object_new (object_type, NULL);
  return self;
}


MedEntry*
med_entry_new (void)
{
  return med_entry_construct (med_entry_get_type());
}


static GObject*
med_entry_constructor (GType type,
                       guint n_construct_properties,
                       GObjectConstructParam* construct_properties)
{
  GObjectClass* parent_class = G_OBJECT_CLASS (med_entry_parent_class);
  GObject* obj = parent_class->constructor (type, n_construct_properties, construct_properties);

  MedEntry* self = G_TYPE_CHECK_INSTANCE_CAST (obj, med_entry_get_type(), MedEntry);
  MedEntryPrivate* priv = med_entry_get_instance_private (self);

  self->entry = (GtkEntry*) gtk_entry_new();

  priv->entry_label = (GtkLabel*) gtk_label_new (NULL);
  gtk_label_set_markup (priv->entry_label, priv->_label);
  gtk_label_set_xalign (priv->entry_label, 1.00);

  gtk_box_pack_start ((GtkBox*) self, (GtkWidget*) priv->entry_label, FALSE, FALSE, 0);
  gtk_box_pack_start ((GtkBox*) self, (GtkWidget*) self->entry, FALSE, FALSE, 12);

  gtk_widget_show ((GtkWidget*) priv->entry_label);
  gtk_widget_show ((GtkWidget*) self->entry);

  g_signal_connect_object ((GtkEditable*) self->entry,
                           "changed",
                           (GCallback) med_entry_changed,
                           self,
                           0);

  med_widget_init ((MedWidget*) self, (GtkWidget*) self);

  return obj;
}


static void
med_entry_init (MedEntry* self)
{
  MedEntryPrivate* priv = med_entry_get_instance_private (self);

  priv->internal_value = g_strdup (" ");
}


static void
med_entry_get_property (GObject* object,
                        guint property_id,
                        GValue* value,
                        GParamSpec* pspec)
{
  MedEntry * self = G_TYPE_CHECK_INSTANCE_CAST (object, med_entry_get_type(), MedEntry);

  switch (property_id)
  {
    case MED_ENTRY_COMMAND_PROPERTY:
      g_value_set_string (value, med_widget_get_command ((MedWidget*) self));
    break;
    case MED_ENTRY_LABEL_PROPERTY:
      g_value_set_string (value, med_entry_get_label (self));
    break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
med_entry_set_property (GObject* object,
                        guint property_id,
                        const GValue* value,
                        GParamSpec* pspec)
{
  MedEntry* self = G_TYPE_CHECK_INSTANCE_CAST (object, med_entry_get_type(), MedEntry);

  switch (property_id)
  {
    case MED_ENTRY_COMMAND_PROPERTY:
      med_widget_set_command ((MedWidget*) self, g_value_get_string (value));
    break;
    case MED_ENTRY_LABEL_PROPERTY:
      med_entry_set_label (self, g_value_get_string (value));
    break;
    case MED_ENTRY_LABELWIDTH_PROPERTY:
      med_entry_set_label_width (self, g_value_get_int (value));
    break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
med_entry_class_init (MedEntryClass* klass)
{
  ((MedEntryClass*) klass)->entry_changed = (void (*) (MedEntry*)) med_entry_real_entry_changed;

  G_OBJECT_CLASS (klass)->get_property = med_entry_get_property;
  G_OBJECT_CLASS (klass)->set_property = med_entry_set_property;
  G_OBJECT_CLASS (klass)->constructor = med_entry_constructor;
  G_OBJECT_CLASS (klass)->finalize = med_entry_finalize;

  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_ENTRY_COMMAND_PROPERTY,
                                   med_entry_properties[MED_ENTRY_COMMAND_PROPERTY] = g_param_spec_string
                                   (
                                     "command",
                                     "command",
                                     "command",
                                     NULL,
                                     G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE
                                   ));

  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_ENTRY_LABEL_PROPERTY,
                                   med_entry_properties[MED_ENTRY_LABEL_PROPERTY] = g_param_spec_string
                                   (
                                     "label",
                                     "label",
                                     "label",
                                     NULL,
                                     G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT
                                   ));

  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_ENTRY_LABELWIDTH_PROPERTY,
                                   med_entry_properties[MED_ENTRY_LABELWIDTH_PROPERTY] = g_param_spec_int
                                   (
                                    "label-width",
                                    "label-width",
                                    "label-width",
                                    G_MININT, G_MAXINT, 0,
                                    G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE
                                   ));
}


static void
med_entry_med_widget_interface_init (MedWidgetInterface* iface)
{
  iface->set_value = (void (*) (MedWidget*, const gchar*)) med_entry_real_set_value;
  iface->get_value = (const gchar* (*) (MedWidget*)) med_entry_real_get_value;

  iface->set_modified = (void (*) (MedWidget*, gboolean)) med_entry_real_set_modified;
  iface->get_modified = (gboolean (*) (MedWidget*)) med_entry_real_get_modified;

  iface->set_updated = (void (*) (MedWidget*, gboolean)) med_entry_real_set_updated;
  iface->get_updated = (gboolean (*) (MedWidget*)) med_entry_real_get_updated;

  iface->get_command = med_entry_real_get_command;
  iface->set_command = med_entry_real_set_command;
}
