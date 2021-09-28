/*
 * medlistjoy.c
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


#include "joystick.h"
#include "marshallers.h"
#include "medlistjoy.h"


typedef struct _MedListJoyClass MedListJoyClass;
typedef struct _MedListJoyPrivate MedListJoyPrivate;

struct _MedListJoyClass {
  GObjectClass parent_class;
};

struct _MedListJoyPrivate {
  GSList* list;
};


enum  {
  MED_LIST_JOY_JOY_FOUND_SIGNAL,
  MED_LIST_JOY_JOY_EVENT_SIGNAL,
  MED_LIST_JOY_NUM_SIGNALS
};

static guint med_list_joy_signals[MED_LIST_JOY_NUM_SIGNALS] = {0};


G_DEFINE_TYPE_WITH_PRIVATE (MedListJoy, med_list_joy, G_TYPE_OBJECT);


static gboolean
med_list_joy_watch (gpointer self)
{
  MedListJoyPrivate* priv = med_list_joy_get_instance_private (self);

  gchar* value = read_joys (priv->list);

  if (value)
  {
    gchar *text = value_to_text (priv->list, value);
    g_signal_emit (self, med_list_joy_signals[MED_LIST_JOY_JOY_EVENT_SIGNAL], 0, text, value);
    g_free (text);
    g_free (value);
  }

  return TRUE;
}


void
med_list_joy_enable_all (MedListJoy* self, gboolean enable)
{
  if (enable)
  {
    MedListJoyPrivate* priv = med_list_joy_get_instance_private (self);

    discard_read (priv->list);
    g_timeout_add (100, med_list_joy_watch, self);
  }
  else
    g_source_remove_by_user_data (self);
}


static void
check_duplicates (MedListJoy* self, joy_s *joy)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (joy != NULL);

  MedListJoyPrivate* priv = med_list_joy_get_instance_private (self);

  GSList* it = NULL;
  for (it = priv->list; it != NULL; it = it->next)
  {
    joy_s *joy2 = it->data;

    if (joy->num == joy2->num)
      break;

    if (g_strcmp0 (joy->id, joy2->id) == 0)
    {
      joy->id[33]++; //FIXME: This does not support more than 10 joysticks.
      check_duplicates (self, joy);
      break;
    }
  }
}


void
med_list_joy_init_list_joy (MedListJoy* self)
{
  g_return_if_fail (self != NULL);

  MedListJoyPrivate* priv = med_list_joy_get_instance_private (self);

  priv->list = init_joys ();

  GSList* it = NULL;
  for (it = priv->list; it != NULL; it = it->next)
  {
    joy_s *joy = it->data;
    check_duplicates (self, joy);
    g_signal_emit (self, med_list_joy_signals[MED_LIST_JOY_JOY_FOUND_SIGNAL], 0, joy->name, joy->id);
  }
}


gchar*
med_list_joy_value_to_text (MedListJoy* self,
                            const gchar* value)
{
  g_return_val_if_fail (self != NULL, NULL);
  g_return_val_if_fail (value != NULL, NULL);

  MedListJoyPrivate* priv = med_list_joy_get_instance_private (self);

  return value_to_text (priv->list, value);
}


MedListJoy*
med_list_joy_new (void)
{
  MedListJoy* self = (MedListJoy*) g_object_new (med_list_joy_get_type(), NULL);
  return self;
}


static void
med_list_joy_init (MedListJoy* self)
{
}


static void
med_list_joy_finalize (GObject* obj)
{
  MedListJoy* self = G_TYPE_CHECK_INSTANCE_CAST (obj, med_list_joy_get_type(), MedListJoy);
  MedListJoyPrivate* priv = med_list_joy_get_instance_private (self);

  g_source_remove_by_user_data (self);
  close_joys (priv->list);
  g_slist_free (priv->list);

  G_OBJECT_CLASS (med_list_joy_parent_class)->finalize (obj);
}


static void
med_list_joy_class_init (MedListJoyClass * klass)
{
  G_OBJECT_CLASS (klass)->finalize = med_list_joy_finalize;

  med_list_joy_signals[MED_LIST_JOY_JOY_FOUND_SIGNAL] = g_signal_new ("joy-found",
                                                                      med_list_joy_get_type(),
                                                                      G_SIGNAL_RUN_LAST,
                                                                      0,
                                                                      NULL,
                                                                      NULL,
                                                                      g_cclosure_user_marshal_VOID__STRING_STRING,
                                                                      G_TYPE_NONE,
                                                                      2,
                                                                      G_TYPE_STRING,
                                                                      G_TYPE_STRING);

  med_list_joy_signals[MED_LIST_JOY_JOY_EVENT_SIGNAL] = g_signal_new ("joy-event",
                                                                      med_list_joy_get_type(),
                                                                      G_SIGNAL_RUN_LAST,
                                                                      0,
                                                                      NULL,
                                                                      NULL,
                                                                      g_cclosure_user_marshal_VOID__STRING_STRING,
                                                                      G_TYPE_NONE,
                                                                      2,
                                                                      G_TYPE_STRING,
                                                                      G_TYPE_STRING);
}
