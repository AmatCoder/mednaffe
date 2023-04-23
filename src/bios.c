/*
 * bios.c
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


#include "bios.h"
#include "widgets/bios_helper.h"


typedef struct _BiosWindowClass BiosWindowClass;
typedef struct _BiosWindowPrivate BiosWindowPrivate;

struct _BiosWindowClass {
  GtkDialogClass parent_class;
};

struct _BiosWindowPrivate {
  GtkNotebook* bios_notebook;

  GtkLabel* label_lynx_bios;
  GtkImage* icon_lynx_bios;

  GtkLabel* label_pccdbios;
  GtkImage* icon_pccdbios;

  GtkLabel* label_pcfx_bios;
  GtkImage* icon_pcfx_bios;

  GtkLabel* label_ss_bios_na_eu;
  GtkImage* icon_ss_bios_na_eu;
  GtkLabel* label_ss_bios_jp;
  GtkImage* icon_ss_bios_jp;


  GtkLabel* label_psx_bios_jp;
  GtkImage* icon_psx_bios_jp;
  GtkLabel* label_psx_bios_na;
  GtkImage* icon_psx_bios_na;
  GtkLabel* label_psx_bios_eu;
  GtkImage* icon_psx_bios_eu;
};


G_DEFINE_TYPE_WITH_PRIVATE (BiosWindow, bios_window, GTK_TYPE_DIALOG);


static void
bios_window_set_label (GHashTable* table, GtkLabel* label, GtkImage* image, const gchar* command, const gchar* sha256)
{
  gchar* info = bios_check_sha256 (table, command, NULL, sha256);
  gtk_label_set_label (label, info);
  gtk_image_set_from_resource (image, bios_get_icon (info));
  g_free(info);
}


static void
bios_window_update (BiosWindow* bw,
                    GHashTable* table)
{
  g_return_if_fail (bw != NULL);

  BiosWindowPrivate* priv = bios_window_get_instance_private (bw);

  bios_window_set_label (table, priv->label_lynx_bios, priv->icon_lynx_bios, NULL, "c26a36c1990bcf841155e5a6fea4d2ee1a4d53b3cc772e70f257a962ad43b383");

  bios_window_set_label (table, priv->label_pccdbios, priv->icon_pccdbios, "pce.cdbios", "e11527b3b96ce112a037138988ca72fd117a6b0779c2480d9e03eaebece3d9ce");

  bios_window_set_label (table, priv->label_pcfx_bios, priv->icon_pcfx_bios, "pcfx.bios", "4b44ccf5d84cc83daa2e6a2bee00fdafa14eb58bdf5859e96d8861a891675417");

  bios_window_set_label (table, priv->label_ss_bios_na_eu, priv->icon_ss_bios_na_eu, "ss.bios_na_eu", "96e106f740ab448cf89f0dd49dfbac7fe5391cb6bd6e14ad5e3061c13330266f");
  bios_window_set_label (table, priv->label_ss_bios_jp, priv->icon_ss_bios_jp, "ss.bios_jp", "dcfef4b99605f872b6c3b6d05c045385cdea3d1b702906a0ed930df7bcb7deac");

  bios_window_set_label (table, priv->label_psx_bios_jp, priv->icon_psx_bios_jp, "psx.bios_jp", "9c0421858e217805f4abe18698afea8d5aa36ff0727eb8484944e00eb5e7eadb");
  bios_window_set_label (table, priv->label_psx_bios_na, priv->icon_psx_bios_na, "psx.bios_na", "11052b6499e466bbf0a709b1f9cb6834a9418e66680387912451e971cf8a1fef");
  bios_window_set_label (table, priv->label_psx_bios_eu, priv->icon_psx_bios_eu, "psx.bios_eu", "1faaa18fa820a0225e488d9f086296b8e6c46df739666093987ff7d8fd352c09");
}


static void
bios_window_on_system_changed (GtkTreeSelection* sender,
                               gpointer self)
{
  g_return_if_fail (self != NULL);

  BiosWindow* bw = self;
  BiosWindowPrivate* priv = bios_window_get_instance_private (bw);

  GtkTreeModel* model;
  GtkTreeIter iter;

  gboolean valid = gtk_tree_selection_get_selected (sender, &model, &iter);

  if (valid)
  {
    gint num;
    gtk_tree_model_get (model, &iter, 1, &num, -1);
    gtk_notebook_set_current_page (priv->bios_notebook, num);
  }
}


static gboolean
bios_window_on_close_x (GtkWidget* sender,
                        GdkEventAny* event,
                        gpointer self)
{
  gtk_widget_destroy ((GtkWidget*) self);
  return TRUE;
}


static void
bios_window_on_close (GtkButton* sender,
                      gpointer self)
{
  gtk_widget_destroy ((GtkWidget*) self);
}


BiosWindow*
bios_window_new (GtkWindow* parent,
                 GHashTable* table)
{
  g_return_val_if_fail (parent != NULL, NULL);

  BiosWindow* self = (BiosWindow*) g_object_new (bios_window_get_type (), NULL);

  gtk_window_set_transient_for ((GtkWindow*) self, parent);

  bios_window_update (self, table);

  return self;
}


static void
bios_window_init (BiosWindow* self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}


static void
bios_window_class_init (BiosWindowClass* klass)
{
  gint BiosWindow_private_offset = g_type_class_get_instance_private_offset (klass);

  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (klass), "/com/github/mednaffe/Bios.ui");

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "bios_notebook",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, bios_notebook));


  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "label_lynx_bios",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label_lynx_bios));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "icon_lynx_bios",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, icon_lynx_bios));


  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "label_pccdbios",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label_pccdbios));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "icon_pccdbios",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, icon_pccdbios));


  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "label_pcfx_bios",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label_pcfx_bios));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "icon_pcfx_bios",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, icon_pcfx_bios));


  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "label_ss_bios_jp",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label_ss_bios_jp));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "icon_ss_bios_jp",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, icon_ss_bios_jp));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "label_ss_bios_na_eu",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label_ss_bios_na_eu));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "icon_ss_bios_na_eu",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, icon_ss_bios_na_eu));


  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "label_psx_bios_jp",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label_psx_bios_jp));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "icon_psx_bios_jp",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, icon_psx_bios_jp));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "label_psx_bios_na",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label_psx_bios_na));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "icon_psx_bios_na",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, icon_psx_bios_na));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "label_psx_bios_eu",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label_psx_bios_eu));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "icon_psx_bios_eu",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, icon_psx_bios_eu));


  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "on_system_changed",
                                                G_CALLBACK(bios_window_on_system_changed));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "on_close_x",
                                                G_CALLBACK(bios_window_on_close_x));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "on_close",
                                                G_CALLBACK(bios_window_on_close));
}
