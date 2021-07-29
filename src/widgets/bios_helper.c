/*
 * bios_helper.c
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


#include "bios_helper.h"

#ifdef G_OS_WIN32
  #include "win32util.h"
#endif


static const gchar*
bios_get_value (GHashTable* table, const gchar* command)
{
  const gchar* value = NULL;

  if (table)
  {
    gchar* command2 = g_strconcat ("-", command, NULL);

    if (g_hash_table_contains (table, command2))
      value = g_hash_table_lookup (table, command2);
    else
      value = g_hash_table_lookup (table, command);

    g_free (command2);
  }

  return value;
}


static gchar*
bios_get_base_path (void)
{
  gchar* mh;

#ifdef G_OS_WIN32
  mh = win32_get_process_directory ();

  if (mh == NULL)
    mh = g_strdup ("");
#else
  const gchar* home = g_getenv ("MEDNAFEN_HOME");

  if (home == NULL)
    mh = g_strconcat (g_get_home_dir (), "/.mednafen", NULL);
  else
    mh = g_strconcat (home, "/", NULL);
#endif

  return mh;
}


static gchar*
bios_get_real_path (GHashTable* table, const gchar* text)
{
  gchar* path = NULL;
  const gchar* firmware = bios_get_value (table, "filesys.path_firmware");

  if (firmware)
  {
    if (!g_path_is_absolute (text))
    {
      if (!g_path_is_absolute (firmware))
      {
        gchar* mh = bios_get_base_path();
        path = g_strconcat (mh, G_DIR_SEPARATOR_S, firmware, G_DIR_SEPARATOR_S, text, NULL);

        if (!g_file_test (path, G_FILE_TEST_IS_REGULAR))
        {
          g_free (path);
          path = g_strconcat (mh, G_DIR_SEPARATOR_S, text, NULL);
        }
        g_free (mh);
      }
      else path = g_strconcat (firmware, G_DIR_SEPARATOR_S, text, NULL);
    }
    else path = g_strdup (text);
  }

  return path;
}


const gchar*
bios_get_icon (const gchar* text)
{
  if (strlen(text) < 14)
    return "/com/github/mednaffe/error.png";

  if (text[13] == 'O')
    return "/com/github/mednaffe/apply.png";
  else if (text[13] == 'n')
    return "/com/github/mednaffe/warning.png";
  else
    return "/com/github/mednaffe/error.png";
}


gchar*
bios_check_sha256 (GHashTable* table, const gchar* command, const gchar* value, const gchar* sha256)
{
  GMappedFile* file = NULL;
  gchar* real_path;

  if (value == NULL)
  {
    if (command == NULL)
      real_path = bios_get_real_path (table, "lynxboot.img");
    else
      real_path = bios_get_real_path (table, bios_get_value (table, command));
  }
  else
    real_path = bios_get_real_path (table, value);

  if (real_path)
    file = g_mapped_file_new (real_path, FALSE, NULL);

  gchar* info;

  if (file == NULL)
  {
    info = g_strconcat ("Bios file not found!\n\nPath:\n", real_path, "\n\nExpected SHA256 checksum:\n", sha256, NULL);
    g_free (real_path);
    return info;
  }

  GBytes* data = g_mapped_file_get_bytes (file);
  gchar* cm = g_compute_checksum_for_bytes (G_CHECKSUM_SHA256, data);

  if (g_strcmp0 (cm, sha256) == 0)
    info = g_strconcat ("Bios file is OK\n\nPath:\n", real_path, "\n\nSHA256 checksum from file:\n", sha256, NULL);

  else
    info = g_strconcat ("Bios file is not correct!\n\nPath:\n", real_path,
                               "\n\nSHA256 checksum from file:\n", cm, "\n\nExpected SHA256 checksum:\n", sha256, NULL);

  g_free (real_path);
  g_free (cm);
  g_bytes_unref (data);
  g_mapped_file_unref (file);

  return info;
}
