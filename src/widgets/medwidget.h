/*
 * medwidget.h
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


#ifndef __MEDWIDGET_H__
#define __MEDWIDGET_H__

#include <gtk/gtk.h>

#define IS_MED_WIDGET(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), med_widget_get_type ()))

G_BEGIN_DECLS

typedef struct _MedWidget MedWidget;
typedef struct _MedWidgetInterface MedWidgetInterface;

struct _MedWidgetInterface {
  GTypeInterface parent_iface;
  const gchar* (*get_value) (MedWidget* self);
  void (*set_value) (MedWidget* self, const gchar* v);
  void (*init) (MedWidget* self, GtkWidget* widget);
  const gchar* (*get_command) (MedWidget* self);
  void (*set_command) (MedWidget* self, const gchar* value);
  gboolean (*get_updated) (MedWidget* self);
  void (*set_updated) (MedWidget* self, gboolean value);
  gboolean (*get_modified) (MedWidget* self);
  void (*set_modified) (MedWidget* self, gboolean value);
};

GType med_widget_get_type (void) G_GNUC_CONST;
void med_widget_init (MedWidget* self, GtkWidget* widget);

const gchar* med_widget_get_value (MedWidget* self);
void med_widget_set_value (MedWidget* self, const gchar* v);
const gchar* med_widget_get_command (MedWidget* self);
void med_widget_set_command (MedWidget* self, const gchar* value);
gboolean med_widget_get_updated (MedWidget* self);
void med_widget_set_updated (MedWidget* self, gboolean value);
gboolean med_widget_get_modified (MedWidget* self);
void med_widget_set_modified (MedWidget* self, gboolean value);

G_END_DECLS

#endif
