/*
 * pathcombobox.c
 *
 * Copyright 2013-2018 AmatCoder
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


#include "pathcombobox.h"
#include "widgets/dialogs.h"


typedef struct _PathComboBoxClass PathComboBoxClass;

struct _PathComboBoxClass {
  GtkBoxClass parent_class;
};


enum  {
  PATH_COMBO_BOX_SELECTED_SIGNAL,
  PATH_COMBO_BOX_NUM_SIGNALS
};

static guint path_combo_box_signals[PATH_COMBO_BOX_NUM_SIGNALS] = {0};


G_DEFINE_TYPE (PathComboBox, path_combo_box, GTK_TYPE_BOX);


static void
path_combo_box_add_to_combo (PathComboBox* self,
                             const gchar* item)
{
  GtkTreeIter iter;

  g_return_if_fail (self != NULL);
  g_return_if_fail (item != NULL);

  gtk_list_store_append (self->path_store, &iter);
  gtk_list_store_set (self->path_store, &iter, 0, item, 1, FALSE, 2, FALSE, 3, "*.*", 4, "", 5, "", 6, 0, 7, 0, -1);

  gtk_combo_box_set_active_iter (self->combo, &iter);
}


static void
path_combo_box_add_clicked (GtkButton* sender,
                            gpointer self)
{
  g_return_if_fail (self != NULL);

  GtkWidget* toplevel = gtk_widget_get_toplevel ((GtkWidget*) self);
  gchar* filename = select_path (toplevel, TRUE);

  if (filename)
  {
    path_combo_box_add_to_combo (self, filename);
    g_free (filename);
  }
}


static void
path_combo_box_changed (GtkComboBox* sender,
                        gpointer self)
{
  g_return_if_fail (self != NULL);

  GtkTreeIter iter;
  gboolean valid;
  PathComboBox* pcb = self;

  valid = gtk_combo_box_get_active_iter (pcb->combo, &iter);

  if (valid)
    g_signal_emit (pcb, path_combo_box_signals[PATH_COMBO_BOX_SELECTED_SIGNAL], 0, &iter);
  else
    g_signal_emit (pcb, path_combo_box_signals[PATH_COMBO_BOX_SELECTED_SIGNAL], 0, NULL);
}


void
path_combo_box_save_panel_position (PathComboBox* self,
                                    gint vpos,
                                    gint hpos)
{
  g_return_if_fail (self != NULL);

  GtkTreeIter iter;
  gboolean valid = gtk_combo_box_get_active_iter (self->combo, &iter);

  if (valid)
    gtk_list_store_set (self->path_store, &iter, 6, vpos, 7, hpos, -1);

}

static const gchar*
bool_to_string (gboolean self)
{
  if (self)
    return "true";
  else
    return "false";
}


gchar**
path_combo_box_get_dirs (PathComboBox* self,
                         int* length)
{
  g_return_val_if_fail (self != NULL, NULL);

  GtkTreeIter iter;
  gint i = 0;
  gchar* res =  g_strdup("");

  gboolean next = gtk_tree_model_get_iter_first ((GtkTreeModel*) self->path_store, &iter);

  while (next)
  {
    gboolean scan;
    gboolean hide;
    gchar* path;
    gchar* filters;
    gchar* sa;
    gchar* sb;
    gint ppv;
    gint pph;

    i++;
    gtk_tree_model_get ((GtkTreeModel*) self->path_store, &iter, 0, &path,
                                                                 1, &scan,
                                                                 2, &hide,
                                                                 3, &filters,
                                                                 4, &sa,
                                                                 5, &sb,
                                                                 6, &ppv,
                                                                 7, &pph,
                                                                 -1);

    gchar* ppv_str = g_strdup_printf ("%i", ppv);
    gchar* pph_str = g_strdup_printf ("%i", pph);

    gchar* tmp = g_strconcat (res, path, ",", bool_to_string (scan),
                                         ",", bool_to_string (hide),
                                         ",", filters,
                                         ",", sa,
                                         ",", sb,
                                         ",", ppv_str,
                                         ",", pph_str,
                                         ";", NULL);
    g_free(res);
    res = tmp;

    g_free (pph_str);
    g_free (ppv_str);
    g_free (sb);
    g_free (sa);
    g_free (filters);
    g_free (path);

    next = gtk_tree_model_iter_next ((GtkTreeModel*) self->path_store, &iter);
  }

  gchar** dirs = g_strsplit(res, ";", -1);
  *length = i;
  g_free (res);

  return dirs;
}


static gboolean
string_to_bool (const gchar* str)
{
  if (g_strcmp0 (str, "true") == 0)
    return TRUE;
  else
    return FALSE;
}


void
path_combo_box_set_dirs (PathComboBox* self,
                         gchar** st,
                         int length)
{
  GtkTreeIter iter;

  g_return_if_fail (self != NULL);

  if (length == 0) return;

  gint i = 0;

      while (TRUE)
      {
        if (!(i < length))
          break;

        gchar** values = g_strsplit (st[i], ",", -1);

        gint a = g_ascii_strtoll (values[6], NULL, 0);
        gint b = g_ascii_strtoll (values[7], NULL, 0);

        gtk_list_store_append (self->path_store, &iter);

        gtk_list_store_set (self->path_store, &iter, 0, values[0],
                                                     1, string_to_bool (values[1]),
                                                     2, string_to_bool (values[2]),
                                                     3, values[3],
                                                     4, values[4],
                                                     5, values[5],
                                                     6, a,
                                                     7, b,
                                                     -1);

        g_strfreev (values);
        i++;
      }

}


PathComboBox*
path_combo_box_new (void)
{
  GtkCellRendererText* renderer;
  GtkButton* button;

  PathComboBox *self = (PathComboBox*) g_object_new (path_combo_box_get_type (), NULL);

  self->path_store = gtk_list_store_new (8, G_TYPE_STRING,
                                            G_TYPE_BOOLEAN,
                                            G_TYPE_BOOLEAN,
                                            G_TYPE_STRING,
                                            G_TYPE_STRING,
                                            G_TYPE_STRING,
                                            G_TYPE_INT,
                                            G_TYPE_INT,
                                            -1);

  self->combo = (GtkComboBox*) gtk_combo_box_new_with_model ((GtkTreeModel*) self->path_store);
  g_object_unref (self->path_store);

  renderer = (GtkCellRendererText*) gtk_cell_renderer_text_new ();

  gtk_cell_layout_pack_start ((GtkCellLayout*) self->combo, (GtkCellRenderer*) renderer, TRUE);
  gtk_cell_layout_add_attribute ((GtkCellLayout*) self->combo, (GtkCellRenderer*) renderer, "text", 0);

  button = (GtkButton*) gtk_button_new_from_icon_name ("list-add", (GtkIconSize) GTK_ICON_SIZE_BUTTON);
  gtk_button_set_label (button, "Add");

  gtk_box_pack_start ((GtkBox*) self, (GtkWidget*) self->combo, TRUE, TRUE, 0);
  gtk_box_pack_end ((GtkBox*) self, (GtkWidget*) button, FALSE, TRUE, 0);
  gtk_box_set_spacing ((GtkBox*) self, 16);

  g_signal_connect_object (button, "clicked", (GCallback) path_combo_box_add_clicked, self, 0);
  g_signal_connect_object (self->combo, "changed", (GCallback) path_combo_box_changed, self, 0);

  gtk_widget_show_all ((GtkWidget*) self);

  return self;
}


static void
path_combo_box_init (PathComboBox * self)
{
}


static void
path_combo_box_class_init (PathComboBoxClass * klass)
{
  path_combo_box_signals[PATH_COMBO_BOX_SELECTED_SIGNAL] = g_signal_new ("selected",
                                                                          path_combo_box_get_type (),
                                                                          G_SIGNAL_RUN_LAST,
                                                                          0,
                                                                          NULL,
                                                                          NULL,
                                                                          g_cclosure_marshal_VOID__POINTER,
                                                                          G_TYPE_NONE,
                                                                          1,
                                                                          G_TYPE_POINTER);
}
