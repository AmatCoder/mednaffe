/*
 * bios_helper.h
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


#ifndef __BIOS_HELPER_H__
#define __BIOS_HELPER_H__

#include <glib.h>

G_BEGIN_DECLS

const gchar* bios_get_icon (const gchar* text);
gchar* bios_check_sha256 (GHashTable* table, const gchar* command, const gchar* value, const gchar* sha256);

G_END_DECLS

#endif
