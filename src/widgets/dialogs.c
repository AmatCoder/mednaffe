/*
 * dialogs.c
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


#include <widgets/dialogs.h>

/*
#ifdef G_OS_WIN32
  #include <gdk/gdkwin32.h>
  #include <shlobj.h>
#endif
*/


gchar*
select_path (GtkWidget* parent,
             gboolean is_folder)
{
  gchar* filename = NULL;
  GtkFileChooserAction action;

  if (is_folder)
    action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
  else
    action = GTK_FILE_CHOOSER_ACTION_OPEN;

#ifdef G_OS_WIN32

  GtkFileChooserNative *dialog;

  dialog = gtk_file_chooser_native_new ("Select...",
                                        (GtkWindow*) parent,
                                        action,
                                        "_Open",
                                        "_Cancel");


  if (gtk_native_dialog_run ((GtkNativeDialog*) dialog) == ((gint) GTK_RESPONSE_ACCEPT))
    filename = gtk_file_chooser_get_filename ((GtkFileChooser*) dialog);

  g_object_unref (dialog);


  /*
  gchar* ret = NULL;

  BROWSEINFO br;
  ZeroMemory(&br, sizeof(BROWSEINFO));
  br.lpfn = NULL;
  br.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
  br.hwndOwner = gdk_win32_window_get_handle (gtk_widget_get_window (parent) );
  br.lpszTitle = "Select a folder";

  LPITEMIDLIST pidl = NULL;
  if ((pidl = SHBrowseForFolder(&br)) != NULL)
  {
    wchar_t buffer[MAX_PATH];
    if (SHGetPathFromIDList(pidl, (LPSTR)buffer)) ret = (gchar*)buffer;
  }

  return g_strdup (ret);
  */

#else

  GtkWidget *dialog;

  dialog = gtk_file_chooser_dialog_new ("Select...",
                                        (GtkWindow*) parent,
                                        action,
                                        "_Open",
                                        GTK_RESPONSE_ACCEPT,
                                        "_Cancel",
                                        GTK_RESPONSE_CANCEL,
                                        NULL);

  if (gtk_dialog_run ((GtkDialog*) dialog) == ((gint) GTK_RESPONSE_ACCEPT))
    filename = gtk_file_chooser_get_filename ((GtkFileChooser*) dialog);

  gtk_widget_destroy (dialog);

#endif

  return filename;
}
