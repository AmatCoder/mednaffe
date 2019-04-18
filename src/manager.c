/*
 * manager.c
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
	SetupWindow* setup;
};


G_DEFINE_TYPE_WITH_PRIVATE (ManagerWindow, manager_window, GTK_TYPE_WINDOW);


static void
manager_window_show_setup (GtkButton* sender,
                           gpointer self)
{
	GtkTreeIter iter;

  ManagerWindowPrivate* priv = manager_window_get_instance_private (self);

	gboolean valid = gtk_tree_selection_get_selected (priv->path_selection, NULL, &iter);

	if (valid)
		setup_window_setup_show (priv->setup, &iter);
}


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
	GtkTreeIter iter2;

	g_return_if_fail (iter != NULL);

  ManagerWindowPrivate* priv = manager_window_get_instance_private (self);

	GtkTreeModel* model = gtk_combo_box_get_model (priv->combo);

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
	GtkTreeIter iter;
  ManagerWindow* mw = self;
  ManagerWindowPrivate* priv = manager_window_get_instance_private (mw);

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
manager_window_on_show (GtkWidget* sender,
                        gpointer self)
{
	GtkTreeIter iter;
  ManagerWindow* mw = self;
  ManagerWindowPrivate* priv = manager_window_get_instance_private (mw);

	gboolean valid = gtk_combo_box_get_active_iter (priv->combo, &iter);

	if (valid)
  {
		gtk_tree_selection_select_iter (priv->path_selection, &iter);
		manager_window_set_buttons (self, &iter);
	}
}


static void
manager_window_up_down_clicked (GtkButton* sender,
                                 gpointer self)
{
	GtkTreeIter iter, iter2;
  ManagerWindow* mw = self;
  ManagerWindowPrivate* priv = manager_window_get_instance_private(mw);

	gboolean valid = gtk_tree_selection_get_selected (priv->path_selection, NULL, &iter);

	if (valid)
  {
    GtkTreeModel* model = gtk_combo_box_get_model (priv->combo);

		iter2 = iter;

		if (sender == priv->up_button)
			gtk_tree_model_iter_previous (gtk_combo_box_get_model (priv->combo), &iter);
    else if (sender == priv->down_button)
      gtk_tree_model_iter_next (gtk_combo_box_get_model (priv->combo), &iter);

		gtk_list_store_swap ((GtkListStore*) model, &iter, &iter2);

		manager_window_set_buttons (mw, &iter2);
  }
}


static void
manager_window_delete_row (GtkButton* sender,
                           gpointer self)
{
	GtkTreeIter iter;

  ManagerWindowPrivate* priv = manager_window_get_instance_private (self);

  GtkTreeModel* model = gtk_combo_box_get_model (priv->combo);

	gboolean valid = gtk_tree_selection_get_selected (priv->path_selection, NULL, &iter);

	if (valid)
		gtk_list_store_remove ((GtkListStore*) model, &iter);

	valid = gtk_tree_model_get_iter_first (model, &iter);

	gtk_widget_set_sensitive ((GtkWidget*) priv->setup_button, valid);
	gtk_widget_set_sensitive ((GtkWidget*) priv->setup_button, valid);
}


static gboolean
manager_window_on_close_x (GtkWidget* sender,
                           GdkEventAny* event,
                           gpointer self)
{
	g_return_val_if_fail (self != NULL, FALSE);
	gtk_widget_set_visible ((GtkWidget*) self, FALSE);

	return TRUE;
}


static void
manager_window_on_close (GtkButton* sender,
                          gpointer self)
{
	g_return_if_fail (self != NULL);
	gtk_widget_set_visible ((GtkWidget*) self, FALSE);
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

  priv->setup = setup_window_new ((GtkWindow*) self, model);

	g_signal_connect_object (priv->setup, "row-has-changed", (GCallback) manager_window_row_changed, self, 0);

  gtk_widget_hide ((GtkWidget *)self);

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
                                                "on_show",
                                                G_CALLBACK(manager_window_on_show));

	gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "selection_changed",
                                                G_CALLBACK(manager_window_selection_changed));

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
