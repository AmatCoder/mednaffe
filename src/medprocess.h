/*
 * medprocess.h
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


#ifndef __MEDPROCESS_H__
#define __MEDPROCESS_H__


#include <glib-object.h>


G_BEGIN_DECLS


typedef struct _MedProcess MedProcess;

struct _MedProcess {
  GObject parent_instance;
  gchar* MedVersion;
  gchar* MedPath;
  GHashTable* table;
};


GType med_process_get_type (void) G_GNUC_CONST;
MedProcess* med_process_new (void);

void med_process_read_conf (MedProcess* self);
void med_process_exec_emu (MedProcess* self, gchar** command);


G_END_DECLS

#endif
