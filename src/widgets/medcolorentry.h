/*
 * medcolorentry.h
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


#ifndef __MEDCOLORENTRY_H__
#define __MEDCOLORENTRY_H__

#include <gtk/gtk.h>

#include "medentry.h"

G_BEGIN_DECLS

typedef struct _MedColorEntry MedColorEntry;

struct _MedColorEntry {
  MedEntry parent_instance;
};


GType med_color_entry_get_type (void) G_GNUC_CONST;
MedColorEntry* med_color_entry_new (void);


G_END_DECLS

#endif
