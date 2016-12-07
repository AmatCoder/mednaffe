/*
 * common.h
 *
 * Copyright 2013-2015 AmatCoder
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

#include <gtk/gtk.h>

#ifdef G_OS_WIN32
 #include <windows.h>
 #include "SDL2/SDL.h"
#endif

typedef struct
{
#ifdef G_OS_UNIX
  gint js_fd;
  gint ev_fd;
  long long unsigned int id;
  gchar name[128];
  GIOChannel *channel;
#else
  SDL_Joystick *sdljoy;
  SDL_JoystickID sdl_id;
  long long unsigned int id;
  gchar *name;
  gboolean xinput;
#endif
}joydata;

typedef struct
{
   GtkWidget *topwindow;
   GtkWidget *prefwindow;
   GtkWidget *folderwindow;
   GtkWidget *inputwindow;
   GtkWidget *cbpath;
   GtkWidget *sbname;
   GtkWidget *sbnumber;
   GtkWidget *gamelist;
   GtkWidget *scrollwindow;
   GtkWidget *notebook;
   GtkWidget *notebook2;
   GtkWidget *setlabel;
   GtkWidget *launch;
   GtkWidget *systemlist;
   GtkWidget *globalist;
   GtkTextBuffer *textfe;
   GtkTextBuffer *textout;
   GtkBuilder *specific;
   GtkBuilder *builder;
   GtkBuilder *settings;
   GtkTreeViewColumn *column;
   GtkListStore *store;
   GtkCellEditable *editable;
   GdkPixbuf *pixbuf;
   gboolean listmode;
   gint filter;
   gint pagesys;
   GSList *itemlist;
   gchar *filters;
   gchar *binpath;
   gchar *fullpath;
   gchar *rompath;
   gchar *rom;
   gchar *cfgfile;
   GHashTable* hash;
   GHashTable* clist;
   GSList *dinlist;
   gboolean executing;
   gboolean changed;
 #ifdef G_OS_WIN32
   gchar *command;
 #else
   gchar **command;
 #endif
   gchar *system;
   gchar *fullsystem;
   gchar *treepath;
   gchar *port;
   gboolean changing;
   gboolean resetup_folder;
   gint state;
   joydata joy[9];
   gboolean inputedited;
   gboolean inputkeys;
   gchar *m_error;
}guidata;
