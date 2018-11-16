/*
 * medbiosentry.c
 *
 * Copyright 2013-2018 AmatCoder
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


#include "medbiosentry.h"

typedef struct _MedBiosEntryClass MedBiosEntryClass;
typedef struct _MedBiosEntryPrivate MedBiosEntryPrivate;

struct _MedBiosEntryClass {
	MedDialogEntryClass parent_class;
};

struct _MedBiosEntryPrivate {
	gchar* _sha256;
};


enum  {
	MED_BIOS_ENTRY_0_PROPERTY,
	MED_BIOS_ENTRY_SHA256_PROPERTY,
	MED_BIOS_ENTRY_NUM_PROPERTIES
};

static GParamSpec* med_bios_entry_properties[MED_BIOS_ENTRY_NUM_PROPERTIES];


G_DEFINE_TYPE_WITH_PRIVATE (MedBiosEntry, med_bios_entry, med_dialog_entry_get_type ());


gchar*
med_bios_entry_get_tooltip (MedBiosEntry* self)
{
	g_return_val_if_fail (self != NULL, NULL);

  GtkWidget* widget = (GtkWidget*)((MedEntry*) self)->entry;

	return gtk_widget_get_tooltip_text (widget);
}


gchar*
med_bios_entry_get_icon (MedBiosEntry* self)
{
	g_return_val_if_fail (self != NULL, NULL);

	return g_strdup (gtk_entry_get_icon_name (((MedEntry*) self)->entry, GTK_ENTRY_ICON_SECONDARY));
}


void
med_bios_entry_update_check (MedBiosEntry* self,
                             const gchar* real_path)
{
	g_return_if_fail (self != NULL);
	g_return_if_fail (real_path != NULL);
  MedBiosEntryPrivate* priv = med_bios_entry_get_instance_private (self);

  GMappedFile* file;
  GBytes* data;
  gchar* cm;

  GtkEntry* entry = ((MedEntry*) self)->entry;

  file = g_mapped_file_new (real_path, FALSE, NULL);

  if (file == NULL)
  {
    gchar* info;

    gtk_entry_set_icon_from_icon_name (entry, GTK_ENTRY_ICON_SECONDARY, "gtk-cancel");
    info = g_strconcat ("Bios file not found!\n\nPath: ", real_path, NULL);
    gtk_widget_set_tooltip_text ((GtkWidget*) entry, info);
    g_free (info);
    return;
  }

  data = g_mapped_file_get_bytes (file);
  cm = g_compute_checksum_for_bytes (G_CHECKSUM_SHA256, data);

  if (g_strcmp0 (cm, priv->_sha256) == 0)
  {
    gtk_entry_set_icon_from_icon_name (entry, GTK_ENTRY_ICON_SECONDARY, "gtk-apply");

    gchar* info = g_strconcat ("Bios file is OK\n\nPath: ", real_path, "\n\nSha256 checksum:\n", priv->_sha256, NULL);
    gtk_widget_set_tooltip_text ((GtkWidget*) entry, info);

    g_free (info);

  }
  else
  {
    gtk_entry_set_icon_from_icon_name (entry, GTK_ENTRY_ICON_SECONDARY, "dialog-warning");

    gchar* info = g_strconcat ("Bios file is not correct!\n\nPath: ", real_path,
                               "\n\nSha256 checksum:\n", cm, "\nShould be:\n", priv->_sha256, NULL);

    gtk_widget_set_tooltip_text ((GtkWidget*) entry, info);

    g_free (info);
  }

  g_free (cm);
  g_bytes_unref (data);
  g_mapped_file_unref (file);
}


static const gchar*
med_bios_entry_get_sha256 (MedBiosEntry* self)
{
	g_return_val_if_fail (self != NULL, NULL);
  MedBiosEntryPrivate* priv = med_bios_entry_get_instance_private (self);

	return priv->_sha256;
}


static void
med_bios_entry_set_sha256 (MedBiosEntry* self,
                           const gchar* value)
{
	g_return_if_fail (self != NULL);
  MedBiosEntryPrivate* priv = med_bios_entry_get_instance_private (self);

	if (g_strcmp0 (value, med_bios_entry_get_sha256 (self)) != 0)
  {
		g_free (priv->_sha256);
		priv->_sha256 = g_strdup (value);

		g_object_notify_by_pspec ((GObject*) self, med_bios_entry_properties[MED_BIOS_ENTRY_SHA256_PROPERTY]);
	}
}


static void
med_bios_entry_finalize (GObject * obj)
{
	MedBiosEntry * self = G_TYPE_CHECK_INSTANCE_CAST (obj, med_bios_entry_get_type (), MedBiosEntry);
  MedBiosEntryPrivate* priv = med_bios_entry_get_instance_private (self);

	g_free (priv->_sha256);

	G_OBJECT_CLASS (med_bios_entry_parent_class)->finalize (obj);
}


MedBiosEntry*
med_bios_entry_new (void)
{
	MedBiosEntry * self = (MedBiosEntry*) med_dialog_entry_construct (med_bios_entry_get_type ());
	return self;
}


static void
med_bios_entry_init (MedBiosEntry * self)
{
}


static void
med_bios_entry_get_property (GObject * object,
                             guint property_id,
                             GValue * value,
                             GParamSpec * pspec)
{
	MedBiosEntry * self = G_TYPE_CHECK_INSTANCE_CAST (object, med_bios_entry_get_type (), MedBiosEntry);

	switch (property_id)
  {
		case MED_BIOS_ENTRY_SHA256_PROPERTY:
		  g_value_set_string (value, med_bios_entry_get_sha256 (self));
		break;
		default:
		  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static void
med_bios_entry_set_property (GObject * object,
                             guint property_id,
                             const GValue * value,
                             GParamSpec * pspec)
{
	MedBiosEntry * self = G_TYPE_CHECK_INSTANCE_CAST (object, med_bios_entry_get_type (), MedBiosEntry);

	switch (property_id)
  {
		case MED_BIOS_ENTRY_SHA256_PROPERTY:
		  med_bios_entry_set_sha256 (self, g_value_get_string (value));
		break;
		default:
		  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static void
med_bios_entry_class_init (MedBiosEntryClass * klass)
{
	G_OBJECT_CLASS (klass)->get_property = med_bios_entry_get_property;
	G_OBJECT_CLASS (klass)->set_property = med_bios_entry_set_property;
	G_OBJECT_CLASS (klass)->finalize = med_bios_entry_finalize;

	g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_BIOS_ENTRY_SHA256_PROPERTY,
                                   med_bios_entry_properties[MED_BIOS_ENTRY_SHA256_PROPERTY] = g_param_spec_string
                                  (
                                    "sha256",
                                    "sha256",
                                    "sha256",
                                    NULL,
                                    G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE
                                  ));
}
