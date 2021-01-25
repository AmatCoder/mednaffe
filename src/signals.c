/*
 * signals.c
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


#include <gtk/gtk.h>


static void
show_stack (GtkStack *stack,
            guint n_port)
{
  GtkWidget* ports[] = {
    gtk_stack_get_child_by_name (stack, "port3"),
    gtk_stack_get_child_by_name (stack, "port4"),
    gtk_stack_get_child_by_name (stack, "port5"),
    gtk_stack_get_child_by_name (stack, "port6"),
    gtk_stack_get_child_by_name (stack, "port7"),
    gtk_stack_get_child_by_name (stack, "port8"),
    gtk_stack_get_child_by_name (stack, "port9"),
    gtk_stack_get_child_by_name (stack, "port10"),
    gtk_stack_get_child_by_name (stack, "port11"),
    gtk_stack_get_child_by_name (stack, "port12"),
    NULL
  };

  guint i = 0;

  while (ports[i] != NULL)
  {
    if (i < n_port)
      gtk_widget_show (ports[i]);
    else
      gtk_widget_hide (ports[i]);

    i++;
  }
}


G_MODULE_EXPORT
void
medcombo_changed (GtkComboBox* combo,
                  const gchar* value,
                  gpointer stack)
{
  gtk_stack_set_visible_child_name ((GtkStack*) stack, value);
}


G_MODULE_EXPORT
void
enable_buddy_toggle (GtkToggleButton *toggle,
                     gpointer *buddy)
{
  gtk_widget_set_sensitive ((GtkWidget*) buddy, gtk_toggle_button_get_active (toggle));
}


G_MODULE_EXPORT
void
enable_buddy_menu (GtkCheckMenuItem *checkmenuitem,
                   gpointer *buddy)
{
  gtk_widget_set_visible ((GtkWidget*) buddy, gtk_check_menu_item_get_active (checkmenuitem));
}


G_MODULE_EXPORT
void
enable_buddy_combo_opengl (GtkComboBox *combo,
                           const gchar* value,
                           gpointer *buddy)
{
  gboolean ogl = (g_strcmp0 (value, "default") == 0) ||
                 (g_strcmp0 (value, "opengl") == 0);

  gboolean goat = (g_strcmp0 (value, "goat") == 0);

  gboolean scale = (g_strcmp0 (value, "0") == 0);

  if ( ogl || goat || scale)
    gtk_widget_set_sensitive ((GtkWidget*) buddy, TRUE);
  else
    gtk_widget_set_sensitive ((GtkWidget*) buddy, FALSE);
}


G_MODULE_EXPORT
void
enable_ports_combo (GtkComboBox *combo,
                    const gchar* value,
                    GtkStack *stack)
{
  if (gtk_stack_get_child_by_name (stack, "global_md") != NULL)
  {
    if (g_strcmp0 (value, "none") == 0)
      show_stack(stack, 0);
    else if (g_strcmp0 (value, "tp1") == 0)
      show_stack(stack, 3);
    else if (g_strcmp0 (value, "tp2") == 0)
      show_stack(stack, 3);
    else if (g_strcmp0 (value, "tpd") == 0)
      show_stack(stack, 6);
    else if (g_strcmp0 (value, "4way") == 0)
      show_stack(stack, 2);
  }
}


G_MODULE_EXPORT
void
enable_ports_toggle_saturn (GtkToggleButton *toggle,
                            GtkStack *stack)
{
  if (gtk_toggle_button_get_active (toggle))
  {
    if (gtk_widget_is_visible (gtk_stack_get_child_by_name (stack, "port3") ))
      show_stack (stack, 10);
    else
      show_stack (stack, 5);
  }
  else
  {
    gboolean visible = FALSE;
    GtkWidget* port12 = gtk_stack_get_child_by_name (stack, "port12");

    if (port12 != NULL)
      visible = gtk_widget_is_visible (port12);

    if (visible)
      show_stack (stack, 5);
    else
      show_stack (stack, 0);
  }
}


G_MODULE_EXPORT
void
enable_ports_toggle_nes (GtkToggleButton *toggle,
                         GtkStack *stack)
{
    GtkWidget *port3 = gtk_stack_get_child_by_name (stack, "port3");
    GtkWidget *port4 = gtk_stack_get_child_by_name (stack, "port4");

  if (gtk_toggle_button_get_active (toggle))
  {
    gtk_widget_hide (port3);
    gtk_widget_hide (port4);
  }
  else
  {
    gtk_widget_show (port3);
    gtk_widget_show (port4);
  }
}


G_MODULE_EXPORT
void
enable_ports_toggle (GtkToggleButton *toggle,
                     GtkStack *stack)
{
  if (gtk_toggle_button_get_active (toggle))
  {
    if (gtk_widget_is_visible (gtk_stack_get_child_by_name (stack, "port3") ))
      show_stack (stack, 6);
    else
      show_stack (stack, 3);
  }
  else
  {
    gboolean visible = FALSE;
    GtkWidget* port8 = gtk_stack_get_child_by_name (stack, "port8");

    if (port8 != NULL)
      visible = gtk_widget_is_visible (port8);

    if (visible)
      show_stack (stack, 3);
    else
      show_stack (stack, 0);
  }
}


G_MODULE_EXPORT
void
custom_entry_icon_press (GtkEntry *entry,
                         GtkEntryIconPosition icon_pos,
                         GdkEvent *event,
                         gpointer data)
{
  gtk_entry_set_text (entry, "");
}


G_MODULE_EXPORT
void
vb_mode_changed (GtkComboBox* combo,
                 const gchar* value,
                 gpointer box)
{
  GList *list = gtk_container_get_children (box);

  gboolean b = (g_strcmp0 (value, "anaglyph") == 0);
  gpointer p = g_object_get_data (box, "preset");

  if (GPOINTER_TO_INT(p) == 0)
    gtk_widget_set_sensitive (g_list_nth_data (list, 1), b);

  gtk_widget_set_sensitive (g_list_nth_data (list, 0), b);
  gtk_widget_set_sensitive (g_list_nth_data (list, 2), !b);

  g_list_free (list);
}


G_MODULE_EXPORT
void
vb_colors_changed (GtkComboBox* combo,
                   const gchar* value,
                   gpointer box)
{
  GList *list = gtk_container_get_children (box);

  gint ip = g_strcmp0 (value, "disabled");
  g_object_set_data (box, "preset", GINT_TO_POINTER(ip));
  gtk_widget_set_sensitive (g_list_nth_data (list, 1), (ip == 0));

  g_list_free (list);
}
