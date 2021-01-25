/*
 * medentry.h
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


#ifndef __MEDENTRY_H__
#define __MEDENTRY_H__

#include <gtk/gtk.h>

#include "medwidget.h"

G_BEGIN_DECLS

#define MED_ENTRY_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), med_entry_get_type (), MedEntryClass))

typedef struct _MedEntry MedEntry;
typedef struct _MedEntryClass MedEntryClass;

struct _MedEntry {
  GtkBox parent_instance;
  GtkEntry* entry;
};

struct _MedEntryClass {
  GtkBoxClass parent_class;
  void (*entry_changed) (MedEntry* self);
};

GType med_entry_get_type (void) G_GNUC_CONST;
MedEntry* med_entry_new (void);
MedEntry* med_entry_construct (GType object_type);

G_END_DECLS

#endif
