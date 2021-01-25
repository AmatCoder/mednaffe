/*
 * medinput.h
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


#ifndef __MEDINPUT_H__
#define __MEDINPUT_H__

#include <gtk/gtk.h>
#include "medwidget.h"

G_BEGIN_DECLS

typedef struct _MedInput MedInput;

struct _MedInput {
  GtkBox parent_instance;
};


GType med_input_get_type (void) G_GNUC_CONST;
MedInput* med_input_new (void);

G_END_DECLS

#endif
