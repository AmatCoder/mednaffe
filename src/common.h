/*
 * common.h
 * 
 * Copyright 2013 AmatCoder
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

typedef struct
{
   GtkWidget *topwindow;
   GtkWidget *cbpath;
   GtkWidget *sbname;
   GtkWidget *sbnumber;
   GtkWidget *gamelist;
   GtkWidget *scrollwindow;
   GtkWidget *notebook;
   GtkWidget *notebook2;
   GtkWidget *setlabel;
   GtkWidget *systemlist;
   GtkWidget *globalist;
   GtkBuilder *specific;
   GtkBuilder *builder;
   GtkTreeModelSort *modelsort;
   GtkTreeModelSort *zipmodelsort;
   GtkTreeModelFilter *zipfilter;
   GtkTreeModelSort *cuemodelsort;
   GtkTreeModelFilter *cuefilter;
   GtkTreeViewColumn *column;
   GtkListStore *store;
   gint filtermode;
   gboolean recursive;
   gchar *binpath;
   gchar *fullpath;
   gchar *rompath;
   gchar *rom;
   GHashTable* hash;
   GHashTable* clist;
   GSList *dinlist;
   gboolean executing;
   gchar **command;
   gchar *system;
   gboolean changing;
   gint state;
}guidata;
