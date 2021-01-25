/*
 * medcolorentry.c
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


#include <string.h>
#include "medcolorentry.h"


typedef struct _MedColorEntryClass MedColorEntryClass;
typedef struct _MedColorEntryPrivate MedColorEntryPrivate;


struct _MedColorEntryClass {
  MedEntryClass parent_class;
};

struct _MedColorEntryPrivate {
  GtkColorButton* colorbutton;
  gboolean _has_alpha;
  gboolean editing;
};


enum  {
  MED_COLOR_ENTRY_0_PROPERTY,
  MED_COLOR_ENTRY_HAS_ALPHA_PROPERTY,
  MED_COLOR_ENTRY_NUM_PROPERTIES
};
static GParamSpec* med_color_entry_properties[MED_COLOR_ENTRY_NUM_PROPERTIES];


G_DEFINE_TYPE_WITH_PRIVATE (MedColorEntry, med_color_entry, med_entry_get_type());


static gdouble
med_color_entry_value_to_alpha (const gchar* value)
{
  guint v = g_ascii_strtoull (value, NULL, 16);
  gdouble d = ((gdouble) v / (gdouble) 255);

  return d;
}


static gchar*
med_color_entry_text_to_rgb (const gchar* text)
{
  int p = 0;
  gchar a[3] = {'0', '0', 0};
  gchar r[3] = {'0', '0', 0};
  gchar g[3] = {'0', '0', 0};
  gchar b[3] = {'0', '0', 0};

  if (strlen (text) == 8)
  {
    a[0] = 'F';
    a[1] = 'F';
  }
  else if (strlen (text) == 10)
  {
    a[0] = text[2];
    a[1] = text[3];
    p = 2;
  }

  r[0] = text[p+2];
  r[1] = text[p+3];
  g[0] = text[p+4];
  g[1] = text[p+5];
  b[0] = text[p+6];
  b[1] = text[p+7];

  guint nr = g_ascii_strtoull(r, NULL, 16);
  guint ng = g_ascii_strtoull(g, NULL, 16);
  guint nb = g_ascii_strtoull(b, NULL, 16);

  return g_strdup_printf ("rgba(%u,%u,%u,%f)",
                          nr,
                          ng,
                          nb,
                          med_color_entry_value_to_alpha (a));
}


static void
med_color_entry_real_entry_changed (MedEntry* base)
{
  MedColorEntry* self = (MedColorEntry*) base;
  MedColorEntryPrivate* priv = med_color_entry_get_instance_private (self);

  MED_ENTRY_CLASS (med_color_entry_parent_class)->entry_changed (G_TYPE_CHECK_INSTANCE_CAST (self, med_entry_get_type(), MedEntry));

  if (priv->editing)
    return;

  gchar* color;
  GdkRGBA rgba;

  const gchar* text = gtk_entry_get_text (((MedEntry*) self)->entry);

  if ((text[0] == '0') && ((text[1] == 'x') || (text[1] == 'X')))
    color = med_color_entry_text_to_rgb (text);
  else
    color = g_strdup ("rgba(0,0,0,0.0)");

  gdk_rgba_parse (&rgba, color);

  g_free (color);

  gtk_color_chooser_set_rgba ((GtkColorChooser*) priv->colorbutton, &rgba);
}


MedColorEntry*
med_color_entry_new (void)
{
  MedColorEntry * self = (MedColorEntry*) med_entry_construct (med_color_entry_get_type());

  return self;
}


static void
med_color_entry_colorbutton_clicked (GtkColorButton* sender,
                                     gpointer self)
{
  g_return_if_fail (self != NULL);

  gchar* text;
  GdkRGBA* rgba;

  MedColorEntry* ce = self;
  MedColorEntryPrivate* priv = med_color_entry_get_instance_private (ce);

  g_object_get (priv->colorbutton, "rgba", &rgba, NULL);

  if (priv->_has_alpha)
    text = g_strdup_printf ("0x%02x%02x%02x%02x", (guint)(rgba->alpha * 255),
                                                  (guint)(rgba->red * 255),
                                                  (guint)(rgba->green * 255),
                                                  (guint)(rgba->blue * 255));
  else
    text = g_strdup_printf ("0x%02x%02x%02x", (guint)(rgba->red * 255),
                                              (guint)(rgba->green * 255),
                                              (guint)(rgba->blue * 255));

  priv->editing = TRUE;
  gtk_entry_set_text (((MedEntry*) self)->entry, text);
  priv->editing = FALSE;

  g_free (text);
}


gboolean
med_color_entry_get_has_alpha (MedColorEntry* self)
{
  g_return_val_if_fail (self != NULL, FALSE);

  MedColorEntryPrivate* priv = med_color_entry_get_instance_private (self);

  return priv->_has_alpha;
}


static void
med_color_entry_set_has_alpha (MedColorEntry* self,
                               gboolean value)
{
  g_return_if_fail (self != NULL);

  MedColorEntryPrivate* priv = med_color_entry_get_instance_private (self);

  if (med_color_entry_get_has_alpha (self) != value)
  {
    priv->_has_alpha = value;
    gtk_color_chooser_set_use_alpha ((GtkColorChooser*) priv->colorbutton, value);
    g_object_notify_by_pspec ((GObject*) self, med_color_entry_properties[MED_COLOR_ENTRY_HAS_ALPHA_PROPERTY]);
  }
}


static GObject*
med_color_entry_constructor (GType type,
                             guint n_construct_properties,
                             GObjectConstructParam* construct_properties)
{
  GObjectClass* parent_class = G_OBJECT_CLASS (med_color_entry_parent_class);
  GObject* obj = parent_class->constructor (type, n_construct_properties, construct_properties);

  MedColorEntry*  self = G_TYPE_CHECK_INSTANCE_CAST (obj, med_color_entry_get_type(), MedColorEntry);
  MedColorEntryPrivate* priv = med_color_entry_get_instance_private (self);

  priv->colorbutton = (GtkColorButton*) gtk_color_button_new();

  g_signal_connect_object (priv->colorbutton, "color-set", (GCallback) med_color_entry_colorbutton_clicked, self, 0);

  gtk_box_pack_start ((GtkBox*) self, (GtkWidget*) priv->colorbutton, FALSE, FALSE, 0);
  gtk_widget_show ((GtkWidget*) priv->colorbutton);

  return obj;
}


static void
med_color_entry_init (MedColorEntry* self)
{
  MedColorEntryPrivate* priv = med_color_entry_get_instance_private (self);
  priv->_has_alpha = FALSE;
  priv->editing = FALSE;
}


static void
med_color_entry_get_property (GObject* object,
                              guint property_id,
                              GValue* value,
                              GParamSpec* pspec)
{
  MedColorEntry* self = G_TYPE_CHECK_INSTANCE_CAST (object, med_color_entry_get_type(), MedColorEntry);

  switch (property_id)
  {
    case MED_COLOR_ENTRY_HAS_ALPHA_PROPERTY:
      g_value_set_boolean (value, med_color_entry_get_has_alpha (self));
    break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
med_color_entry_set_property (GObject* object,
                              guint property_id,
                              const GValue* value,
                              GParamSpec* pspec)
{
  MedColorEntry* self = G_TYPE_CHECK_INSTANCE_CAST (object, med_color_entry_get_type(), MedColorEntry);

  switch (property_id)
  {
    case MED_COLOR_ENTRY_HAS_ALPHA_PROPERTY:
      med_color_entry_set_has_alpha (self, g_value_get_boolean (value));
    break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
med_color_entry_class_init (MedColorEntryClass* klass)
{
  ((MedEntryClass *) klass)->entry_changed = (void (*) (MedEntry*)) med_color_entry_real_entry_changed;

  G_OBJECT_CLASS (klass)->constructor = med_color_entry_constructor;
  G_OBJECT_CLASS (klass)->get_property = med_color_entry_get_property;
  G_OBJECT_CLASS (klass)->set_property = med_color_entry_set_property;


  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_COLOR_ENTRY_HAS_ALPHA_PROPERTY,
                                   med_color_entry_properties[MED_COLOR_ENTRY_HAS_ALPHA_PROPERTY] = g_param_spec_boolean
                                   (
                                     "has-alpha",
                                     "has-alpha",
                                     "has-alpha",
                                     FALSE,
                                     G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE
                                   ));
}
