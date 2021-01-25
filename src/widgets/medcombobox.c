/*
 * medcombobox.c
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


#include "medcombobox.h"


typedef struct _MedComboBoxClass MedComboBoxClass;
typedef struct _MedComboBoxPrivate MedComboBoxPrivate;

struct _MedComboBoxClass {
  GtkBoxClass parent_class;
};

struct _MedComboBoxPrivate {
  GtkComboBoxText* combo;
  GtkLabel* combo_label;
  gchar* _command;
  gboolean _updated;
  gboolean _modified;
  gchar* _label;
  gchar** _values;
  gchar* value;
};


enum  {
  MED_COMBO_BOX_0_PROPERTY,
  MED_COMBO_BOX_COMMAND_PROPERTY,
  MED_COMBO_BOX_LABEL_PROPERTY,
  MED_COMBO_BOX_LABELWIDTH_PROPERTY,
  MED_COMBO_BOX_VALUES_PROPERTY,
  MED_COMBO_BOX_NUM_PROPERTIES
};

static GParamSpec* med_combo_box_properties[MED_COMBO_BOX_NUM_PROPERTIES];


enum  {
  MED_COMBO_BOX_CHANGED_SIGNAL,
  MED_COMBO_BOX_NUM_SIGNALS
};

static guint med_combo_box_signals[MED_COMBO_BOX_NUM_SIGNALS] = {0};


static void med_combo_box_med_widget_interface_init (MedWidgetInterface* iface);

G_DEFINE_TYPE_WITH_CODE (MedComboBox, med_combo_box, GTK_TYPE_BOX, G_ADD_PRIVATE (MedComboBox)
                         G_IMPLEMENT_INTERFACE (med_widget_get_type(), med_combo_box_med_widget_interface_init));


static void
med_combo_box_real_set_value (MedWidget* base,
                              const gchar* value)
{
  g_return_if_fail (value != NULL);

  MedComboBox* self = (MedComboBox*) base;
  MedComboBoxPrivate* priv = med_combo_box_get_instance_private (self);

  GtkTreeModel* medcombobox_store = gtk_combo_box_get_model ((GtkComboBox*) priv->combo);

  GtkTreeIter iter;
  gboolean valid = gtk_tree_model_get_iter_first (medcombobox_store, &iter);

  gchar* item = NULL;

  while (valid)
  {
    g_free (item);
    gtk_tree_model_get (medcombobox_store, &iter, 0, &item, -1);

    if (g_strcmp0 (item, value) == 0)
    {
      gtk_combo_box_set_active_iter ((GtkComboBox*) priv->combo, &iter);
      g_free (item);
      break;
    }

    valid = gtk_tree_model_iter_next (medcombobox_store, &iter);
  }
}


static const gchar*
med_combo_box_real_get_value (MedWidget* base)
{
  MedComboBox* self = (MedComboBox*) base;
  MedComboBoxPrivate* priv = med_combo_box_get_instance_private (self);

  g_free(priv->value);
  priv->value = gtk_combo_box_text_get_active_text (priv->combo);

  return priv->value;
}


static const gchar*
med_combo_box_real_get_command (MedWidget* base)
{
  MedComboBox* self = (MedComboBox*) base;
  MedComboBoxPrivate* priv = med_combo_box_get_instance_private (self);

  return priv->_command;
}


static void
med_combo_box_real_set_command (MedWidget* base,
                                const gchar* value)
{
  MedComboBox* self = (MedComboBox*) base;
  MedComboBoxPrivate* priv = med_combo_box_get_instance_private (self);

  if (g_strcmp0 (value, med_combo_box_real_get_command (base)) != 0)
  {
    g_free (priv->_command);
    priv->_command = g_strdup (value);

    g_object_notify_by_pspec ((GObject*) self, med_combo_box_properties[MED_COMBO_BOX_COMMAND_PROPERTY]);
  }
}


static gboolean
med_combo_box_real_get_updated (MedWidget* base)
{
  MedComboBox* self = (MedComboBox*) base;
  MedComboBoxPrivate* priv = med_combo_box_get_instance_private (self);

  return priv->_updated;
}


static void
med_combo_box_real_set_updated (MedWidget* base,
                                gboolean value)
{
  MedComboBox* self = (MedComboBox*) base;
  MedComboBoxPrivate* priv = med_combo_box_get_instance_private (self);

  priv->_updated = value;
}


static gboolean
med_combo_box_real_get_modified (MedWidget* base)
{
  MedComboBox* self = (MedComboBox*) base;
  MedComboBoxPrivate* priv = med_combo_box_get_instance_private(self);

  return priv->_modified;
}


static void
med_combo_box_real_set_modified (MedWidget* base,
                                   gboolean value)
{
  MedComboBox* self = (MedComboBox*) base;
  MedComboBoxPrivate* priv = med_combo_box_get_instance_private(self);

  priv->_modified = value;
}


static const gchar*
med_combo_box_get_label (MedComboBox* self)
{
  g_return_val_if_fail (self != NULL, NULL);

  MedComboBoxPrivate* priv = med_combo_box_get_instance_private (self);

  return priv->_label;
}


static void
med_combo_box_set_label (MedComboBox* self,
                         const gchar* value)
{
  g_return_if_fail (self != NULL);

  MedComboBoxPrivate* priv = med_combo_box_get_instance_private (self);

  if (g_strcmp0 (value, priv->_label) != 0)
  {
    g_free (priv->_label);
    priv->_label = g_strdup (value);

    g_object_notify_by_pspec ((GObject*) self, med_combo_box_properties[MED_COMBO_BOX_LABEL_PROPERTY]);
  }
}


static void
med_combo_box_set_label_width (MedComboBox* self,
                               gint value)
{
  g_return_if_fail (self != NULL);

  MedComboBoxPrivate* priv = med_combo_box_get_instance_private (self);
  g_object_set ((GtkWidget*) priv->combo_label, "width-chars", value, NULL);
}


static gchar**
med_combo_box_get_values (MedComboBox* self)
{
  g_return_val_if_fail (self != NULL, NULL);

  MedComboBoxPrivate* priv = med_combo_box_get_instance_private (self);

  return priv->_values;
}

static void
med_combo_box_set_values (MedComboBox* self,
                          gchar** value)
{
  g_return_if_fail (self != NULL);

  MedComboBoxPrivate* priv = med_combo_box_get_instance_private (self);

  if (med_combo_box_get_values (self) != value)
  {
    g_strfreev(priv->_values);
    priv->_values  = g_strdupv (value);

    g_object_notify_by_pspec ((GObject*) self, med_combo_box_properties[MED_COMBO_BOX_VALUES_PROPERTY]);
  }
}


static void
med_combo_box_changed (GtkComboBox* sender,
                       gpointer self)
{
  g_return_if_fail (self != NULL);

  MedComboBoxPrivate* priv = med_combo_box_get_instance_private (self);

  gchar* text = gtk_combo_box_text_get_active_text (priv->combo);

  med_widget_set_modified ((MedWidget*) self, TRUE);

  g_signal_emit (self,
                 med_combo_box_signals[MED_COMBO_BOX_CHANGED_SIGNAL],
                 0,
                 text);

  g_free(text);
}


static void
med_combo_box_finalize (GObject* obj)
{
  MedComboBox * self = G_TYPE_CHECK_INSTANCE_CAST (obj, med_combo_box_get_type(), MedComboBox);
  MedComboBoxPrivate* priv = med_combo_box_get_instance_private (self);

  g_free (priv->_command);
  g_free (priv->_label);
  g_free(priv->value);
  g_strfreev(priv->_values);

  G_OBJECT_CLASS (med_combo_box_parent_class)->finalize (obj);
}


MedComboBox*
med_combo_box_new (void)
{
  MedComboBox* self = g_object_new (med_combo_box_get_type(), NULL);
  return self;
}


static GObject*
med_combo_box_constructor (GType type,
                           guint n_construct_properties,
                           GObjectConstructParam* construct_properties)
{
  GObjectClass* parent_class = G_OBJECT_CLASS (med_combo_box_parent_class);
  GObject* obj = parent_class->constructor (type, n_construct_properties, construct_properties);

  MedComboBox* self = G_TYPE_CHECK_INSTANCE_CAST (obj, med_combo_box_get_type(), MedComboBox);
  MedComboBoxPrivate* priv = med_combo_box_get_instance_private (self);

  gtk_orientable_set_orientation ((GtkOrientable*) self, GTK_ORIENTATION_HORIZONTAL);

  priv->combo = (GtkComboBoxText*) gtk_combo_box_text_new();
  g_object_set ((GtkWidget*) priv->combo, "width-request", 168, NULL);

  priv->combo_label = (GtkLabel*) gtk_label_new (priv->_label);
  gtk_label_set_xalign (priv->combo_label, 1.00);

  gchar** collection = med_combo_box_get_values (self);

  if (collection)
  {
    gint it = 0;
    const gchar* str = collection[it];

    while (str)
    {
      gtk_combo_box_text_append_text (priv->combo, str);
      it++;
      str = collection[it];
    }
  }
  gtk_box_pack_start ((GtkBox*) self, (GtkWidget*) priv->combo_label, FALSE, FALSE, 0);
  gtk_box_pack_start ((GtkBox*) self, (GtkWidget*) priv->combo, FALSE, FALSE, 12);

  gtk_widget_show ((GtkWidget*) priv->combo_label);
  gtk_widget_show ((GtkWidget*) priv->combo);

  g_signal_connect_object ((GtkComboBox*) priv->combo,
                           "changed",
                           (GCallback) med_combo_box_changed, self, 0);

  priv->value = g_strdup ("");

  med_widget_init ((MedWidget*) self, (GtkWidget*) self);

  return obj;
}


static void
med_combo_box_init (MedComboBox* self)
{
}


static void
med_combo_box_get_property (GObject* object,
                            guint property_id,
                            GValue* value,
                            GParamSpec* pspec)
{
  MedComboBox* self = G_TYPE_CHECK_INSTANCE_CAST (object, med_combo_box_get_type(), MedComboBox);

  switch (property_id)
  {
    case MED_COMBO_BOX_COMMAND_PROPERTY:
      g_value_set_string (value, med_widget_get_command ((MedWidget*) self));
    break;
    case MED_COMBO_BOX_LABEL_PROPERTY:
      g_value_set_string (value, med_combo_box_get_label (self));
    break;
    case MED_COMBO_BOX_VALUES_PROPERTY:
      g_value_set_boxed (value, med_combo_box_get_values (self));
    break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
med_combo_box_set_property (GObject* object,
                             guint property_id,
                             const GValue* value,
                             GParamSpec* pspec)
{
  MedComboBox* self = G_TYPE_CHECK_INSTANCE_CAST (object, med_combo_box_get_type(), MedComboBox);

  switch (property_id)
  {
    case MED_COMBO_BOX_COMMAND_PROPERTY:
      med_widget_set_command ((MedWidget*) self, g_value_get_string (value));
    break;
    case MED_COMBO_BOX_LABEL_PROPERTY:
      med_combo_box_set_label (self, g_value_get_string (value));
    break;
    case MED_COMBO_BOX_LABELWIDTH_PROPERTY:
      med_combo_box_set_label_width (self, g_value_get_int (value));
    break;
    case MED_COMBO_BOX_VALUES_PROPERTY:
      med_combo_box_set_values (self, g_value_get_boxed (value));
    break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
med_combo_box_class_init (MedComboBoxClass* klass)
{
  G_OBJECT_CLASS (klass)->get_property = med_combo_box_get_property;
  G_OBJECT_CLASS (klass)->set_property = med_combo_box_set_property;
  G_OBJECT_CLASS (klass)->constructor = med_combo_box_constructor;
  G_OBJECT_CLASS (klass)->finalize = med_combo_box_finalize;

  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_COMBO_BOX_COMMAND_PROPERTY,
                                   med_combo_box_properties[MED_COMBO_BOX_COMMAND_PROPERTY] = g_param_spec_string
                                   (
                                     "command",
                                     "command",
                                     "command",
                                     NULL,
                                     G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE
                                   ));

  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_COMBO_BOX_LABEL_PROPERTY,
                                   med_combo_box_properties[MED_COMBO_BOX_LABEL_PROPERTY] = g_param_spec_string
                                   (
                                     "label",
                                     "label",
                                     "label",
                                     NULL,
                                     G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT
                                   ));

  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_COMBO_BOX_VALUES_PROPERTY,
                                   med_combo_box_properties[MED_COMBO_BOX_VALUES_PROPERTY] = g_param_spec_boxed
                                   (
                                     "values",
                                     "values",
                                     "values",
                                     G_TYPE_STRV,
                                     G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT
                                   ));

  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_COMBO_BOX_LABELWIDTH_PROPERTY,
                                   med_combo_box_properties[MED_COMBO_BOX_LABELWIDTH_PROPERTY] = g_param_spec_int
                                   (
                                    "label-width",
                                    "label-width",
                                    "label-width",
                                    G_MININT, G_MAXINT, 0,
                                    G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE
                                   ));

  med_combo_box_signals[MED_COMBO_BOX_CHANGED_SIGNAL] = g_signal_new ("medcombo_changed",
                                                                      med_combo_box_get_type(),
                                                                      G_SIGNAL_RUN_LAST,
                                                                      0,
                                                                      NULL,
                                                                      NULL,
                                                                      g_cclosure_marshal_VOID__STRING,
                                                                      G_TYPE_NONE,
                                                                      1,
                                                                      G_TYPE_STRING);
}


static void
med_combo_box_med_widget_interface_init (MedWidgetInterface* iface)
{
  iface->set_value = (void (*) (MedWidget*, const gchar*)) med_combo_box_real_set_value;
  iface->get_value = (const gchar* (*) (MedWidget*)) med_combo_box_real_get_value;

  iface->set_modified = (void (*) (MedWidget*, gboolean)) med_combo_box_real_set_modified;
  iface->get_modified = (gboolean (*) (MedWidget*)) med_combo_box_real_get_modified;

  iface->set_updated = (void (*) (MedWidget*, gboolean)) med_combo_box_real_set_updated;
  iface->get_updated = (gboolean (*) (MedWidget*)) med_combo_box_real_get_updated;

  iface->get_command = med_combo_box_real_get_command;
  iface->set_command = med_combo_box_real_set_command;
}
