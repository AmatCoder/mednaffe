/*
 * meddialogentry.c
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


#include "meddialogentry.h"
#include "widgets/dialogs.h"


typedef struct _MedDialogEntryPrivate MedDialogEntryPrivate;

struct _MedDialogEntryPrivate {
  GtkButton* button;
  gboolean _is_folder;
};


enum  {
  MED_DIALOG_ENTRY_0_PROPERTY,
  MED_DIALOG_ENTRY_IS_FOLDER_PROPERTY,
  MED_DIALOG_ENTRY_NUM_PROPERTIES
};
static GParamSpec* med_dialog_entry_properties[MED_DIALOG_ENTRY_NUM_PROPERTIES];

enum  {
  MED_DIALOG_ENTRY_EMIT_CHANGE_SIGNAL,
  MED_DIALOG_ENTRY_NUM_SIGNALS
};
static guint med_dialog_entry_signals[MED_DIALOG_ENTRY_NUM_SIGNALS] = {0};


G_DEFINE_TYPE_WITH_PRIVATE (MedDialogEntry, med_dialog_entry, med_entry_get_type());


static void
med_dialog_entry_real_entry_changed (MedEntry* base)
{
  MedDialogEntry* self = (MedDialogEntry*) base;

  MED_ENTRY_CLASS(med_dialog_entry_parent_class)->entry_changed (G_TYPE_CHECK_INSTANCE_CAST (self, med_entry_get_type(), MedEntry));

  g_signal_emit (self, med_dialog_entry_signals[MED_DIALOG_ENTRY_EMIT_CHANGE_SIGNAL], 0);
}


gboolean
med_dialog_entry_get_is_folder (MedDialogEntry* self)
{
  g_return_val_if_fail (self != NULL, FALSE);

  MedDialogEntryPrivate* priv = med_dialog_entry_get_instance_private (self);

  return priv->_is_folder;
}


static void
med_dialog_entry_set_is_folder (MedDialogEntry* self,
                                gboolean value)
{
  g_return_if_fail (self != NULL);

  MedDialogEntryPrivate* priv = med_dialog_entry_get_instance_private (self);

  if (med_dialog_entry_get_is_folder (self) != value)
  {
    priv->_is_folder = value;
    g_object_notify_by_pspec ((GObject*) self, med_dialog_entry_properties[MED_DIALOG_ENTRY_IS_FOLDER_PROPERTY]);
  }
}


static void
med_dialog_entry_button_clicked (GtkButton* sender,
                                 gpointer self)
{
  g_return_if_fail (self != NULL);

  MedDialogEntry* de = self;
  MedDialogEntryPrivate* priv = med_dialog_entry_get_instance_private (de);

  GtkWidget* parent = gtk_widget_get_toplevel ((GtkWidget*) de);

  gchar* file = select_path (parent, priv->_is_folder);

  if (file)
  {
    gtk_entry_set_text (((MedEntry*) de)->entry, file);
    g_free (file);
  }
}


MedDialogEntry*
med_dialog_entry_construct (GType object_type)
{
  MedDialogEntry* self = (MedDialogEntry*) med_entry_construct (object_type);
  return self;
}


MedDialogEntry*
med_dialog_entry_new (void)
{
  return med_dialog_entry_construct (med_dialog_entry_get_type());
}


static GObject*
med_dialog_entry_constructor (GType type,
                              guint n_construct_properties,
                              GObjectConstructParam* construct_properties)
{
  GObjectClass* parent_class = G_OBJECT_CLASS (med_dialog_entry_parent_class);
  GObject* obj = parent_class->constructor (type, n_construct_properties, construct_properties);

  MedDialogEntry* self = G_TYPE_CHECK_INSTANCE_CAST (obj, med_dialog_entry_get_type(), MedDialogEntry);
  MedDialogEntryPrivate* priv = med_dialog_entry_get_instance_private (self);

  gtk_box_set_child_packing ((GtkBox*) self, (GtkWidget*) ((MedEntry*) self)->entry, TRUE, TRUE, 16, GTK_PACK_START);

  priv->button = (GtkButton*) gtk_button_new_from_icon_name ("folder-open", (GtkIconSize) GTK_ICON_SIZE_BUTTON);
  gtk_box_pack_start ((GtkBox*) self, (GtkWidget*) priv->button, FALSE, FALSE, 0);

  g_signal_connect_object (priv->button, "clicked", (GCallback) med_dialog_entry_button_clicked, self, 0);

  gtk_widget_show ((GtkWidget*) priv->button);

  return obj;
}


static void
med_dialog_entry_init (MedDialogEntry* self)
{
  MedDialogEntryPrivate* priv = med_dialog_entry_get_instance_private (self);
  priv->_is_folder = FALSE;
}


static void
med_dialog_entry_get_property (GObject* object,
                                guint property_id,
                                GValue* value,
                                GParamSpec* pspec)
{
  MedDialogEntry* self = G_TYPE_CHECK_INSTANCE_CAST (object, med_dialog_entry_get_type(), MedDialogEntry);

  switch (property_id)
  {
    case MED_DIALOG_ENTRY_IS_FOLDER_PROPERTY:
      g_value_set_boolean (value, med_dialog_entry_get_is_folder (self));
    break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
med_dialog_entry_set_property (GObject* object,
                               guint property_id,
                               const GValue* value,
                               GParamSpec* pspec)
{
  MedDialogEntry* self = G_TYPE_CHECK_INSTANCE_CAST (object, med_dialog_entry_get_type(), MedDialogEntry);

  switch (property_id)
  {
    case MED_DIALOG_ENTRY_IS_FOLDER_PROPERTY:
      med_dialog_entry_set_is_folder (self, g_value_get_boolean (value));
    break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
med_dialog_entry_class_init (MedDialogEntryClass* klass)
{
  ((MedEntryClass*) klass)->entry_changed = (void (*) (MedEntry*)) med_dialog_entry_real_entry_changed;

  G_OBJECT_CLASS (klass)->get_property = med_dialog_entry_get_property;
  G_OBJECT_CLASS (klass)->set_property = med_dialog_entry_set_property;
  G_OBJECT_CLASS (klass)->constructor = med_dialog_entry_constructor;

  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_DIALOG_ENTRY_IS_FOLDER_PROPERTY,
                                   med_dialog_entry_properties[MED_DIALOG_ENTRY_IS_FOLDER_PROPERTY] = g_param_spec_boolean
                                   (
                                     "is-folder",
                                     "is-folder",
                                     "is-folder",
                                     FALSE,
                                     G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE
                                   ));


  med_dialog_entry_signals[MED_DIALOG_ENTRY_EMIT_CHANGE_SIGNAL] = g_signal_new ("emit-change",
                                                                                med_dialog_entry_get_type(),
                                                                                G_SIGNAL_RUN_LAST,
                                                                                0,
                                                                                NULL,
                                                                                NULL,
                                                                                g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
                                                                                0);
}
