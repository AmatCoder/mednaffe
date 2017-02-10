/*
 * bios.c
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

gchar* get_real_path(gchar *path, guidata *gui)
{
  if (!g_path_is_absolute(path))
  {
    GtkEntry *entry = GTK_ENTRY(gtk_builder_get_object(gui->builder, "-filesys.path_firmware"));
    const gchar *firm_path = gtk_entry_get_text(entry);

    if (g_path_is_absolute(firm_path))
    {
      gchar *path2 = g_strconcat(firm_path, path, NULL);
      return path2;
    }
    else
    {
      const gchar *home;
    #ifdef G_OS_WIN32
      home = g_path_get_dirname(gui->binpath);
    #else
      home = g_getenv ("HOME");
      if (home != NULL) home = g_get_home_dir();
    #endif

    #ifdef G_OS_WIN32
       gchar *path2 = g_strconcat(home, firm_path, G_DIR_SEPARATOR_S, path, NULL);
    #else
       gchar *path2 = g_strconcat(home,"/.mednafen/", firm_path, G_DIR_SEPARATOR_S, path, NULL);
    #endif

       if (!g_file_test (path2, G_FILE_TEST_EXISTS))
       {
         g_free(path2);
      #ifdef G_OS_WIN32
         path2 = g_strconcat(home, path, NULL);
      #else
         path2 = g_strconcat(home,"/.mednafen/", path, NULL);
      #endif
       }
       return path2;
    }
  }
  else
    return g_strdup (path);
}

gchar* get_checksum(gchar *path)
{
  gchar *hash = NULL;
  GMappedFile *mfile = g_mapped_file_new (path, FALSE, NULL);

  if (mfile != NULL)
  {
    gsize len = g_mapped_file_get_length (mfile);
    gchar *content = g_mapped_file_get_contents (mfile);

    hash = g_compute_checksum_for_data (G_CHECKSUM_SHA256,
                                        (const guchar *)content,
                                        len);

    g_mapped_file_unref (mfile);
  }
  return hash;
}

void set_bios(GtkEditable *editable, GdkPixbuf *icon, const gchar *message, guidata *gui)
{
  gchar *cname;

  if (editable != NULL)
  {
    g_object_set(G_OBJECT(editable), "secondary-icon-pixbuf", icon,
                                     "secondary-icon-tooltip-text", message,
                                     "tooltip-text", message,
                                     NULL);

    cname = (gchar *)g_object_get_data (G_OBJECT(editable), "cname");
  }
  else
    cname = "-lynx.bios";

  gchar *str = g_strconcat("label", cname, NULL);
  GtkLabel *label = GTK_LABEL(gtk_builder_get_object(gui->settings, str));
  gtk_label_set_text (label, message);
  g_free(str);

  str = g_strconcat("image", cname, NULL);
  GtkImage *image = GTK_IMAGE(gtk_builder_get_object(gui->settings, str));
  gtk_image_set_from_pixbuf(image, icon);
  g_free(str);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void check_bios(GtkEditable *editable, guidata *gui)
{
  const gchar* const system[7] = { "-pcfx.bios",
                                   "-psx.bios_eu",
                                   "-psx.bios_jp",
                                   "-psx.bios_na",
                                   "-ss.bios_na_eu",
                                   "-ss.bios_jp",
                                   NULL };

  const gchar* const sha256[7] = { "4b44ccf5d84cc83daa2e6a2bee00fdafa14eb58bdf5859e96d8861a891675417",
                                   "1faaa18fa820a0225e488d9f086296b8e6c46df739666093987ff7d8fd352c09",
                                   "9c0421858e217805f4abe18698afea8d5aa36ff0727eb8484944e00eb5e7eadb",
                                   "11052b6499e466bbf0a709b1f9cb6834a9418e66680387912451e971cf8a1fef",
                                   "96e106f740ab448cf89f0dd49dfbac7fe5391cb6bd6e14ad5e3061c13330266f",
                                   "dcfef4b99605f872b6c3b6d05c045385cdea3d1b702906a0ed930df7bcb7deac",
                                   NULL };

  gpointer *cname = g_object_get_data (G_OBJECT(editable), "cname");

  gchar *path = gtk_editable_get_chars (editable, 0, -1);
  gchar *real_path = get_real_path (path, gui);
  g_free(path);
  gchar *hash = get_checksum (real_path);
  g_free(real_path);

  if (hash != NULL)
  {
    gint i = 0;
    while (system[i] != NULL)
    {
      if (g_strcmp0((gchar *)cname, system[i]) == 0)
      {
        if (g_strcmp0(hash, sha256[i]) == 0)
          set_bios(editable, gui->ok, "Correct", gui);
        else
          set_bios(editable, gui->notok, "Not valid", gui);
      }
      i++;
    }
  }
  else
    set_bios(editable, gui->missing, "Not found", gui);

  g_free(hash);
}

void check_lynx_bios(guidata *gui)
{
  gchar *path = get_real_path("lynxboot.img", gui);
  gchar *hash = get_checksum(path);
  g_free(path);

  if (hash != NULL)
  {
    if (g_strcmp0(hash, "c26a36c1990bcf841155e5a6fea4d2ee1a4d53b3cc772e70f257a962ad43b383") == 0)
      set_bios(NULL, gui->ok, "Correct", gui);
    else
      set_bios(NULL, gui->notok, "Not valid", gui);
  }
  else
   set_bios(NULL, gui->missing, "Not found", gui);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void on_bios_check_activate(GtkMenuItem *menuitem, guidata *gui)
{
  check_lynx_bios(gui);
  gtk_widget_show(gui->bioswindow);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
void cancel_bios_check(GtkButton *button, guidata *gui)
{
  gtk_widget_hide(gui->bioswindow);
}

#ifdef G_OS_WIN32
G_MODULE_EXPORT
#endif
gboolean close_bios_check(GtkWidget *widget, GdkEvent *event, guidata *gui)
{
  gtk_widget_hide_on_delete(gui->bioswindow);
  return TRUE;
}
