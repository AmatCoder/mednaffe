/*
 * active.c
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

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_entry_color_changed(GtkEditable *editable, gpointer colorbutton)
{

  gchar *text;
  gchar *total;

  text = gtk_editable_get_chars(editable,2,-1);
  total = g_strconcat("#", text, NULL);
#ifdef GTK2_ENABLED
  GdkColor color;
  gdk_color_parse(total, &color);
  gtk_color_button_set_color(GTK_COLOR_BUTTON(colorbutton), &color);
#else
  GdkRGBA color;
  gdk_rgba_parse (&color, total);
  gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(colorbutton), &color);
#endif
  g_free(text);
  g_free(total);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_cbvbdefault_color_set(GtkColorButton *widget, gpointer entry)
{
  gchar *hex;
  gchar *total;

#ifdef GTK2_ENABLED
  GdkColor color;
  gtk_color_button_get_color(widget, &color);
  hex = gtk_color_selection_palette_to_string(&color, 1);
  hex++;
  total = g_strconcat("0x", hex, NULL);
  hex--;
#else
  GdkRGBA color;
  gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(widget), &color);
  hex = gdk_rgba_to_string(&color);
  total = g_strdup_printf ("0x%02X%02X%02X",
                       (unsigned int)(color.red * 255),
                       (unsigned int)(color.green * 255),
                      (unsigned int)(color.blue * 255));
#endif


  gtk_entry_set_text(entry, total);

  g_free(hex);
  g_free(total);

}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_entry_icon_release(GtkEntry *entry,
                           GtkEntryIconPosition icon_pos,
                           GdkEvent *event, gpointer user_data)
{
  gtk_entry_set_text(entry,"");
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_button_entry_clicked_folder(GtkButton *button, GtkEntry *entry)
{
  GtkWidget *folder;

#ifdef GTK2_ENABLED
  folder = gtk_file_chooser_dialog_new(
    "Choose a folder...", NULL,
    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, GTK_STOCK_CANCEL,
    GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
    NULL);
#else
  folder = gtk_file_chooser_dialog_new(
    "Choose a folder...", NULL,
    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, ("_Cancel"),
    GTK_RESPONSE_CANCEL, ("_Open"), GTK_RESPONSE_ACCEPT,
    NULL);
#endif

  if (gtk_dialog_run(GTK_DIALOG(folder)) == GTK_RESPONSE_ACCEPT)
  {
    gchar *path;

    path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (folder));
    gtk_entry_set_text(entry, path);
    g_free(path);
  }
  gtk_widget_destroy(folder);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_button_entry_clicked(GtkButton *button, GtkEntry *entry)
{
  GtkWidget *folder;

#ifdef GTK2_ENABLED
  folder = gtk_file_chooser_dialog_new(
    "Choose a file...", NULL,
    GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL,
    GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
#else
  folder = gtk_file_chooser_dialog_new(
    "Choose a file...", NULL,
    GTK_FILE_CHOOSER_ACTION_OPEN, ("_Cancel"),
    GTK_RESPONSE_CANCEL, ("_Open"), GTK_RESPONSE_ACCEPT, NULL);
#endif

  if (gtk_dialog_run(GTK_DIALOG(folder)) == GTK_RESPONSE_ACCEPT)
  {
    gchar *path;

    path = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (folder));
    gtk_entry_set_text(entry, path);
    g_free(path);
  }
  gtk_widget_destroy(folder);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void cbvideodrv_changed_cb(GtkComboBox *combobox, GtkWidget *widget)
{
  if (gtk_combo_box_get_active(combobox) == 2)
    gtk_widget_set_sensitive(widget, FALSE);
  else
    gtk_widget_set_sensitive(widget, TRUE);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_tbsound_toggled(GtkToggleButton *sender, GtkWidget *widget)
{
  GList *list = NULL;

  list = gtk_container_get_children(GTK_CONTAINER(widget));
  list = g_list_remove(list, sender);

  while (list)
  {
    gtk_widget_set_sensitive(
      list->data, gtk_toggle_button_get_active(sender));

    list = list->next;
  }
  g_list_free (list);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_tblur_toggled(GtkToggleButton *sender, GtkWidget *widget)
{
  gtk_widget_set_sensitive(widget, gtk_toggle_button_get_active(sender));
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_ntsc_changed(GtkComboBox *combobox, GtkWidget *widget)
{
  if ((gtk_combo_box_get_active(combobox)==0) || (gtk_combo_box_get_active(combobox)==5))
    gtk_widget_set_sensitive(widget, TRUE);
  else
    gtk_widget_set_sensitive(widget, FALSE);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_vbmode2_changed(GtkComboBox *combobox, GtkWidget *widget)
{
  if (gtk_combo_box_get_active(combobox)!=0)
    gtk_widget_set_sensitive(widget, TRUE);
  else
    gtk_widget_set_sensitive(widget, FALSE);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_shader_changed(GtkComboBox *combobox, GtkWidget *widget)
{
  if (gtk_combo_box_get_active(combobox) == 10)
    gtk_widget_set_sensitive(widget, TRUE);
  else
    gtk_widget_set_sensitive(widget, FALSE);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_audio_changed(GtkComboBox *combobox, GtkWidget *widget)
{
  if (
    #ifdef G_OS_WIN32
      (gtk_combo_box_get_active(combobox)==0) ||
    #endif
      (gtk_combo_box_get_active(combobox)==3) || 
      (gtk_combo_box_get_active(combobox)==4) ||
      (gtk_combo_box_get_active(combobox)==7))
    gtk_widget_set_sensitive(widget, FALSE);
  else
    gtk_widget_set_sensitive(widget, TRUE);
}
