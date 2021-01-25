/*
 * panedlist.h
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


#ifndef __PANEDLIST_H__
#define __PANEDLIST_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _PanedList PanedList;

struct _PanedList {
  GtkPaned parent_instance;
};

GType paned_list_get_type (void) G_GNUC_CONST;
PanedList* paned_list_new (void);

void paned_list_fill_list (PanedList* self, GtkComboBox* combo, GtkTreeIter* iter);
void paned_list_show_filters (PanedList* self, gboolean b);
void paned_list_show_screens (PanedList* self, gboolean b);
void paned_list_request_launch (PanedList* self);

G_END_DECLS

#endif
