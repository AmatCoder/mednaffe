/*
 * preferences.c
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


#include <gtk/gtk.h>
#include "widgets/marshallers.h"
#include "preferences.h"


typedef struct _PreferencesWindowClass PreferencesWindowClass;

struct _PreferencesWindowClass {
  GtkDialogClass parent_class;
};


enum  {
  PREFERENCES_WINDOW_ON_SHOW_TIPS_SIGNAL,
  PREFERENCES_WINDOW_ON_SHOW_FILTERS_SIGNAL,
  PREFERENCES_WINDOW_ON_SHOW_SYSTEMS_SIGNAL,
  PREFERENCES_WINDOW_ON_CHANGE_THEME_SIGNAL,
  PREFERENCES_WINDOW_NUM_SIGNALS
};

static guint preferences_window_signals[PREFERENCES_WINDOW_NUM_SIGNALS] = {0};


typedef enum  {
  LYNX,
  GB,
  GBA,
  NGP,
  NES,
  PCE,
  PCFX,
  GG,
  MD,
  SMS,
  SS,
  PSX,
  SNES,
  VB,
  WSWAN,
  SHOWTIPS,
  SHOWFILTERSINHEADER
} PreferencesWindowSystems;


G_DEFINE_TYPE (PreferencesWindow, preferences_window, GTK_TYPE_DIALOG);


static GType
preferences_window_systems_get_type (void)
{
  static volatile gsize preferences_window_systems_type_id__volatile = 0;

  if (g_once_init_enter (&preferences_window_systems_type_id__volatile))
  {
    static const GEnumValue values[] = { {LYNX, "LYNX", "lynx"},
                                         {GB, "GB", "gb"},
                                         {GBA, "GBA", "gba"},
                                         {NGP, "NGP", "ngp"},
                                         {NES, "NES", "nes"},
                                         {PCE, "PCE", "pce"},
                                         {PCFX, "PCFX", "pcfx"},
                                         {GG, "GG", "gg"},
                                         {MD, "MD", "md"},
                                         {SMS, "SMS", "sms"},
                                         {SS, "SS", "ss"},
                                         {PSX, "PSX", "psx"},
                                         {SNES, "SNES", "snes"},
                                         {VB, "VB", "vb"},
                                         {WSWAN, "WSWAN", "wswan"},
                                         {SHOWTIPS, "SHOWTIPS", "showtips"},
                                         {SHOWFILTERSINHEADER, "SHOWFILTERSINHEADER", "showfiltersinheader"},
                                         {0, NULL, NULL} };

    GType preferences_window_systems_type_id = g_enum_register_static (g_intern_static_string ("PreferencesWindowSystems"), values);
    g_once_init_leave (&preferences_window_systems_type_id__volatile, preferences_window_systems_type_id);
  }
  return preferences_window_systems_type_id__volatile;
}


static void
preferences_window_on_system_toggled (GtkToggleButton* sender,
                                      gpointer self)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (sender != NULL);

  PreferencesWindow* pw = self;

  gboolean b = gtk_toggle_button_get_active (sender);

  const gchar *s = med_widget_get_command ((MedWidget*) sender);

  GEnumClass *enum_class = g_type_class_ref (preferences_window_systems_get_type());
  GEnumValue *enum_value = g_enum_get_value_by_nick (enum_class, s);

  if (enum_value)
  {
    switch (enum_value->value)
    {
      case SHOWTIPS:
        g_signal_emit (pw, preferences_window_signals[PREFERENCES_WINDOW_ON_SHOW_TIPS_SIGNAL], 0, b);
      break;
      case SHOWFILTERSINHEADER:
        g_signal_emit (pw, preferences_window_signals[PREFERENCES_WINDOW_ON_SHOW_FILTERS_SIGNAL], 0, b);
      break;
      default:
        g_signal_emit (pw, preferences_window_signals[PREFERENCES_WINDOW_ON_SHOW_SYSTEMS_SIGNAL], 0, enum_value->value, b);
    }
  }

  g_type_class_unref (enum_class);
}


static void
preferences_window_on_change_theme (GtkComboBox* sender,
                                    gpointer self)
{
  PreferencesWindow* pw = self;
  gint t = gtk_combo_box_get_active (sender);
  g_signal_emit (pw, preferences_window_signals[PREFERENCES_WINDOW_ON_CHANGE_THEME_SIGNAL], 0, t);
}


static void
preferences_window_set_list (PreferencesWindow* self,
                             GtkWidget* wid)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (wid != NULL);

  GList* element_collection = gtk_container_get_children ((GtkContainer*) wid);
  GList* element_it = NULL;

  for (element_it = element_collection; element_it != NULL; element_it = element_it->next)
  {
    if (IS_MED_WIDGET(element_it->data))
    {
      self->list = g_slist_append (self->list, element_it->data);
      gtk_widget_set_sensitive (element_it->data, TRUE);
    }

    if (G_TYPE_CHECK_INSTANCE_TYPE (element_it->data, gtk_container_get_type ()))
      preferences_window_set_list (self, element_it->data);
  }

  g_list_free (element_collection);
}


static gboolean
preferences_window_on_close_x (GtkWidget* _sender,
                               GdkEventAny* event,
                               gpointer self)
{
  g_return_val_if_fail (self != NULL, FALSE);

  gtk_widget_set_visible ((GtkWidget*) self, FALSE);
  return TRUE;
}


static void
preferences_window_on_close (GtkButton* sender,
                             gpointer self)
{
  g_return_if_fail (self != NULL);

  gtk_widget_set_visible ((GtkWidget*) self, FALSE);
}


static void
preferences_window_finalize (GObject * obj)
{
  PreferencesWindow *self = G_TYPE_CHECK_INSTANCE_CAST (obj, preferences_window_get_type (), PreferencesWindow);

  g_slist_free(self->list);

  G_OBJECT_CLASS (preferences_window_parent_class)->finalize (obj);
}


PreferencesWindow*
preferences_window_new (GtkWindow* parent)
{
  g_return_val_if_fail (parent != NULL, NULL);

  PreferencesWindow *self = (PreferencesWindow*) g_object_new (preferences_window_get_type (), NULL);

  gtk_window_set_destroy_with_parent ((GtkWindow*) self, TRUE);
  gtk_window_set_transient_for ((GtkWindow*) self, parent);
  gtk_window_set_modal ((GtkWindow*) self, TRUE);
  gtk_window_set_title ((GtkWindow*) self, "Preferences");

  med_widget_set_value ((MedWidget*) self->action_launch, "do nothing");

  preferences_window_set_list (self, (GtkWidget*) self);

#ifndef G_OS_WIN32
  gtk_combo_box_text_remove_all (self->change_theme);
#endif

  return self;
}


static void
preferences_window_init (PreferencesWindow * self)
{
  g_type_ensure (med_check_button_get_type ());
  g_type_ensure (med_combo_box_get_type ());

  gtk_widget_init_template (GTK_WIDGET (self));
}


static void
preferences_window_class_init (PreferencesWindowClass * klass)
{
  G_OBJECT_CLASS (klass)->finalize = preferences_window_finalize;

  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (klass), "/com/github/mednaffe/Preferences.ui");

  preferences_window_signals[PREFERENCES_WINDOW_ON_SHOW_TIPS_SIGNAL] = g_signal_new ("on-show-tips",
                                                                                     preferences_window_get_type (),
                                                                                     G_SIGNAL_RUN_LAST,
                                                                                     0,
                                                                                     NULL,
                                                                                     NULL,
                                                                                     g_cclosure_marshal_VOID__BOOLEAN,
                                                                                     G_TYPE_NONE,
                                                                                     1,
                                                                                     G_TYPE_BOOLEAN);

  preferences_window_signals[PREFERENCES_WINDOW_ON_SHOW_FILTERS_SIGNAL] = g_signal_new ("on-show-filters",
                                                                                        preferences_window_get_type (),
                                                                                        G_SIGNAL_RUN_LAST,
                                                                                        0,
                                                                                        NULL,
                                                                                        NULL,
                                                                                        g_cclosure_marshal_VOID__BOOLEAN,
                                                                                        G_TYPE_NONE,
                                                                                        1,
                                                                                        G_TYPE_BOOLEAN);

  preferences_window_signals[PREFERENCES_WINDOW_ON_SHOW_SYSTEMS_SIGNAL] = g_signal_new ("on-show-systems",
                                                                                        preferences_window_get_type (),
                                                                                        G_SIGNAL_RUN_LAST,
                                                                                        0,
                                                                                        NULL,
                                                                                        NULL,
                                                                                        g_cclosure_user_marshal_VOID__INT_BOOLEAN,
                                                                                        G_TYPE_NONE,
                                                                                        2,
                                                                                        G_TYPE_INT,
                                                                                        G_TYPE_BOOLEAN);

  preferences_window_signals[PREFERENCES_WINDOW_ON_CHANGE_THEME_SIGNAL] = g_signal_new ("on-change-theme",
                                                                                     preferences_window_get_type (),
                                                                                     G_SIGNAL_RUN_LAST,
                                                                                     0,
                                                                                     NULL,
                                                                                     NULL,
                                                                                     g_cclosure_marshal_VOID__INT,
                                                                                     G_TYPE_NONE,
                                                                                     1,
                                                                                     G_TYPE_INT);

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "win_size",
                                              FALSE,
                                              G_STRUCT_OFFSET (PreferencesWindow, win_size));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "win_pos",
                                             FALSE,
                                             G_STRUCT_OFFSET (PreferencesWindow, win_pos));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "action_launch",
                                             FALSE,
                                             G_STRUCT_OFFSET (PreferencesWindow, action_launch));

  gtk_widget_class_bind_template_child_full (GTK_WIDGET_CLASS (klass),
                                             "change_theme",
                                             FALSE,
                                             G_STRUCT_OFFSET (PreferencesWindow, change_theme));


  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "on_system_toggled",
                                                G_CALLBACK(preferences_window_on_system_toggled));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "on_close_x",
                                                G_CALLBACK(preferences_window_on_close_x));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "on_close",
                                                G_CALLBACK(preferences_window_on_close));

  gtk_widget_class_bind_template_callback_full (GTK_WIDGET_CLASS (klass),
                                                "on_change_theme",
                                                G_CALLBACK(preferences_window_on_change_theme));
}
