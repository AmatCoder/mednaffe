/*
 * gui.c
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

#include "common.h"

void display_tooltips(GtkBuilder *builder, gboolean state)
{
  gchar *text;
  GSList *list = NULL;
  GSList *iterator = NULL;
  
  list = gtk_builder_get_objects(builder);

  for (iterator = list; iterator; iterator = iterator->next)
  {
    if (GTK_IS_WIDGET(iterator->data))
    {
      text = gtk_widget_get_tooltip_text(iterator->data);
      if (text!=NULL)
        gtk_widget_set_has_tooltip(
            iterator->data, state);
          
      g_free(text);
    }
  }
  
  g_slist_free(list);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_showtooltips_toggled(GtkToggleButton *togglebutton,
                             guidata *gui)
{
  gboolean state;

  state = gtk_toggle_button_get_active(togglebutton);
  
  display_tooltips(gui->builder, state);
  display_tooltips(gui->specific, state);
  display_tooltips(gui->settings, state);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_rbnothing_activate(GtkButton *button, guidata *gui)
{
  gui->state=0;
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_rbminimize_activate(GtkButton *button, guidata *gui)
{
  gui->state=1;
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_rbhide_activate(GtkButton *button, guidata *gui)
{
  gui->state=2;
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
gboolean close_prefs(GtkWidget *widget, GdkEvent *event, guidata *gui)
{
  gtk_widget_hide_on_delete(gui->prefwindow);
  return TRUE;
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void cancel_prefs(GtkButton *button, guidata *gui)
{
  gtk_widget_hide(gui->prefwindow);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_preferences_activate(GtkMenuItem *menuitem, guidata *gui)
{
  gtk_widget_show(gui->prefwindow);

}
