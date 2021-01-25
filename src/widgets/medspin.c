/*
 * medspin.c
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
#include "medspin.h"


typedef struct _MedSpinClass MedSpinClass;
typedef struct _MedSpinPrivate MedSpinPrivate;

struct _MedSpinClass {
  GtkSpinButtonClass parent_class;
};

struct _MedSpinPrivate {
  gchar* value;
};


static void med_spin_imed_range_interface_init (IMedRangeInterface* iface);

G_DEFINE_TYPE_WITH_CODE (MedSpin, med_spin, GTK_TYPE_SPIN_BUTTON, G_ADD_PRIVATE (MedSpin)
                         G_IMPLEMENT_INTERFACE (imed_range_get_type(), med_spin_imed_range_interface_init));


static gdouble
double_parse (const gchar* str)
{
  g_return_val_if_fail (str != NULL, 0.0);

  return g_ascii_strtod (str, NULL);
}


static void
med_spin_real_medrange_set_value (IMedRange* base,
                                  const gchar* value)
{
  g_return_if_fail (value != NULL);

  MedSpin* self = (MedSpin*) base;
  gtk_spin_button_set_value ((GtkSpinButton*) self, double_parse (value));
}


static const gchar*
med_spin_real_medrange_get_value (IMedRange* base)
{
  MedSpin* self = (MedSpin*) base;
  MedSpinPrivate* priv = med_spin_get_instance_private (self);

  gdouble value = gtk_spin_button_get_value ((GtkSpinButton*) self);
  g_free (priv->value);
  priv->value = g_strdup_printf ("%.*f", gtk_spin_button_get_digits ((GtkSpinButton*) self), value);

  return priv->value;
}


static void
main_spin_finalize (GObject* obj)
{
  MedSpin * self = G_TYPE_CHECK_INSTANCE_CAST (obj, med_spin_get_type(), MedSpin);
  MedSpinPrivate* priv = med_spin_get_instance_private (self);

  g_free (priv->value);

  G_OBJECT_CLASS (med_spin_parent_class)->finalize (obj);
}


MedSpin*
med_spin_new (GtkAdjustment* adjustment,
              gint digits)
{
  MedSpin * self = (MedSpin*) g_object_new (med_spin_get_type(), NULL);
  MedSpinPrivate* priv = med_spin_get_instance_private (self);

  gtk_spin_button_set_adjustment ((GtkSpinButton*) self, adjustment);
  gtk_spin_button_set_digits ((GtkSpinButton*) self, digits);

  priv->value = g_strdup ("");

  return self;
}


static void
med_spin_init (MedSpin* self)
{
}


static void
med_spin_class_init (MedSpinClass* klass)
{
  G_OBJECT_CLASS (klass)->finalize = main_spin_finalize;
}


static void
med_spin_imed_range_interface_init (IMedRangeInterface* iface)
{
  iface->medrange_set_value = (void (*) (IMedRange*, const gchar*)) med_spin_real_medrange_set_value;
  iface->medrange_get_value = (const gchar* (*) (IMedRange*)) med_spin_real_medrange_get_value;
}
