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
#include "log.h"

#ifdef G_OS_UNIX
 #include "joystick_linux.h"
 #include <linux/joystick.h>
 #include <fcntl.h>
#else
 #include <windows.h>
 #include "joystick_win.h"

  static int bool;
  static guint which;
  static gchar *on;
  static gchar *joyc;
  static GMutex mutex;
#endif

#define DEADZONE 16384

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

#ifdef G_OS_WIN32
int i;
for(i=0;i<9;i++)
{
  if(SDL_JoystickGetAttached(gui->joy[i].sdljoy))
    SDL_JoystickClose(gui->joy[i].sdljoy);
}
   SDL_Quit();
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

gchar* modkeys(gchar *key, gchar *value)
{
  if (key[0] == '\0') return NULL;

  gchar **items;
  gchar *value2 = NULL;
  guint num,i;

  items = g_strsplit(key, "+", 4);
  num = g_strv_length(items);

  if (num>1)
  {
    for (i=(num-1);i>0;i--)
    {
      if (items[i][0]=='a') value2 = g_strconcat("Alt+", value, NULL);
      else if (items[i][0]=='c') value2 = g_strconcat("Ctrl+", value, NULL);
      else if (items[i][0]=='s') value2 = g_strconcat("Shift+", value, NULL);

      g_free(value);
      value = value2;
    }
  }
  g_strfreev(items);

  return value;
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

  if (fd==0) return FALSE;
  if (a>8) return FALSE;
  if (fd != gui->joy[a].js_fd) return FALSE;

  ssize_t s = read (fd, &e, sizeof(e));

  if (s==-1) return TRUE;

  if (gui->inputedited == TRUE)
   return TRUE;

  if ((e.type == JS_EVENT_BUTTON) ||
     ((e.type == JS_EVENT_AXIS) &&
     ((e.value < -DEADZONE) || (e.value > DEADZONE))))
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
    gchar *fullcommand;
    GtkTreeModel *model;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(gtk_builder_get_object(gui->specific, "treeview_input")));
    gtk_tree_model_get_iter_from_string(model, &iter, gui->treepath);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter, 2, on, 3, gui->joy[a].name, -1);

    gtk_tree_model_get(model, &iter, 1, &command, -1);

    if (gui->inputkeys)
      fullcommand = g_strconcat("-command.", command, NULL);
    else
      fullcommand = g_strconcat(gui->system,".input.",gui->port, command, NULL);

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

#ifdef G_OS_WIN32
gchar* xinput_map(long long unsigned int b)
{
  gchar *string = NULL;

  switch (b)
  {
    case 0:  string = g_strdup("D-Pad Up"); break;
    case 1:  string = g_strdup("D-Pad Down"); break;
    case 2:  string = g_strdup("D-Pad Left"); break;
    case 3:  string = g_strdup("D-Pad Right"); break;
    case 4:  string = g_strdup("Button Start"); break;
    case 5:  string = g_strdup("Button Back"); break;
    case 6:  string = g_strdup("Stick Button Left"); break;
    case 7:  string = g_strdup("Stick Button Right"); break;
    case 8:  string = g_strdup("Shoulder Left"); break;
    case 9:  string = g_strdup("Shoulder Right"); break;
    case 12: string = g_strdup("Button A"); break;
    case 13: string = g_strdup("Button B"); break;
    case 14: string = g_strdup("Button X"); break;
    case 15: string = g_strdup("Button Y"); break;
    default : break;
  }
  return string;
}
#endif

void read_input(guidata *gui)
{
  GtkTreeModel *model;
  GtkTreeIter  iter;

  gui->system++;
  if (gui->inputkeys)
  {
    model = GTK_TREE_MODEL(gtk_builder_get_object(gui->builder, "keys"));
  }
  else
  {
    model = GTK_TREE_MODEL(gtk_builder_get_object(
                                           gui->specific, gui->system));
  }
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
      if (gui->inputkeys) command = g_strconcat("command.", action, NULL);
      else
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

          if ((value != NULL) && (gui->inputkeys)) value = modkeys(hashkey, value);

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

        #ifdef G_OS_WIN32
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
                  if (ll<0xc000) value = g_strdup_printf("Axis %01I64x Down", ll & 0xFFF);
                  else value = g_strdup_printf("Axis %01I64x Up", ll & 0xFFF);
                }
                else
                {
                  if (ll<0xc000) value = g_strdup_printf("Axis %01I64x Right", ll & 0xFFF);
                  else value = g_strdup_printf("Axis %01I64x Left", ll & 0xFFF);
                }

                if (gui->joy[a].xinput==TRUE)
                {
                  if ((ll & 0xFFF)==4)
                  {
                    g_free(value);
                    value = g_strdup("Trigger Left");
                  }
                  else if ((ll & 0xFFF)==5)
                  {
                    g_free(value);
                    value = g_strdup("Trigger Right");
                  }
                }

              }
              else
              {
                if (gui->joy[a].xinput==TRUE)
                {
                  value = xinput_map((ll & 0xFF));
                }
                else value = g_strdup_printf("Button %02I64i", ll & 0xFF);
              }

             if ((value) && (gui->joy[a].name))
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
 int GetControllerIndex(SDL_JoystickID instance, guidata *gui)
 {
  int i;
  for(i=0;i<9;i++){
      if (gui->joy[i].sdl_id == instance)
      return i;
  }
  return -1;
 }

DWORD WINAPI joy_thread(LPVOID lpParam)
{
  guidata *gui = lpParam;
  SDL_Event event;

  SDL_JoystickEventState(SDL_ENABLE);

  while (SDL_PollEvent(&event)){} //Drops pending events

  while(SDL_WaitEvent(&event)) {

  if (bool==1) break;

  //printf("Unhandled Event: %i\n", event.type);

  if ((event.type == SDL_JOYBUTTONDOWN) ||
      (event.type == SDL_JOYHATMOTION) ||
      ((event.type == SDL_JOYAXISMOTION) &&
       ((event.jaxis.value < -DEADZONE) || (event.jaxis.value > DEADZONE))))
  {
  unsigned int b;
  int a;

  g_mutex_lock (&mutex);
    bool=1;
  g_mutex_unlock (&mutex);

  if (event.type == SDL_JOYBUTTONDOWN) {

    b = event.jbutton.button;
    a = GetControllerIndex(event.jbutton.which, gui);
    if (a>-1)
    {
      if ((gui->joy[a].xinput==TRUE) && (b>9))
        b=b+2;

      joyc = g_strdup_printf("joystick %016I64x %08x", gui->joy[a].id, b);
      //printf("Button Event: %s\n", joyc);
      which = a;
    }
    if (gui->joy[a].xinput==TRUE)
    {
     on = xinput_map(b);
    }
    else on = g_strdup_printf("Button %02i", b);
  }
  else
    if (event.type == SDL_JOYHATMOTION) {
        a = GetControllerIndex(event.jhat.which, gui);
    if (a>-1)
    {
    //printf("Value: %i\n", event.jhat.value);
    int h=(((event.jhat.hat*2)-1) + SDL_JoystickNumAxes(gui->joy[a].sdljoy));
    b = (0x8000 | h);
    if (event.jhat.value == SDL_HAT_UP)
    {
      on = g_strdup_printf("Axis %i Up", h+2);
      b = (0x4000 | b)+2;
    }
    else if(event.jhat.value == SDL_HAT_DOWN)
    {
      on = g_strdup_printf("Axis %i Down", h+2);
      b=b+2;
    }
    else if(event.jhat.value == SDL_HAT_RIGHT)
    {
      on = g_strdup_printf("Axis %i Right", h+1);
      b=b+1;
    }
    else if(event.jhat.value == SDL_HAT_LEFT)
    {
      on = g_strdup_printf("Axis %i Left", h+1);
      b = (0x4000 | b)+1;
    }

    which = a;
    joyc = g_strdup_printf("joystick %016I64x %08x", gui->joy[a].id, b);
    //printf("Hat Event: %s\n", joyc);
  }
}
  else
  if (event.type == SDL_JOYAXISMOTION)
  {
    a = GetControllerIndex(event.jaxis.which, gui);
    if (a>-1)
    {
    printf("Value: %i\n", event.jaxis.value);

    if (event.jaxis.axis % 2)
    {
      if (gui->joy[a].xinput==TRUE)
      {
        if (event.jaxis.value<0) on = g_strdup_printf("Axis %i Up", event.jaxis.axis);
          else on = g_strdup_printf("Axis %i Down", event.jaxis.axis);

          if (event.jaxis.axis==5)
          {
            g_free(on);
            on = g_strdup("Trigger Right");
          }
      }
      else
      {
        if (event.jaxis.value<0) on = g_strdup_printf("Axis %i Up", event.jaxis.axis);
         else on = g_strdup_printf("Axis %i Down", event.jaxis.axis);
      }
    }
    else
    {
      if (event.jaxis.value<0) {on = g_strdup_printf("Axis %i Left", event.jaxis.axis);}
        else {on = g_strdup_printf("Axis %i Right", event.jaxis.axis);}

      if (gui->joy[a].xinput==TRUE)
      {
        if (event.jaxis.axis==4)
        {
          g_free(on);
          on = g_strdup("Trigger Left");
        }
      }
    }
    which = a;
    b = (0x8000 | event.jaxis.axis);
    if ((gui->joy[a].xinput==TRUE) && (event.jaxis.axis % 2))
    {
      if (event.jaxis.value>0) b = (0x4000 | b);
    }
    else
    {
      if (event.jaxis.value<0) b = (0x4000 | b);
    }
    joyc = g_strdup_printf("joystick %016I64x %08x", gui->joy[a].id, b);
    printf("Axis Event: %s\n", joyc);
    }
   }
  }
}
  SDL_JoystickEventState(SDL_IGNORE);
  return 0;
}
#endif

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_input_clicked (GtkButton *button, guidata *gui)
{

  if (gui->executing) return;

  gui->inputkeys = (gtk_button_get_label(button)[0]=='K');

  GtkNotebook *notebook =
    GTK_NOTEBOOK(gtk_builder_get_object(gui->specific, "notebook1"));

  if (gui->inputkeys)
  {
    gtk_label_set_markup(GTK_LABEL(gtk_builder_get_object(
                                   gui->specific, "label108")), "<b>Key Assignments</b>");

    gtk_window_set_title (GTK_WINDOW(gui->inputwindow), "Key Assignments");

    gtk_widget_hide(GTK_WIDGET(notebook));
  }
  else
  {
    gchar *text = g_strconcat("<b>Controller Setup - ",
                    gui->fullsystem, "</b>", NULL);

    gtk_label_set_markup(GTK_LABEL(gtk_builder_get_object(
                                     gui->specific, "label108")), text);
    g_free(text);

    text = g_strconcat("Controller Setup - ", gui->fullsystem, NULL);
    gtk_window_set_title (GTK_WINDOW(gui->inputwindow), text);

    g_free(text);
    g_free(gui->port);

    gtk_widget_show(GTK_WIDGET(notebook));

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
      case 10: set_ports(2, notebook, gui); break;
      case 11: set_ports(2, notebook, gui); break;
      case 12: set_builtin(notebook, gui); break;
      case 13: set_builtin(notebook, gui); break;
      default : break;
    }

    gtk_notebook_set_current_page(notebook, 0);
  }

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
      gchar *id =g_strdup_printf(" - Unique ID: %016llx\n", gui->joy[a].id); 
      print_log("Joystick detected: ", FE, gui);
      print_log(gui->joy[a].name, FE, gui);
      print_log(id, FE, gui);
      g_free(id);

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

  #ifdef G_OS_WIN32

  SDL_SetHint("SDL_JOYSTICK_ALLOW_BACKGROUND_EVENTS", "1");
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

  int i;
  for(i=0;i<9;i++)
  {
    gui->joy[i].sdljoy = SDL_JoystickOpen(i);
    gui->joy[i].xinput = FALSE;

    if(!gui->joy[i].sdljoy)
    {
      //printf("Couldn't open Joystick %i\n", i);
      gui->joy[i].sdl_id = 0;
      gui->joy[i].id = 1;
      gui->joy[i].name = NULL;
    }
    else
    {
      gui->joy[i].sdl_id = SDL_JoystickInstanceID(gui->joy[i].sdljoy);
      gui->joy[i].name = g_strdup(SDL_JoystickName(gui->joy[i].sdljoy));

      SDL_JoystickGUID guid = SDL_JoystickGetGUID(gui->joy[i].sdljoy);
      char pszGUID[32];
      SDL_JoystickGetGUIDString(guid, (char *)pszGUID, 33);

      /*printf("GUID: %s\n", pszGUID);*/

      /*if ((guid.data[0]=='x') &&
          (guid.data[1]=='i') &&
          (guid.data[2]=='n') &&
          (guid.data[3]=='p') &&
          (guid.data[4]=='u') &&
          (guid.data[5]=='t'))*/
     if (g_strcmp0(pszGUID, "00000000000000000000000000000000")==0)
     {
      gui->joy[i].id = (0x00000000 << 24) | (0x00000001 << 16);
      gui->joy[i].xinput = TRUE;
     }
     else
     {
      GetJoy(i, gui);
     }
     CheckDuplicates(i, gui);
    }
  }

  for(i=0;i<9;i++)
  {
    if (gui->joy[i].name != NULL)
    {
      gchar *id =g_strdup_printf(" - Unique ID: %016I64x\n", gui->joy[i].id);
      print_log("Joystick detected: ", FE, gui);
      print_log(SDL_JoystickName(gui->joy[i].sdljoy), FE, gui);
      print_log(id, FE, gui);
      g_free(id);
    }
  }

   /* gchar *string;

      gchar *dir = g_win32_get_package_installation_directory_of_module(NULL);
      gchar *path =g_strconcat(dir, "\\stdout.txt", NULL);

      if (g_file_get_contents(path, &string, NULL, NULL))
      {
        gchar **aline = g_strsplit(string, "\n", 0);
        gint num = g_strv_length(aline);
        gint l;
        for(l=0;l<num-1;l++)
        {
          if (aline[l][2]=='J')
          {
             gint ii = g_ascii_digit_value(aline[l][11]);
             printf ("\n%i\n",ii);
             gchar *id = g_strrstr(aline[l],"ID:");
             gui->joy[ii].id = g_strndup(id+=4,16);
             //id = g_strrstr(aline[l],"Joystick ");
             //gui->joy[ii].name = g_strndup(id+=13,(strlen(aline[l])-45));
             printf (gui->joy[ii].id);
             //printf (gui->joy[ii].name);
          }
        }
        g_free(string);
        g_strfreev(aline);
      }
      g_free(dir);
      g_free(path); */

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

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
gboolean editable_mouse_cb(GtkWidget *ed, GdkEvent *event, guidata *gui)
{
  return TRUE;
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
gboolean editable_key_cb(GtkWidget *ed, GdkEventKey *event, guidata *gui)
{
  GtkTreeModel *model;
  GtkTreeIter  iter;
  gchar *command;
  gchar *fullcommand;
  gchar *key = NULL;
  gchar *key2;
  guint nkey;

  if (gui->inputkeys)
  {
    if (
        (event->keyval==65505) ||
        (event->keyval==65506) ||
        (event->keyval==65507) ||
        (event->keyval==65508) ||
        (event->keyval==65511) ||
        (event->keyval==65512) ||
        (event->keyval==65513) ||
        (event->keyval==65514)
       ) return FALSE;
  }

  #ifdef G_OS_WIN32
    g_mutex_lock (&mutex);
    if (bool==1) {
      g_mutex_unlock (&mutex);
      return TRUE;
    }
    bool=1;
  #endif

  nkey = gdk_to_sdl_keyval(event->keyval);
  if (sdl_to_gdk[nkey] != NULL)
    key = g_strdup(sdl_to_gdk[nkey]);//g_ascii_strup(gdk_keyval_name(event->keyval), -1);
  else return TRUE;

  model = gtk_tree_view_get_model(GTK_TREE_VIEW(gtk_builder_get_object(gui->specific, "treeview_input")));
  gtk_tree_model_get_iter_from_string(model, &iter, gui->treepath);

  if (gui->inputkeys)
  {
    key2 = key;

    if (event->state & GDK_SHIFT_MASK)
    {
      key = g_strconcat("Shift+", key2, NULL);
      g_free(key2);
      key2 = key;
    }
    if (event->state & GDK_MOD1_MASK)
    {
      key = g_strconcat("Alt+", key2, NULL);
      g_free(key2);
      key2 = key;
    }
    if (event->state & GDK_CONTROL_MASK)
    {
      key = g_strconcat("Ctrl+", key2, NULL);
      g_free(key2);
    }

  }
  gtk_list_store_set(GTK_LIST_STORE(model), &iter, 2, key, 3, "Keyboard", -1);
  g_free(key);

  gtk_tree_model_get(model, &iter, 1, &command, -1);

  if (gui->inputkeys)
   fullcommand = g_strconcat("-command.", command, NULL);
  else
    fullcommand = g_strconcat(gui->system,".input.",gui->port, command, NULL);

  g_free(command);
  g_hash_table_replace(gui->clist, fullcommand, fullcommand);
  key = g_strdup_printf("%i",nkey);

  if (gui->inputkeys)
  {
        key2 = key;

    if (event->state & GDK_CONTROL_MASK)
    {
      key = g_strconcat(key2, "+ctrl", NULL);
      g_free(key2);
      key2 = key;
    }
    if (event->state & GDK_MOD1_MASK)
    {
      key = g_strconcat(key2, "+alt", NULL);
      g_free(key2);
      key2 = key;
    }

    if (event->state & GDK_SHIFT_MASK)
    {
      key = g_strconcat(key2, "+shift", NULL);
      g_free(key2);
    }
  }

  fullcommand++;
  g_hash_table_insert(gui->hash, g_strdup(fullcommand), g_strconcat("keyboard ", key, NULL));
  fullcommand--;
  g_free(key);

  gtk_cell_editable_editing_done(GTK_CELL_EDITABLE(ed));
  gtk_cell_editable_remove_widget(GTK_CELL_EDITABLE(ed));

  gui->inputedited = TRUE;

  #ifdef G_OS_WIN32
    g_mutex_unlock (&mutex);
  #endif

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
void thread_watch(GPid pid, gint status, guidata *gui)
{
   //printf("Exiting from thread\n");
   g_mutex_lock (&mutex);

   if ((joyc != NULL) && (on != NULL) && (!gui->inputedited))
   {
     GtkTreeIter  iter;
     gchar *command;
     gchar* fullcommand;

     GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(gtk_builder_get_object(gui->specific, "treeview_input")));
     gtk_tree_model_get_iter_from_string(model, &iter, gui->treepath);
     gtk_list_store_set(GTK_LIST_STORE(model), &iter, 2, on, 3, gui->joy[which].name, -1);
     gtk_tree_model_get(model, &iter, 1, &command, -1);

    if (gui->inputkeys)
      fullcommand = g_strconcat("-command.", command, NULL);
    else
      fullcommand = g_strconcat(gui->system,".input.",gui->port, command, NULL);

     g_free(command);
     g_hash_table_replace(gui->clist, fullcommand, fullcommand);
     fullcommand++;
     g_hash_table_insert(gui->hash, g_strdup(fullcommand), g_strdup(joyc));
     fullcommand--;

     gtk_cell_editable_editing_done(gui->editable);
     gtk_cell_editable_remove_widget(gui->editable);
   }

   gui->inputedited = TRUE;

   g_free(on);
   g_free(joyc);

   g_mutex_unlock (&mutex);
   g_spawn_close_pid(pid);
}
#endif

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

    gui->editable = editable;
    gui->inputedited = FALSE;

    g_signal_connect(GTK_WIDGET(editable), "button-press-event",
                     G_CALLBACK(editable_mouse_cb), gui);

    g_signal_connect(GTK_WIDGET(editable), "key-press-event",
                     G_CALLBACK(editable_key_cb), gui);

#ifdef G_OS_WIN32
   bool = 0;
   on = NULL;
   joyc = NULL;
   which = 0;

   HANDLE hThread = CreateThread(NULL, 0, joy_thread, gui, 0, NULL);
   g_child_watch_add(hThread, (GChildWatchFunc)thread_watch, gui);
#endif

  }
}
