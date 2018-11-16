/*
 * pathcombobox.h
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


#ifndef __PATHCOMBOBOX_H__
#define __PATHCOMBOBOX_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS


typedef struct _PathComboBox PathComboBox;

struct _PathComboBox {
  GtkBox parent_instance;
  GtkComboBox* combo;
  GtkListStore* path_store;
};


GType path_combo_box_get_type (void) G_GNUC_CONST;
PathComboBox* path_combo_box_new (void);

gchar** path_combo_box_get_dirs (PathComboBox* self, int* length);
void path_combo_box_set_dirs (PathComboBox* self, gchar** st, int length);
void path_combo_box_save_panel_position (PathComboBox* self, gint vpos, gint hpos);


G_END_DECLS

#endif
