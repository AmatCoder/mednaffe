/*
 * logbook.h
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


#ifndef __LOGBOOK_H__
#define __LOGBOOK_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS


typedef struct _Logbook Logbook;

struct _Logbook {
  GtkNotebook parent_instance;
};


typedef enum  {
  LOGBOOK_LOG_TAB_FRONTEND = 1 << 0,
  LOGBOOK_LOG_TAB_EMU = 1 << 1
} LogbookLogTab;


GType logbook_get_type (void) G_GNUC_CONST;
Logbook* logbook_new (void);

void logbook_write_log (Logbook* self, LogbookLogTab tab, const gchar* line);
void logbook_delete_log (Logbook* self, LogbookLogTab tab);
gchar* logbook_get_last_line (Logbook* self);

G_END_DECLS

#endif
