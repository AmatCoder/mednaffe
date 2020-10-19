/*
 * medbiosentry.h
 *
 * Copyright 2013-2020 AmatCoder
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


#ifndef __MEDBIOSENTRY_H__
#define __MEDBIOSENTRY_H__

#include <gtk/gtk.h>

#include "meddialogentry.h"

G_BEGIN_DECLS

#define IS_MED_BIOS_ENTRY(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), med_bios_entry_get_type ()))

typedef struct _MedBiosEntry MedBiosEntry;

struct _MedBiosEntry {
	MedDialogEntry parent_instance;
};


GType med_bios_entry_get_type (void) G_GNUC_CONST;
MedBiosEntry* med_bios_entry_new (void);

G_END_DECLS

#endif
