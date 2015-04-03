/*
 * input.c
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
 
#include "common.h"
#include "input.h"
#include "toggles.h"

#ifdef G_OS_UNIX
 #include "joystick_linux.h"
 #include <linux/joystick.h>
 #include <fcntl.h>
#endif

void close_channels(guidata *gui)
{
#ifdef G_OS_UNIX
  int a;
  for (a=0;a<9;a++)
  {
    if (gui->joy[a].channel != NULL)
    {
      g_io_channel_unref(gui->joy[a].channel);
    }
  }
#endif

  gtk_window_set_resizable(GTK_WINDOW(gui->inputwindow), TRUE);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
gboolean close_input(GtkWidget *widget, GdkEvent *event, guidata *gui)
{
  close_channels(gui);
  gtk_widget_hide_on_delete(gui->inputwindow);
  return TRUE;
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void cancel_input(GtkButton *button, guidata *gui)
{
  close_channels(gui);
  gtk_widget_hide(gui->inputwindow);
}

gchar* hash2joy(gchar *key ,gint i)
{
  gchar **line;
  gchar *joy_hash;
  
  if (key[0] == '\0') return NULL;
    
  line = g_strsplit(key, " ", 0);
    
  if (line == NULL) return NULL;
  
    joy_hash = g_strdup(line[i]);
  
  g_strfreev(line);
  
  return joy_hash;
}

gchar* sdl2gdk(gchar *key)
{
  gchar **line;
  gchar **item;
  gchar *gdk_key;
  guint sdl_value;
  
  if (key[0] == '\0') return NULL;
    
  line = g_strsplit(key, " ", 2);
    
  if (!line) return NULL;
  
  item = g_strsplit(line[1], "~", 2);
  
  if (item[1] != NULL)
    sdl_value = g_ascii_strtod(item[0], NULL);
  else
    sdl_value = g_ascii_strtod(line[1], NULL);
    
  g_strfreev(item);
  g_strfreev(line);
  
  if (sdl_value < 325) 
    gdk_key = g_strdup(sdl_to_gdk[sdl_value]);
  else 
    gdk_key = NULL;
  
  return gdk_key;
}

#ifdef G_OS_UNIX
gboolean joy_watch( GIOChannel *channel, GIOCondition cond, guidata *gui)
{
  if(cond == G_IO_HUP)
  {
    //g_io_channel_unref(channel);
    return FALSE;
  }
  
  struct js_event e;
  int fd;
  
  int a;
  for (a=0;a<9;a++)
  {
	fd=0;
    if (gui->joy[a].channel)
      fd = g_io_channel_unix_get_fd (channel);
      
    if ((fd > 0) && (fd == gui->joy[a].js_fd))
     break;
  }
  
  if ((fd==0) || (fd != gui->joy[a].js_fd))
    return FALSE;
    
  read (fd, &e, sizeof(e));
  
  if (gui->inputedited == TRUE)
   return TRUE;
   
  if ((e.type == JS_EVENT_BUTTON) || (e.type == JS_EVENT_AXIS))
  {
  GtkTreeIter  iter;
  gchar *command;
  gchar *on;
  gchar *joyc;
  unsigned int b;

  if (e.type == JS_EVENT_BUTTON) {
    b = e.number;
    on = g_strdup_printf("Button %02i", b);
    //printf("Event: %08x\n", b);
    joyc = g_strdup_printf("joystick %016llx %08x", gui->joy[a].id, b);
  }
  else
  if (e.type == JS_EVENT_AXIS) {
    //printf("%i\n", e.value);
    
    if (e.number % 2) 
    {
      if (e.value<0) on = g_strdup_printf("Axis %i Up", e.number);
        else on = g_strdup_printf("Axis %i Down", e.number);
    }
    else
    {
	  if (e.value<0) on = g_strdup_printf("Axis %i Left", e.number);
	    else on = g_strdup_printf("Axis %i Right", e.number);
	}
	
    b = (0x8000 | e.number);
    if (e.value<0) b = (0x4000 | b);
    //printf("Event: %08x\n", b);
    joyc = g_strdup_printf("joystick %016llx %08x", gui->joy[a].id, b);
  }
  
  if ((gui->treepath != NULL) && (!gui->inputedited))
  {
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(gtk_builder_get_object(gui->specific, "treeview_input")));  
    gtk_tree_model_get_iter_from_string(model, &iter, gui->treepath);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter, 2, on, 3, gui->joy[a].name, -1);
  
    gtk_tree_model_get(model, &iter, 1, &command, -1);
  
    gchar *fullcommand = g_strconcat(gui->system,".input.",gui->port, command, NULL);
    g_free(command);
    g_hash_table_replace(gui->clist, fullcommand, fullcommand);
  
    fullcommand++;
    g_hash_table_insert(gui->hash, g_strdup(fullcommand), joyc);
    fullcommand--;
  
    gui->inputedited = TRUE;
  }
  else g_free(joyc);
  
  g_free(on);
  return TRUE;
}
  return TRUE;
}
#endif

void read_input(guidata *gui)
{
  GtkTreeModel *model;
  GtkTreeIter  iter;
  
  gui->system++;
  
  model = GTK_TREE_MODEL(gtk_builder_get_object(
                                           gui->specific, gui->system));
  
  gtk_tree_view_set_model(GTK_TREE_VIEW(gtk_builder_get_object(
                              gui->specific, "treeview_input")), model);
  
  if (gtk_tree_model_get_iter_first(model, &iter))
  {
	do
	{
	  gchar *action;
	  gchar *command;	  
	  gchar *hashkey;
	  
	  gtk_tree_model_get(model, &iter, 1, &action, -1);
      command = g_strconcat(gui->system, ".input.", 
                            gui->port, action, NULL);
      g_free(action);
      hashkey = g_hash_table_lookup(gui->hash, command);
      g_free(command);

      if (hashkey != NULL)
      {
		if (hashkey[0]=='k')
		{
	      gchar *value = sdl2gdk(hashkey);
	    
          if (value != NULL)
          {
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, 2, value, 3, "Keyboard", -1);
            g_free(value);
          }
          else gtk_list_store_set(GTK_LIST_STORE(model), &iter, 2, "", 3 , "", -1);
	    }
	    #ifdef G_OS_UNIX
	    else if (hashkey[0]=='j')
	    {
		  gchar *value = hash2joy(hashkey, 2);
		  gchar *hash = hash2joy(hashkey, 1);
		  long long unsigned int ll = g_ascii_strtoull(value, NULL, 16);
		  
		  g_free(value);
		  
          int a;
          for (a=0;a<9;a++)
          {
		    if (gui->joy[a].id == g_ascii_strtoull(hash, NULL, 16))
		    {
			  if (ll>0x7FFF)
			  {
			    if (ll % 2) 
                {
                  if (ll<0xc000) value = g_strdup_printf("Axis %01llx Down", ll & 0xFFF);
                  else value = g_strdup_printf("Axis %01llx Up", ll & 0xFFF);
                }
                else
                {
	              if (ll<0xc000) value = g_strdup_printf("Axis %01llx Right", ll & 0xFFF);
	              else value = g_strdup_printf("Axis %01llx Left", ll & 0xFFF);
	            }
	          }
	          else value = g_strdup_printf("Button %02lli", ll & 0xFF);
	          
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, 2, value, 3, gui->joy[a].name, -1);
            g_free(value);
		  }
		 // else gtk_list_store_set(GTK_LIST_STORE(model), &iter, 2, "", 3 , hash, -1);
	    }
	    g_free(hash);
	  }
	  #endif
	  else gtk_list_store_set(GTK_LIST_STORE(model), &iter, 2, "", 3 , "", -1);
      }
    } while (gtk_tree_model_iter_next(model, &iter));
  }
  gui->system--;
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void switch_page(GtkNotebook *notebook, GtkWidget *page, guint page_num, guidata *gui)
{
  g_free(gui->port);
  gui->port = g_strdup_printf("port%i.", (page_num+1));
  read_input(gui);
}                                                       

void set_ports(guint page, GtkNotebook *notebook, guidata *gui)
{
  gint i;
  
  gui->port = g_strdup("port1.");
  gtk_notebook_set_show_tabs(notebook, TRUE);
  
  for (i=0;i<page;i++)
  {
      GtkWidget *tab = gtk_notebook_get_nth_page(notebook, i);        
      gtk_widget_show(tab);  
  } 
  
  for (i=page;i<8;i++)
  {
      GtkWidget *tab = gtk_notebook_get_nth_page(notebook, i);         
      gtk_widget_hide(tab);  
  }   
}

void set_builtin(GtkNotebook *notebook, guidata *gui)
{
  gui->port = g_strdup("builtin.");
  gtk_notebook_set_show_tabs(notebook, FALSE);  
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_input_clicked (GtkButton *button, guidata *gui)
{
  gchar *text;
  GtkNotebook *notebook;
  
  text = g_strconcat("<b>Key Assignments - ", 
                     gui->fullsystem, "</b>", NULL);
                     
  gtk_label_set_markup(GTK_LABEL(gtk_builder_get_object(
                                     gui->specific, "label108")), text);
  g_free(text);
  
  text = g_strconcat("Key Assignments - ", gui->fullsystem, NULL);
  gtk_window_set_title (GTK_WINDOW(gui->inputwindow), text);
  g_free(text);
	
  notebook = GTK_NOTEBOOK(gtk_builder_get_object(gui->specific, "notebook1"));
  
  g_free(gui->port);
  
  switch (gui->pagesys)
  {
    case 0:  set_builtin(notebook, gui); break;
    case 1:  set_builtin(notebook, gui); break;
    case 2:  set_builtin(notebook, gui); break;
    case 3:  set_builtin(notebook, gui); break;
    case 4:  set_ports(4, notebook, gui); break;
    case 5:  set_ports(5, notebook, gui); break;
    case 6:  set_ports(2, notebook, gui); break;
    case 7:  set_builtin(notebook, gui); break;
    case 8:  set_ports(2, notebook, gui); break;
    case 9:  set_ports(2, notebook, gui); break;
    case 10: set_ports(8, notebook, gui); break;
    case 11: set_ports(2, notebook, gui); break;
    case 12: set_builtin(notebook, gui); break;
    case 13: set_builtin(notebook, gui); break;
    default : break;
  }
  
  gtk_notebook_set_current_page(notebook, 0);
  
  if (gui->changed)
  {
    g_hash_table_remove_all(gui->hash);
    read_cfg(gui);
    gui->changed = FALSE;
  }
  
  #ifdef G_OS_UNIX
  int a;
  for (a=0;a<9;a++)
  {
    if (GetJoy(a,gui)>0)
    {
      gui->joy[a].channel = g_io_channel_unix_new(gui->joy[a].js_fd);
  
      //g_io_channel_set_flags (gui->joy[a].channel, G_IO_FLAG_NONBLOCK, NULL);
      g_io_channel_set_close_on_unref(gui->joy[a].channel, TRUE);
      g_io_add_watch(gui->joy[a].channel, G_IO_IN|G_IO_HUP, (GIOFunc)joy_watch, gui);
    }
    else
    {
     gui->joy[a].channel = NULL; 
	}
  }
  #endif
  
  read_input(gui);

  if (!gui->executing)
    g_object_set(G_OBJECT(gtk_builder_get_object(gui->specific,
                                  "cellkey")), "editable", TRUE, NULL);
  else
    g_object_set(G_OBJECT(gtk_builder_get_object(gui->specific,
                                  "cellkey")), "editable", FALSE, NULL); 
                                  
  gtk_widget_show(gui->inputwindow);
  gtk_tree_view_columns_autosize(GTK_TREE_VIEW(gtk_builder_get_object(
                                     gui->specific, "treeview_input")));
  gtk_window_set_resizable(GTK_WINDOW(gui->inputwindow), FALSE);
}

guint gdk_to_sdl_keyval(guint gdk_key)
{
        if (!(gdk_key & 0xFF00))
        {
                gdk_key = g_ascii_tolower(gdk_key);
                return gdk_key;
        }
        
        if (gdk_key & 0xFFFF0000) return 0;
        
        guint sdl_key = gdk_to_sdl[gdk_key & 0xFF];
        return sdl_key;
}

gboolean editable_mouse_cb(GtkWidget *ed, GdkEvent *event, guidata *gui)
{
  return TRUE;
}

gboolean editable_key_cb(GtkWidget *ed, GdkEventKey *event, guidata *gui)
{
  GtkTreeModel *model;
  GtkTreeIter  iter;
  gchar *command;
  gchar *fullcommand;
  gchar *key = NULL;
  guint nkey;

  nkey = gdk_to_sdl_keyval(event->keyval);
  if (sdl_to_gdk[nkey] != NULL) 
    key = g_strdup(sdl_to_gdk[nkey]);//g_ascii_strup(gdk_keyval_name(event->keyval), -1);
  else return TRUE;
  
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(gtk_builder_get_object(gui->specific, "treeview_input")));  
  gtk_tree_model_get_iter_from_string(model, &iter, gui->treepath);
  gtk_list_store_set(GTK_LIST_STORE(model), &iter, 2, key, 3, "Keyboard", -1);
  g_free(key);
  
  gtk_tree_model_get(model, &iter,1 , &command, -1);
  
  fullcommand = g_strconcat(gui->system,".input.",gui->port, command, NULL);
  g_free(command);
  g_hash_table_replace(gui->clist, fullcommand, fullcommand);
  key = g_strdup_printf("%i",nkey);
  
  fullcommand++;
  g_hash_table_insert(gui->hash, g_strdup(fullcommand), g_strconcat("keyboard ", key, NULL));
  fullcommand--;
  g_free(key);
  
  gtk_cell_editable_editing_done(GTK_CELL_EDITABLE(ed));
  gtk_cell_editable_remove_widget(GTK_CELL_EDITABLE(ed));
  
  return TRUE;
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
gboolean on_treeview_input_button_press_event(GtkWidget *widget,
                                              GdkEvent  *event,
                                              guidata *gui)
{
if (event->type == GDK_2BUTTON_PRESS)
  return TRUE;
else
 return FALSE; 
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void key_setting (GtkCellRendererText *renderer, 
                  GtkCellEditable *editable, 
                  gchar *path, 
                  guidata *gui)
{
  if(GTK_IS_ENTRY(editable))
  {
    gtk_entry_set_text(GTK_ENTRY(editable), "");
      
    g_free(gui->treepath);
    gui->treepath = g_strdup(path);
    gui->inputedited = FALSE;
    
    g_signal_connect(GTK_WIDGET(editable), "button-press-event", 
                     G_CALLBACK(editable_mouse_cb), gui);
                       
    g_signal_connect(GTK_WIDGET(editable), "key-press-event", 
                     G_CALLBACK(editable_key_cb), gui);
  
  }
}
