/*
 * win32util.c
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


#include "win32util.h"
#include <windows.h>


gchar*
win32_get_process_directory (void)
{
  wchar_t wpath[MAX_PATH];
  if (!GetModuleFileNameW (NULL, wpath, MAX_PATH))
    return NULL;

  gchar* cpath =  g_utf16_to_utf8 (wpath, -1, NULL, NULL, NULL);
  gchar* path = g_path_get_dirname (cpath);
  g_free (cpath);

  return path;
}
