/*
 * bios.c
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


#include "bios.h"


typedef struct _BiosWindowClass BiosWindowClass;
typedef struct _BiosWindowPrivate BiosWindowPrivate;

struct _BiosWindowClass {
  GtkDialogClass parent_class;
};

struct _BiosWindowPrivate {
  GtkNotebook* bios_notebook;

  GtkLabel* label_lynx_bios;
  GtkLabel* label2_lynx_bios;
  GtkImage* icon_lynx_bios;

  GtkLabel* label_pccdbios;
  GtkLabel* label2_pccdbios;
  GtkImage* icon_pccdbios;

  GtkLabel* label_pcfx_bios;
  GtkLabel* label2_pcfx_bios;
  GtkImage* icon_pcfx_bios;

  GtkLabel* label_ss_bios_jp;
  GtkLabel* label2_ss_bios_jp;
  GtkImage* icon_ss_bios_jp;
  GtkLabel* label_ss_bios_na_eu;
  GtkLabel* label2_ss_bios_na_eu;
  GtkImage* icon_ss_bios_na_eu;

  GtkLabel* label_psx_bios_jp;
  GtkLabel* label2_psx_bios_jp;
  GtkImage* icon_psx_bios_jp;
  GtkLabel* label_psx_bios_na;
  GtkLabel* label2_psx_bios_na;
  GtkImage* icon_psx_bios_na;
  GtkLabel* label_psx_bios_eu;
  GtkLabel* label2_psx_bios_eu;
  GtkImage* icon_psx_bios_eu;
};

typedef enum  {
  LYNX,
  GB,
  PCECD,
  PCFX,
  SS_JP,
  SS_NA_EU,
  PSX_JP,
  PSX_NA,
  PSX_EU
} BiosSystems;


G_DEFINE_TYPE_WITH_PRIVATE (BiosWindow, bios_window, GTK_TYPE_DIALOG);

static GType
bios_systems_get_type (void)
{
  static volatile gsize bios_systems_type_id__volatile = 0;

  if (g_once_init_enter (&bios_systems_type_id__volatile))
  {
    static const GEnumValue values[] = { {LYNX, "LYNX", "-lynx_bios"},
                                         {PCECD, "PCE", "-pce.cdbios"},
                                         {PCFX, "PCFX", "-pcfx.bios"},
                                         {SS_JP, "SS_JP", "-ss.bios_jp"},
                                         {SS_NA_EU, "SS_NA_EU", "-ss.bios_na_eu"},
                                         {PSX_JP, "PSX_JP", "-psx.bios_jp"},
                                         {PSX_NA, "PSX_NA", "-psx.bios_na"},
                                         {PSX_EU, "PSX_EU", "-psx.bios_eu"},
                                         {0, NULL, NULL} };

    GType bios_systems_type_id = g_enum_register_static (g_intern_static_string ("BiosSystems"), values);
    g_once_init_leave (&bios_systems_type_id__volatile, bios_systems_type_id);
  }
  return bios_systems_type_id__volatile;
}


static void
bios_window_set_labels (BiosWindow* self,
                        MedWidget* medwid,
                        GtkLabel* label1,
                        GtkLabel* label2,
                        GtkImage* image)
{
  gchar* tooltip;
  gchar* icon;

  g_return_if_fail (self != NULL);
  g_return_if_fail (medwid != NULL);
  g_return_if_fail (label1 != NULL);
  g_return_if_fail (label2 != NULL);
  g_return_if_fail (image != NULL);

  tooltip = med_bios_entry_get_tooltip ((MedBiosEntry*) medwid);
  gtk_label_set_label (label2, tooltip);

  g_free (tooltip);

  icon = med_bios_entry_get_icon ((MedBiosEntry*) medwid);
  gtk_image_set_from_icon_name (image, icon, (GtkIconSize) GTK_ICON_SIZE_LARGE_TOOLBAR);

  g_free (icon);
}


static void
update_gfunc (gpointer data,
              gpointer self)
{
  g_return_if_fail (data != NULL);

  MedWidget* medwid = data;
  BiosWindow *bw = self;
  BiosWindowPrivate* priv = bios_window_get_instance_private (bw);

  if (IS_MED_BIOS_ENTRY(medwid))
  {
    const gchar* s = med_widget_get_command (medwid);

    GEnumClass *enum_class = g_type_class_ref (bios_systems_get_type());
    GEnumValue *enum_value = g_enum_get_value_by_nick (g_type_class_peek_static (bios_systems_get_type ()), s);

    if (enum_value)
    {
      switch (enum_value->value)
      {
        case LYNX:
          bios_window_set_labels (bw, medwid, priv->label_lynx_bios, priv->label2_lynx_bios, priv->icon_lynx_bios);
        break;
        case PCECD:
          bios_window_set_labels (bw, medwid, priv->label_pccdbios, priv->label2_pccdbios, priv->icon_pccdbios);
        break;
        case PCFX:
          bios_window_set_labels (bw, medwid, priv->label_pcfx_bios, priv->label2_pcfx_bios, priv->icon_pcfx_bios);
        break;
        case SS_JP:
          bios_window_set_labels (bw, medwid, priv->label_ss_bios_jp, priv->label2_ss_bios_jp, priv->icon_ss_bios_jp);
        break;
        case SS_NA_EU:
          bios_window_set_labels (bw, medwid, priv->label_ss_bios_na_eu, priv->label2_ss_bios_na_eu, priv->icon_ss_bios_na_eu);
        break;
        case PSX_JP:
        bios_window_set_labels (bw, medwid, priv->label_psx_bios_jp, priv->label2_psx_bios_jp, priv->icon_psx_bios_jp);
        break;
        case PSX_NA:
            bios_window_set_labels (bw, medwid, priv->label_psx_bios_na, priv->label2_psx_bios_na, priv->icon_psx_bios_na);
        break;
        case PSX_EU:
            bios_window_set_labels (bw, medwid, priv->label_psx_bios_eu, priv->label2_psx_bios_eu, priv->icon_psx_bios_eu);
        break;
      }
    }

    g_type_class_unref (enum_class);
  }
}


void
bios_window_update (BiosWindow* self,
                    GSList* list)
{
  g_return_if_fail (self != NULL);
  g_slist_foreach (list, update_gfunc, self);
}


static void
bios_window_on_system_changed (GtkTreeSelection* sender,
                               gpointer self)
{
  GtkTreeModel* model;
  GtkTreeIter iter;
  gint num;

  g_return_if_fail (self != NULL);
  g_return_if_fail (sender != NULL);

  BiosWindow* bw = self;
  BiosWindowPrivate* priv = bios_window_get_instance_private (bw);

  gboolean valid = gtk_tree_selection_get_selected (sender, &model, &iter);

  if (valid)
  {
    gtk_tree_model_get (model, &iter, 1, &num, -1);
    gtk_notebook_set_current_page (priv->bios_notebook, num);
  }

}


static gboolean
bios_window_on_close_x (GtkWidget* sender,
                        GdkEventAny* event,
                        gpointer self)
{
  gtk_widget_set_visible ((GtkWidget*) self, FALSE);
  return TRUE;
}


static void
bios_window_on_close (GtkButton* sender,
                      gpointer self)
{
  g_return_if_fail (self != NULL);
  gtk_widget_set_visible ((GtkWidget*) self, FALSE);
}


BiosWindow*
bios_window_new (GtkWindow* parent)
{
  g_return_val_if_fail (parent != NULL, NULL);

  BiosWindow * self = (BiosWindow*) g_object_new (bios_window_get_type (), NULL);

  gtk_window_set_transient_for ((GtkWindow*) self, parent);

  return self;
}


static void
bios_window_init (BiosWindow * self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}


static void
bios_window_class_init (BiosWindowClass * klass)
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
                                             "label2_lynx_bios",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label2_lynx_bios));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "icon_lynx_bios",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, icon_lynx_bios));


  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "label_pccdbios",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label_pccdbios));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "label2_pccdbios",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label2_pccdbios));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "icon_pccdbios",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, icon_pccdbios));


  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "label_pcfx_bios",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label_pcfx_bios));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "label2_pcfx_bios",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label2_pcfx_bios));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "icon_pcfx_bios",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, icon_pcfx_bios));


  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "label_ss_bios_jp",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label_ss_bios_jp));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "label2_ss_bios_jp",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label2_ss_bios_jp));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "icon_ss_bios_jp",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, icon_ss_bios_jp));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "label_ss_bios_na_eu",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label_ss_bios_na_eu));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "label2_ss_bios_na_eu",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label2_ss_bios_na_eu));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "icon_ss_bios_na_eu",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, icon_ss_bios_na_eu));


  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "label_psx_bios_jp",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label_psx_bios_jp));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "label2_psx_bios_jp",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label2_psx_bios_jp));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "icon_psx_bios_jp",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, icon_psx_bios_jp));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "label_psx_bios_na",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label_psx_bios_na));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "label2_psx_bios_na",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label2_psx_bios_na));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "icon_psx_bios_na",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, icon_psx_bios_na));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "label_psx_bios_eu",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label_psx_bios_eu));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "label2_psx_bios_eu",
                                             FALSE,
                                             BiosWindow_private_offset + G_STRUCT_OFFSET (BiosWindowPrivate, label2_psx_bios_eu));

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
