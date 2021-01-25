/*
 * medlistjoy.h
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


#ifndef __MEDLISTJOY_H__
#define __MEDLISTJOY_H__

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _MedListJoy MedListJoy;

struct _MedListJoy {
  GObject parent_instance;
};

GType med_list_joy_get_type (void) G_GNUC_CONST;
MedListJoy* med_list_joy_new (void);

void med_list_joy_init_list_joy (MedListJoy* self);
void med_list_joy_enable_all (MedListJoy* self, gboolean enable);
gchar* med_list_joy_value_to_text (MedListJoy* self, const gchar* value);

G_END_DECLS

#endif
