/*
 * menuinput.h
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


#ifndef __MENUINPUT_H__
#define __MENUINPUT_H__


#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _MenuInput MenuInput;

struct _MenuInput {
	GtkMenu parent_instance;
	GtkMenuItem* clear;
	GtkMenuItem* or;
	GtkMenuItem* and;
	GtkMenuItem* and_not;
};

GType menu_input_get_type (void) G_GNUC_CONST;
MenuInput* menu_input_new (void);
void menu_input_enable_all (MenuInput* self, gboolean b);

G_END_DECLS

#endif
