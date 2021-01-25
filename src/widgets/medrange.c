/*
 * medrange.c
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


#include "medirange.h"
#include "medscale.h"
#include "medspin.h"
#include "medrange.h"


typedef struct _MedRangeClass MedRangeClass;
typedef struct _MedRangePrivate MedRangePrivate;

struct _MedRangeClass {
  GtkBoxClass parent_class;
};

struct _MedRangePrivate {
  GtkWidget* s_widget;
  GtkLabel* s_label;
  gchar* _command;
  gboolean _updated;
  gboolean _modified;
  gchar* _label;
  gchar* _values;
  gboolean _is_scale;
  GtkAdjustment* adj;
};


enum  {
  MED_RANGE_0_PROPERTY,
  MED_RANGE_COMMAND_PROPERTY,
  MED_RANGE_LABEL_PROPERTY,
  MED_RANGE_LABELWIDTH_PROPERTY,
  MED_RANGE_VALUES_PROPERTY,
  MED_RANGE_IS_SCALE_PROPERTY,
  MED_RANGE_NUM_PROPERTIES
};

static GParamSpec* med_range_properties[MED_RANGE_NUM_PROPERTIES];


static void med_range_med_widget_interface_init (MedWidgetInterface* iface);

G_DEFINE_TYPE_WITH_CODE (MedRange, med_range, GTK_TYPE_BOX, G_ADD_PRIVATE (MedRange)
                         G_IMPLEMENT_INTERFACE (med_widget_get_type(), med_range_med_widget_interface_init));


static void
med_range_real_set_value (MedWidget* base,
                          const gchar* value)
{
  g_return_if_fail (value != NULL);

  MedRange * self = (MedRange*) base;
  MedRangePrivate* priv = med_range_get_instance_private (self);

  imed_range_medrange_set_value ((IMedRange*) priv->s_widget, value);
}


static const gchar*
med_range_real_get_value (MedWidget* base)
{
  MedRange* self = (MedRange*) base;
  MedRangePrivate* priv = med_range_get_instance_private (self);

  return imed_range_medrange_get_value ((IMedRange*) priv->s_widget);
}


static void
med_range_range_changed (MedRange* self)
{
  g_return_if_fail (self != NULL);

  med_widget_set_modified ((MedWidget*) self, TRUE);
}


static const gchar*
med_range_real_get_command (MedWidget* base)
{
  MedRange* self = (MedRange*) base;
  MedRangePrivate* priv = med_range_get_instance_private (self);

  return priv->_command;
}


static void
med_range_real_set_command (MedWidget* base,
                            const gchar* value)
{
  MedRange* self = (MedRange*) base;
  MedRangePrivate* priv = med_range_get_instance_private (self);

  if (g_strcmp0 (value, med_range_real_get_command (base)) != 0)
  {
    g_free (priv->_command);
    priv->_command = g_strdup (value);

    g_object_notify_by_pspec ((GObject*) self, med_range_properties[MED_RANGE_COMMAND_PROPERTY]);
  }
}


static gboolean
med_range_real_get_updated (MedWidget* base)
{
  MedRange* self = (MedRange*) base;
  MedRangePrivate* priv = med_range_get_instance_private (self);

  return priv->_updated;
}


static void
med_range_real_set_updated (MedWidget* base,
                            gboolean value)
{
  MedRange* self = (MedRange*) base;
  MedRangePrivate* priv = med_range_get_instance_private (self);

  priv->_updated = value;
}


static gboolean
med_range_real_get_modified (MedWidget* base)
{
  MedRange* self = (MedRange*) base;
  MedRangePrivate* priv = med_range_get_instance_private (self);

  return priv->_modified;
}


static void
med_range_real_set_modified (MedWidget* base,
                            gboolean value)
{
  MedRange* self = (MedRange*) base;
  MedRangePrivate* priv = med_range_get_instance_private (self);

  priv->_modified = value;
}


static const gchar*
med_range_get_label (MedRange* self)
{
  g_return_val_if_fail (self != NULL, NULL);

  MedRangePrivate* priv = med_range_get_instance_private (self);
  return priv->_label;
}


static void
med_range_set_label (MedRange* self,
                     const gchar* value)
{
  g_return_if_fail (self != NULL);

  MedRangePrivate* priv = med_range_get_instance_private (self);

  if (g_strcmp0 (value, med_range_get_label (self)) != 0)
  {
    g_free (priv->_label);
    priv->_label = g_strdup (value);

    g_object_notify_by_pspec ((GObject*) self, med_range_properties[MED_RANGE_LABEL_PROPERTY]);
  }
}


static void
med_range_set_label_width (MedRange* self,
                           gint value)
{
  g_return_if_fail (self != NULL);

  MedRangePrivate* priv = med_range_get_instance_private (self);
  g_object_set ((GtkWidget*) priv->s_label, "width-chars", value, NULL);
}


static const gchar*
med_range_get_values (MedRange* self)
{
  g_return_val_if_fail (self != NULL, NULL);

  MedRangePrivate* priv = med_range_get_instance_private (self);
  return priv->_values;
}


static void
med_range_set_values (MedRange* self,
                      const gchar* value)
{
  g_return_if_fail (self != NULL);

  MedRangePrivate* priv = med_range_get_instance_private (self);

  if (g_strcmp0 (value, med_range_get_values (self)) != 0)
  {
    g_free (priv->_values);
    priv->_values = g_strdup (value);

    g_object_notify_by_pspec ((GObject*) self, med_range_properties[MED_RANGE_VALUES_PROPERTY]);
  }
}


static gboolean
med_range_get_is_scale (MedRange* self)
{
  g_return_val_if_fail (self != NULL, FALSE);

  MedRangePrivate* priv = med_range_get_instance_private (self);
  return priv->_is_scale;
}


static void
med_range_set_is_scale (MedRange* self,
                        gboolean value)
{
  g_return_if_fail (self != NULL);
  MedRangePrivate* priv = med_range_get_instance_private (self);

  if (med_range_get_is_scale (self) != value)
  {
    priv->_is_scale = value;
    g_object_notify_by_pspec ((GObject*) self, med_range_properties[MED_RANGE_IS_SCALE_PROPERTY]);
  }
}


static void
med_range_range_value_changed (GtkRange* sender,
                               gpointer self)
{
  med_range_range_changed ((MedRange*) self);
}


static void
med_range_spin_button_value_changed (GtkSpinButton* sender,
                                     gpointer self)
{
  med_range_range_changed ((MedRange*) self);
}


static void
med_range_finalize (GObject* obj)
{
  MedRange* self = G_TYPE_CHECK_INSTANCE_CAST (obj, med_range_get_type(), MedRange);
  MedRangePrivate* priv = med_range_get_instance_private (self);

  g_free (priv->_command);
  g_free (priv->_label);
  g_free (priv->_values);

  G_OBJECT_CLASS (med_range_parent_class)->finalize (obj);
}


MedRange*
med_range_new (void)
{
  MedRange* self = (MedRange*) g_object_new (med_range_get_type(), NULL);
  return self;
}


static GObject*
med_range_constructor (GType type,
                       guint n_construct_properties,
                       GObjectConstructParam* construct_properties)
{
  GObjectClass* parent_class = G_OBJECT_CLASS (med_range_parent_class);
  GObject* obj = parent_class->constructor (type, n_construct_properties, construct_properties);

  MedRange* self = G_TYPE_CHECK_INSTANCE_CAST (obj, med_range_get_type(), MedRange);
  MedRangePrivate* priv = med_range_get_instance_private (self);

  if (priv->_values == NULL)
    priv->_values = g_strdup ("0,0,0,0");

  gchar** vs = g_strsplit (priv->_values, ",", 0);

  priv->adj = gtk_adjustment_new (0,
                                  g_ascii_strtod (vs[0], NULL),
                                  g_ascii_strtod (vs[1], NULL),
                                  g_ascii_strtod (vs[2], NULL),
                                  0,
                                  0);

  gint v3 = g_ascii_strtoll (vs[3], NULL, 0);

  if (priv->_is_scale)
  {
    priv->s_widget = (GtkWidget*) med_scale_new (priv->adj, v3);
    g_signal_connect_object ((GtkRange*) priv->s_widget, "value-changed", (GCallback) med_range_range_value_changed, self, 0);
    gtk_scale_set_value_pos ((GtkScale*) priv->s_widget, GTK_POS_RIGHT);
    g_object_set ((GtkWidget*) priv->s_widget, "width-request", 216, NULL);
  }
  else
  {
    priv->s_widget = (GtkWidget*) med_spin_new (priv->adj, v3);
    g_signal_connect_object ((GtkSpinButton*) priv->s_widget, "value-changed", (GCallback) med_range_spin_button_value_changed, self, 0);
    g_object_set ((GtkWidget*) priv->s_widget, "width-request", 168, NULL);
  }

  priv->s_label = (GtkLabel*) gtk_label_new (priv->_label);
  gtk_label_set_xalign (priv->s_label, 1.00);

  gtk_box_pack_start ((GtkBox*) self, (GtkWidget*) priv->s_label, FALSE, FALSE, 0);

  if (priv->_is_scale)
    gtk_box_pack_start ((GtkBox*) self, priv->s_widget, FALSE, FALSE, 0);
  else
    gtk_box_pack_start ((GtkBox*) self, priv->s_widget, FALSE, FALSE, 12);

  gtk_widget_show ((GtkWidget*) priv->s_label);
  gtk_widget_show (priv->s_widget);

  med_widget_init ((MedWidget*) self, (GtkWidget*) self);

  g_strfreev (vs);

  return obj;
}


static void
med_range_init (MedRange* self)
{
  MedRangePrivate* priv = med_range_get_instance_private (self);
  priv->_is_scale = FALSE;
}


static void
med_range_get_property (GObject* object,
                        guint property_id,
                        GValue* value,
                        GParamSpec* pspec)
{
  MedRange* self = G_TYPE_CHECK_INSTANCE_CAST (object, med_range_get_type(), MedRange);

  switch (property_id)
  {
    case MED_RANGE_COMMAND_PROPERTY:
      g_value_set_string (value, med_widget_get_command ((MedWidget*) self));
    break;
    case MED_RANGE_LABEL_PROPERTY:
      g_value_set_string (value, med_range_get_label (self));
    break;
    case MED_RANGE_VALUES_PROPERTY:
      g_value_set_string (value, med_range_get_values (self));
    break;
    case MED_RANGE_IS_SCALE_PROPERTY:
      g_value_set_boolean (value, med_range_get_is_scale (self));
    break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
med_range_set_property (GObject* object,
                        guint property_id,
                        const GValue * value,
                        GParamSpec * pspec)
{
  MedRange* self = G_TYPE_CHECK_INSTANCE_CAST (object, med_range_get_type(), MedRange);

  switch (property_id)
  {
    case MED_RANGE_COMMAND_PROPERTY:
      med_widget_set_command ((MedWidget*) self, g_value_get_string (value));
    break;
    case MED_RANGE_LABEL_PROPERTY:
      med_range_set_label (self, g_value_get_string (value));
    break;
    case MED_RANGE_LABELWIDTH_PROPERTY:
      med_range_set_label_width (self, g_value_get_int (value));
    break;
    case MED_RANGE_VALUES_PROPERTY:
      med_range_set_values (self, g_value_get_string (value));
    break;
    case MED_RANGE_IS_SCALE_PROPERTY:
      med_range_set_is_scale (self, g_value_get_boolean (value));
    break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
med_range_class_init (MedRangeClass* klass)
{
  G_OBJECT_CLASS (klass)->get_property = med_range_get_property;
  G_OBJECT_CLASS (klass)->set_property = med_range_set_property;
  G_OBJECT_CLASS (klass)->constructor = med_range_constructor;
  G_OBJECT_CLASS (klass)->finalize = med_range_finalize;

  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_RANGE_COMMAND_PROPERTY,
                                   med_range_properties[MED_RANGE_COMMAND_PROPERTY] = g_param_spec_string
                                   (
                                     "command",
                                     "command",
                                     "command",
                                     NULL,
                                     G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE
                                   ));

  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_RANGE_LABEL_PROPERTY,
                                   med_range_properties[MED_RANGE_LABEL_PROPERTY] = g_param_spec_string
                                   (
                                     "label",
                                     "label",
                                     "label",
                                     NULL,
                                     G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT
                                   ));

  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_RANGE_VALUES_PROPERTY,
                                   med_range_properties[MED_RANGE_VALUES_PROPERTY] = g_param_spec_string
                                   (
                                     "values",
                                     "values",
                                     "values",
                                     NULL,
                                     G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT
                                   ));

  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_RANGE_IS_SCALE_PROPERTY,
                                   med_range_properties[MED_RANGE_IS_SCALE_PROPERTY] = g_param_spec_boolean
                                   (
                                     "is-scale",
                                     "is-scale",
                                     "is-scale",
                                     FALSE,
                                     G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT
                                   ));

  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_RANGE_LABELWIDTH_PROPERTY,
                                   med_range_properties[MED_RANGE_LABELWIDTH_PROPERTY] = g_param_spec_int
                                   (
                                    "label-width",
                                    "label-width",
                                    "label-width",
                                    G_MININT, G_MAXINT, 0,
                                    G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE
                                   ));

}


static void
med_range_med_widget_interface_init (MedWidgetInterface* iface)
{
  iface->set_value = (void (*) (MedWidget*, const gchar*)) med_range_real_set_value;
  iface->get_value = (const gchar* (*) (MedWidget*)) med_range_real_get_value;

  iface->set_modified = (void (*) (MedWidget*, gboolean)) med_range_real_set_modified;
  iface->get_modified = (gboolean (*) (MedWidget*)) med_range_real_get_modified;

  iface->set_updated = (void (*) (MedWidget*, gboolean)) med_range_real_set_updated;
  iface->get_updated = (gboolean (*) (MedWidget*)) med_range_real_get_updated;

  iface->get_command = med_range_real_get_command;
  iface->set_command = med_range_real_set_command;
}
