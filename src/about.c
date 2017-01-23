/*
 * about.c
 *
 * Copyright 2013-2017 AmatCoder
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
 */

#include "common.h"

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void show_about(GtkMenuItem *menuitem, guidata *gui)
{
  const char *authors[2] = {"AmatCoder", NULL};

  const char *gpl3 =
"Mednaffe is free software: you can redistribute it and/or modifyit \
under the terms of the GNU General Public License as published by \
the Free Software Foundation, either version 3 of the License, or \
(at your option) any later version.\n\n\
Mednaffe is distributed in the hope that it will be useful, \
but WITHOUT ANY WARRANTY; without even the implied warranty of \
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the \
GNU General Public License for more details.\n\n\
You should have received a copy of the GNU General Public License \
along with Mednaffe.  If not, see <http://www.gnu.org/licenses/>.";


gtk_show_about_dialog(
  GTK_WINDOW(gui->topwindow),
  "program-name", "Mednaffe",
  "version" ,"0.9.0beta",
  "authors", authors,
#ifdef STATIC_ENABLED
  "copyright", "See NOTICE file for license details.",
#else
  "copyright", "Copyright \xc2\xa9 2010-2017 AmatCoder",
#endif
  "comments", "A front-end (GUI) for Mednafen emulator",
  "license", gpl3,
  "wrap-license", TRUE,
  "website", "https://github.com/AmatCoder/mednaffe",
  "logo", gui->pixbuf, 
  NULL);
}
