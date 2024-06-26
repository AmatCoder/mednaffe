/*
 * manager.c
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


#include "manager.h"
#include "setup.h"


typedef struct _ManagerWindowClass ManagerWindowClass;
typedef struct _ManagerWindowPrivate ManagerWindowPrivate;

struct _ManagerWindowClass {
  GtkWindowClass parent_class;
};

struct _ManagerWindowPrivate {
  GtkTreeView* treeview;
  GtkTreeSelection* path_selection;
  GtkButton* up_button;
  GtkButton* down_button;
  GtkButton* setup_button;
  GtkButton* delete_button;
  GtkComboBox* combo;
};


G_DEFINE_TYPE_WITH_PRIVATE (ManagerWindow, manager_window, GTK_TYPE_WINDOW);


static gboolean
gtk_tree_iter_not_equal (const GtkTreeIter * s1,
                         const GtkTreeIter * s2)
{
  if ((s1 == NULL) || (s2 == NULL))
    return TRUE;

  if ((s1->stamp != s2->stamp) ||
       (s1->user_data != s2->user_data) ||
       (s1->user_data2 != s2->user_data2) ||
       (s1->user_data3 != s2->user_data3))
    return TRUE;

  return FALSE;
}


static void
manager_window_set_buttons (ManagerWindow* self,
                            GtkTreeIter* iter)
{
  g_return_if_fail (iter != NULL);

  ManagerWindowPrivate* priv = manager_window_get_instance_private (self);

  GtkTreeModel* model = gtk_combo_box_get_model (priv->combo);

  GtkTreeIter iter2;
  gboolean valid = gtk_tree_model_get_iter_first (model, &iter2);

  gtk_widget_set_sensitive ((GtkWidget*) priv->up_button, gtk_tree_iter_not_equal (iter, &iter2));
  gtk_widget_set_sensitive ((GtkWidget*) priv->down_button, gtk_tree_model_iter_next (model, iter));
  gtk_widget_set_sensitive ((GtkWidget*) priv->setup_button, valid);
  gtk_widget_set_sensitive ((GtkWidget*) priv->delete_button, valid);
}


static void
manager_window_selection_changed (GtkTreeSelection* sender,
                                  gpointer self)
{
  ManagerWindow* mw = self;
  ManagerWindowPrivate* priv = manager_window_get_instance_private (mw);

  GtkTreeIter iter;
  gboolean valid = gtk_tree_selection_get_selected (priv->path_selection, NULL, &iter);

  if (valid)
  {
    gtk_combo_box_set_active_iter (priv->combo, &iter);
    manager_window_set_buttons (mw, &iter);
  }
}

static void
manager_window_row_changed (SetupWindow* sender,
                            gpointer self)
{
  ManagerWindowPrivate* priv = manager_window_get_instance_private (self);

  gtk_combo_box_set_active_iter (priv->combo, NULL);
  manager_window_selection_changed (NULL, self);
}


static void
manager_window_realize (ManagerWindow* mw)
{
  ManagerWindowPrivate* priv = manager_window_get_instance_private (mw);

  GtkTreeIter iter;
  gboolean valid = gtk_combo_box_get_active_iter (priv->combo, &iter);

  if (valid)
  {
    gtk_tree_selection_select_iter (priv->path_selection, &iter);
    gtk_widget_grab_focus ((GtkWidget*) priv->treeview);
    manager_window_set_buttons (mw, &iter);
  }

  g_signal_connect_object (priv->path_selection, "changed", (GCallback) manager_window_selection_changed, mw, 0);
}


static void
manager_window_up_down_clicked (GtkButton* sender,
                                 gpointer self)
{
  ManagerWindow* mw = self;
  ManagerWindowPrivate* priv = manager_window_get_instance_private(mw);

  GtkTreeIter iter;
  gboolean valid = gtk_tree_selection_get_selected (priv->path_selection, NULL, &iter);

  if (valid)
  {
    GtkTreeIter iter2;
    GtkTreeModel* model = gtk_combo_box_get_model (priv->combo);

    iter2 = iter;

    if (sender == priv->up_button)
      gtk_tree_model_iter_previous (gtk_combo_box_get_model (priv->combo), &iter);
    else if (sender == priv->down_button)
      gtk_tree_model_iter_next (gtk_combo_box_get_model (priv->combo), &iter);

    gtk_tree_store_swap ((GtkTreeStore*) model, &iter, &iter2);

    manager_window_set_buttons (mw, &iter2);
  }
}


static gint
manager_window_show_confirmation (gpointer* self)
{
  GtkWidget* dialog = gtk_message_dialog_new (gtk_window_get_transient_for((GtkWindow*) self),
                                              GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
                                              "Are you sure?");

  gint result = gtk_dialog_run ((GtkDialog*) dialog);

  gtk_widget_destroy (dialog);

  return result;
}


static void
manager_window_delete_row (GtkButton* sender,
                           gpointer self)
{
  ManagerWindowPrivate* priv = manager_window_get_instance_private (self);

  GtkTreeModel* model = gtk_combo_box_get_model (priv->combo);

  GtkTreeIter iter;
  gboolean valid = gtk_tree_selection_get_selected (priv->path_selection, NULL, &iter);

  if ( (valid) && (manager_window_show_confirmation (self) == GTK_RESPONSE_YES) )
    gtk_tree_store_remove  ((GtkTreeStore*) model, &iter);

  valid = gtk_tree_model_get_iter_first (model, &iter);

  gtk_widget_set_sensitive ((GtkWidget*) priv->setup_button, valid);
  gtk_widget_set_sensitive ((GtkWidget*) priv->delete_button, valid);
}


static gboolean
manager_window_on_close_x (GtkWidget* sender,
                           GdkEventAny* event,
                           gpointer self)
{
  gtk_widget_destroy ((GtkWidget*) self);

  return TRUE;
}


static void
manager_window_on_close (GtkButton* sender,
                          gpointer self)
{
  gtk_widget_destroy ((GtkWidget*) self);
}


static void
manager_window_show_setup (GtkButton* sender,
                           gpointer self)
{
  ManagerWindowPrivate* priv = manager_window_get_instance_private (self);

  GtkTreeIter iter;
  gboolean valid = gtk_tree_selection_get_selected (priv->path_selection, NULL, &iter);

  if (valid)
  {
    GtkTreeModel* model = gtk_combo_box_get_model (priv->combo);
    SetupWindow* setup = setup_window_new (self, model, &iter);

    g_signal_connect_object (setup, "row-has-changed", (GCallback) manager_window_row_changed, self, 0);
  }
}


ManagerWindow*
manager_window_new (GtkWindow* parent,
                    GtkComboBox* combo)
{
  g_return_val_if_fail (parent != NULL, NULL);
  g_return_val_if_fail (combo != NULL, NULL);

  ManagerWindow *self = (ManagerWindow*) g_object_new (manager_window_get_type (), NULL);
  ManagerWindowPrivate* priv = manager_window_get_instance_private (self);

  gtk_window_set_transient_for ((GtkWindow*) self, parent);

  priv->combo = combo;
  GtkTreeModel* model = gtk_combo_box_get_model (priv->combo);
  gtk_tree_view_set_model (priv->treeview, model);

  gtk_button_set_image (priv->up_button, gtk_image_new_from_icon_name ("go-up", (GtkIconSize) GTK_ICON_SIZE_BUTTON));
  gtk_button_set_image (priv->down_button, gtk_image_new_from_icon_name ("go-down", (GtkIconSize) GTK_ICON_SIZE_BUTTON));
  gtk_button_set_image (priv->delete_button, gtk_image_new_from_icon_name ("edit-delete", (GtkIconSize) GTK_ICON_SIZE_BUTTON));
  gtk_button_set_image (priv->setup_button, gtk_image_new_from_icon_name ("document-page-setup", (GtkIconSize) GTK_ICON_SIZE_BUTTON));

  manager_window_realize (self);

  return self;
}


static void
manager_window_init (ManagerWindow * self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}


static void
manager_window_class_init (ManagerWindowClass * klass)
{
  gint ManagerWindow_private_offset = g_type_class_get_instance_private_offset (klass);
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (klass), "/com/github/mednaffe/Manager.ui");

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "treeview",
                                             FALSE,
                                             ManagerWindow_private_offset + G_STRUCT_OFFSET (ManagerWindowPrivate, treeview));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "path_selection",
                                             FALSE,
                                             ManagerWindow_private_offset + G_STRUCT_OFFSET (ManagerWindowPrivate, path_selection));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "up_button",
                                             FALSE,
                                             ManagerWindow_private_offset + G_STRUCT_OFFSET (ManagerWindowPrivate, up_button));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "down_button",
                                             FALSE,
                                             ManagerWindow_private_offset + G_STRUCT_OFFSET (ManagerWindowPrivate, down_button));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "setup_button",
                                             FALSE,
                                             ManagerWindow_private_offset + G_STRUCT_OFFSET (ManagerWindowPrivate, setup_button));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "delete_button",
                                             FALSE,
                                             ManagerWindow_private_offset + G_STRUCT_OFFSET (ManagerWindowPrivate, delete_button));


  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "up_down_clicked",
                                                G_CALLBACK(manager_window_up_down_clicked));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "delete_row",
                                                G_CALLBACK(manager_window_delete_row));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "show_setup",
                                                G_CALLBACK(manager_window_show_setup));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "on_close_x",
                                                G_CALLBACK(manager_window_on_close_x));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "on_close",
                                                G_CALLBACK(manager_window_on_close));
}
