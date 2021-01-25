/*
 * medscale.c
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


typedef struct _MedScaleClass MedScaleClass;
typedef struct _MedScalePrivate MedScalePrivate;

struct _MedScaleClass {
  GtkScaleClass parent_class;
};

struct _MedScalePrivate {
  gchar* value;
};


static void med_scale_imed_range_interface_init (IMedRangeInterface* iface);

G_DEFINE_TYPE_WITH_CODE (MedScale, med_scale, GTK_TYPE_SCALE, G_ADD_PRIVATE (MedScale)
                         G_IMPLEMENT_INTERFACE (imed_range_get_type(), med_scale_imed_range_interface_init));


static gdouble
double_parse (const gchar* str)
{
  g_return_val_if_fail (str != NULL, 0.0);

  return g_ascii_strtod (str, NULL);
}


static void
med_scale_real_medrange_set_value (IMedRange* base,
                                   const gchar* value)
{
  g_return_if_fail (value != NULL);

  MedScale * self = (MedScale*) base;
  gtk_range_set_value ((GtkRange*) self, double_parse (value));
}


static const gchar*
med_scale_real_medrange_get_value (IMedRange* base)
{
  MedScale* self = (MedScale*) base;
  MedScalePrivate* priv = med_scale_get_instance_private (self);

  g_free(priv->value);
  priv->value = g_strdup_printf ("%.*f", gtk_scale_get_digits ((GtkScale*) self), gtk_range_get_value ((GtkRange*) self));

  return priv->value;
}


static void
main_scale_finalize (GObject* obj)
{
  MedScale * self = G_TYPE_CHECK_INSTANCE_CAST (obj, med_scale_get_type(), MedScale);
  MedScalePrivate* priv = med_scale_get_instance_private (self);

  g_free (priv->value);

  G_OBJECT_CLASS (med_scale_parent_class)->finalize (obj);
}


MedScale*
med_scale_new (GtkAdjustment* adjustment,
               gint digits)
{
  MedScale * self = (MedScale*) g_object_new (med_scale_get_type(), NULL);
  MedScalePrivate* priv = med_scale_get_instance_private (self);

  gtk_range_set_adjustment ((GtkRange*) self, adjustment);
  gtk_scale_set_digits ((GtkScale*) self, digits);
  g_object_set ((GtkWidget*) self, "width-request", 300, NULL);
  gtk_scale_set_value_pos ((GtkScale*) self, GTK_POS_LEFT);

  priv->value = g_strdup ("");

  return self;
}


static void
med_scale_init (MedScale* self)
{
}


static void
med_scale_class_init (MedScaleClass* klass)
{
  G_OBJECT_CLASS (klass)->finalize = main_scale_finalize;
}


static void
med_scale_imed_range_interface_init (IMedRangeInterface* iface)
{
  iface->medrange_set_value = (void (*) (IMedRange*, const gchar*)) med_scale_real_medrange_set_value;
  iface->medrange_get_value = (const gchar* (*) (IMedRange*)) med_scale_real_medrange_get_value;
}
