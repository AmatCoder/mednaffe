/*
 * medirange.c
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


#define IMED_RANGE_GET_INTERFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), imed_range_get_type (), IMedRangeInterface))

G_DEFINE_INTERFACE (IMedRange, imed_range, G_TYPE_OBJECT);


const gchar*
imed_range_medrange_get_value (IMedRange* self)
{
  g_return_val_if_fail (self != NULL, NULL);

  return IMED_RANGE_GET_INTERFACE (self)->medrange_get_value (self);
}


void
imed_range_medrange_set_value (IMedRange* self,
                               const gchar* value)
{
  g_return_if_fail (self != NULL);

  IMED_RANGE_GET_INTERFACE (self)->medrange_set_value (self, value);
}


static void
imed_range_default_init (IMedRangeInterface* iface)
{
}
