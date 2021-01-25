/*
 * medirange.h
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


#ifndef __MEDIRANGE_H__
#define __MEDIRANGE_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _IMedRange IMedRange;
typedef struct _IMedRangeInterface IMedRangeInterface;

struct _IMedRangeInterface
{
  GTypeInterface parent_iface;
  const gchar* (*medrange_get_value) (IMedRange* self);
  void (*medrange_set_value) (IMedRange* self, const gchar* value);
};

GType imed_range_get_type (void) G_GNUC_CONST;
const gchar* imed_range_medrange_get_value (IMedRange* self);
void imed_range_medrange_set_value (IMedRange* self, const gchar* value);

G_END_DECLS

#endif
