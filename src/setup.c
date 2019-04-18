/*
 * setup.c
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


#include "setup.h"
#include "widgets/dialogs.h"


typedef struct _SetupWindowClass SetupWindowClass;
typedef struct _SetupWindowPrivate SetupWindowPrivate;

struct _SetupWindowClass {
  GtkDialogClass parent_class;
};

struct _SetupWindowPrivate {
  GtkLabel* dir;
  GtkCheckButton* scan;
  GtkCheckButton* hide_ext;
  GtkEntry* filters;
  GtkEntry* screen_a;
  GtkEntry* screen_b;
  GtkTreeModel* model;
  GtkTreeIter iter;
};


G_DEFINE_TYPE_WITH_PRIVATE (SetupWindow, setup_window, GTK_TYPE_DIALOG);

enum  {
  SETUP_WINDOW_ROW_HAS_CHANGED_SIGNAL,
  SETUP_WINDOW_NUM_SIGNALS
};

static guint setup_window_signals[SETUP_WINDOW_NUM_SIGNALS] = {0};


void
setup_window_setup_show (SetupWindow* self,
                         GtkTreeIter* iter)
{
  gchar* dir;

  gboolean scan;
  gboolean hide;
  gchar* filters;
  gchar* sa;
  gchar* sb;

  g_return_if_fail (self != NULL);
  g_return_if_fail (iter != NULL);

  SetupWindowPrivate* priv = setup_window_get_instance_private (self);

  priv->iter = *iter;
  gtk_tree_model_get (priv->model, &priv->iter, 0, &dir, 1, &scan, 2, &hide, 3, &filters, 4, &sa, 5, &sb, -1);

  gtk_label_set_text (priv->dir, dir);

  gtk_toggle_button_set_active ((GtkToggleButton*) priv->scan, scan);
  gtk_toggle_button_set_active ((GtkToggleButton*) priv->hide_ext, hide);

  gtk_entry_set_text (priv->filters, filters);
  gtk_entry_set_text (priv->screen_a, sa);
  gtk_entry_set_text (priv->screen_b, sb);

  gtk_window_present ((GtkWindow*) self);

  g_free (sb);
  g_free (sa);
  g_free (filters);
  g_free (dir);
}


static void
setup_window_apply_clicked (GtkButton* sender,
                            gpointer self)
{
  gboolean scan;
  gboolean hide;
  const gchar* filters;
  const gchar* sa;
  const gchar* sb;

  g_return_if_fail (self != NULL);

  SetupWindow* sw = self;
  SetupWindowPrivate* priv = setup_window_get_instance_private (sw);

  scan = gtk_toggle_button_get_active ((GtkToggleButton*) priv->scan );
  hide = gtk_toggle_button_get_active ((GtkToggleButton*) priv->hide_ext);
  filters = gtk_entry_get_text (priv->filters);
  sa = gtk_entry_get_text (priv->screen_a);
  sb = gtk_entry_get_text (priv->screen_b);

  gtk_list_store_set ((GtkListStore*) priv->model, &priv->iter, 1, scan, 2, hide, 3, filters, 4, sa, 5, sb, -1);

  gtk_widget_set_visible ((GtkWidget*) self, FALSE);

  g_signal_emit (sw, setup_window_signals[SETUP_WINDOW_ROW_HAS_CHANGED_SIGNAL], 0);
}


static void
setup_window_entry_button_clicked (GtkButton* sender,
                                   gpointer self)
{
  GtkWidget* parent = gtk_widget_get_toplevel ((GtkWidget*) sender);
  gchar* filename = select_path (parent, TRUE);

  if (filename != NULL)
  {
    gtk_entry_set_text ((GtkEntry*) self, filename);
    g_free (filename);
  }
}


static gboolean
setup_window_on_close_x (GtkWidget* _sender,
                         GdkEventAny* event,
                         gpointer self)
{
  g_return_val_if_fail (self != NULL, FALSE);

  gtk_widget_set_visible ((GtkWidget*) self, FALSE);

  return TRUE;
}


static void
setup_window_on_close (GtkButton* sender,
                       gpointer self)
{
  g_return_if_fail (self != NULL);

  gtk_widget_set_visible ((GtkWidget*) self, FALSE);
}


SetupWindow*
setup_window_new (GtkWindow* parent,
                  GtkTreeModel* model)
{
  g_return_val_if_fail (parent != NULL, NULL);
  g_return_val_if_fail (model != NULL, NULL);

  SetupWindow * self = (SetupWindow*) g_object_new (setup_window_get_type (), NULL);
  SetupWindowPrivate* priv = setup_window_get_instance_private (self);

  gtk_window_set_transient_for ((GtkWindow*) self, parent);

  priv->model = model;

  return self;
}


static void
setup_window_init (SetupWindow * self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}


static void
setup_window_class_init (SetupWindowClass * klass)
{
  gint SetupWindow_private_offset = g_type_class_get_instance_private_offset (klass);
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (klass), "/com/github/mednaffe/Setup.ui");

  setup_window_signals[SETUP_WINDOW_ROW_HAS_CHANGED_SIGNAL] = g_signal_new ("row-has-changed",
                                                                            setup_window_get_type (),
                                                                            G_SIGNAL_RUN_LAST,
                                                                            0,
                                                                            NULL,
                                                                            NULL,
                                                                            g_cclosure_marshal_VOID__VOID,
                                                                            G_TYPE_NONE,
                                                                            0);


  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "dir",
                                             FALSE,
                                             SetupWindow_private_offset + G_STRUCT_OFFSET (SetupWindowPrivate, dir));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "scan",
                                             FALSE,
                                             SetupWindow_private_offset + G_STRUCT_OFFSET (SetupWindowPrivate, scan));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "hide_ext",
                                             FALSE,
                                             SetupWindow_private_offset + G_STRUCT_OFFSET (SetupWindowPrivate, hide_ext));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "filters",
                                             FALSE,
                                             SetupWindow_private_offset + G_STRUCT_OFFSET (SetupWindowPrivate, filters));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "screen_a",
                                             FALSE,
                                             SetupWindow_private_offset + G_STRUCT_OFFSET (SetupWindowPrivate, screen_a));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "screen_b",
                                             FALSE,
                                             SetupWindow_private_offset + G_STRUCT_OFFSET (SetupWindowPrivate, screen_b));


  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "apply_clicked",
                                                G_CALLBACK(setup_window_apply_clicked));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "on_close_x",
                                                G_CALLBACK(setup_window_on_close_x));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "on_close",
                                                G_CALLBACK(setup_window_on_close));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "on_entry_button_clicked",
                                                G_CALLBACK(setup_window_entry_button_clicked));
}
