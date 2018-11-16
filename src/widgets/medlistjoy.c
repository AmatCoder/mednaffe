/*
 * medlistjoy.c
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


#include "marshallers.h"
#include "medjoy.h"
#include "medlistjoy.h"


typedef struct _MedListJoyClass MedListJoyClass;
typedef struct _MedListJoyPrivate MedListJoyPrivate;

struct _MedListJoyClass {
  GObjectClass parent_class;
};

struct _MedListJoyPrivate {
  GSList* list;
  gint num_of_items;
};


enum  {
  MED_LIST_JOY_0_PROPERTY,
  MED_LIST_JOY_NUM_OF_JOYS_PROPERTY,
  MED_LIST_JOY_NUM_PROPERTIES
};

static GParamSpec* med_list_joy_properties[MED_LIST_JOY_NUM_PROPERTIES];

enum  {
  MED_LIST_JOY_JOY_FOUND_SIGNAL,
  MED_LIST_JOY_JOY_EVENT_SIGNAL,
  MED_LIST_JOY_NUM_SIGNALS
};

static guint med_list_joy_signals[MED_LIST_JOY_NUM_SIGNALS] = {0};


G_DEFINE_TYPE_WITH_PRIVATE (MedListJoy, med_list_joy, G_TYPE_OBJECT);


static void
med_list_joy_pressed (MedJoy* sender,
                      const gchar* name,
                      const gchar* id,
                      gint type,
                      gint value,
                      gint number,
                      gpointer self)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (name != NULL);
  g_return_if_fail (id != NULL);

  g_signal_emit ((MedListJoy*) self, med_list_joy_signals[MED_LIST_JOY_JOY_EVENT_SIGNAL], 0, name, id, type, value, number);
}


static void
med_list_joy_check_duplicates (MedListJoy* self,
                               MedJoy* joy)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (joy != NULL);

  MedListJoyPrivate* priv = med_list_joy_get_instance_private (self);
  GSList* it = NULL;

  for (it = priv->list; it != NULL; it = it->next)
  {
    MedJoy* mj = it->data;

    if (g_strcmp0 (joy->id, mj->id) == 0)
    {
      joy->id[33]++; //FIXME: This does not support more than 9 joys.
      med_list_joy_check_duplicates (self, joy);
    }
  }
}

void
med_list_joy_enable_all (MedListJoy* self, gboolean enable)
{
  MedListJoyPrivate* priv = med_list_joy_get_instance_private (self);
  GSList* it = NULL;

  for (it = priv->list; it != NULL; it = it->next)
  {
    MedJoy* mj = it->data;
    med_joy_enable_joy(mj, enable);
  }
}


void
med_list_joy_init_list_joy (MedListJoy* self)
{
  gint fd = 0;
  gint i = 0;

  g_return_if_fail (self != NULL);

  MedListJoyPrivate* priv = med_list_joy_get_instance_private (self);

  g_slist_free_full (priv->list, (GDestroyNotify) g_object_unref);
  priv->list = NULL;

  while (fd > -1)
  {
    MedJoy* joy = med_joy_new ();
    fd = med_joy_init_joy (joy, i);

    if (fd > -1)
    {
      med_list_joy_check_duplicates (self, joy);

      g_object_ref (joy);
      priv->list = g_slist_append (priv->list, joy);

      g_signal_emit (self, med_list_joy_signals[MED_LIST_JOY_JOY_FOUND_SIGNAL], 0, joy->name, joy->id);

      g_signal_connect_object (joy, "joy-event", (GCallback) med_list_joy_pressed, self, 0);

      priv->num_of_items++;
    }

    i++;
    g_object_unref (joy);
  }
}


const gchar*
med_list_joy_get_name_from_id (MedListJoy* self,
                               const gchar* id)
{
  g_return_val_if_fail (self != NULL, NULL);
  g_return_val_if_fail (id != NULL, NULL);

  MedListJoyPrivate* priv = med_list_joy_get_instance_private (self);

  gchar* name = "Unknown";
  GSList* it = NULL;

  for (it = priv->list; it != NULL; it = it->next)
  {
    MedJoy* mj = it->data;

    if (g_strcmp0 (id, mj->id) == 0)
    {
      name = mj->name;
      break;
    }
  }

  return name;
}


gint
med_list_joy_get_num_of_joys (MedListJoy* self)
{
  g_return_val_if_fail (self != NULL, 0);

  MedListJoyPrivate* priv = med_list_joy_get_instance_private (self);
  return priv->num_of_items;
}


MedListJoy*
med_list_joy_new (void)
{
  MedListJoy * self = (MedListJoy*) g_object_new (med_list_joy_get_type (), NULL);
  return self;
}


static void
med_list_joy_init (MedListJoy * self)
{
  MedListJoyPrivate* priv = med_list_joy_get_instance_private (self);
  priv->num_of_items = 0;
}


static void
med_list_joy_finalize (GObject * obj)
{
  MedListJoy * self = G_TYPE_CHECK_INSTANCE_CAST (obj, med_list_joy_get_type (), MedListJoy);
  MedListJoyPrivate* priv = med_list_joy_get_instance_private (self);

  g_slist_free_full (priv->list, (GDestroyNotify) g_object_unref);

  G_OBJECT_CLASS (med_list_joy_parent_class)->finalize (obj);
}


static void
med_list_joy_get_property (GObject * object,
                           guint property_id,
                           GValue * value,
                           GParamSpec * pspec)
{
  MedListJoy * self = G_TYPE_CHECK_INSTANCE_CAST (object, med_list_joy_get_type (), MedListJoy);

  switch (property_id)
  {
    case MED_LIST_JOY_NUM_OF_JOYS_PROPERTY:
      g_value_set_int (value, med_list_joy_get_num_of_joys (self));
    break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
med_list_joy_class_init (MedListJoyClass * klass)
{
  G_OBJECT_CLASS (klass)->get_property = med_list_joy_get_property;
  G_OBJECT_CLASS (klass)->finalize = med_list_joy_finalize;

  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_LIST_JOY_NUM_OF_JOYS_PROPERTY,
                                   med_list_joy_properties[MED_LIST_JOY_NUM_OF_JOYS_PROPERTY] = g_param_spec_int
                                   (
                                     "num-of-joys",
                                     "num-of-joys",
                                     "num-of-joys",
                                      G_MININT,
                                      G_MAXINT,
                                      0,
                                      G_PARAM_STATIC_STRINGS | G_PARAM_READABLE
                                   ));

  med_list_joy_signals[MED_LIST_JOY_JOY_FOUND_SIGNAL] = g_signal_new ("joy-found",
                                                                      med_list_joy_get_type (),
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
                                                                      med_list_joy_get_type (),
                                                                      G_SIGNAL_RUN_LAST,
                                                                      0,
                                                                      NULL,
                                                                      NULL,
                                                                      g_cclosure_user_marshal_VOID__STRING_STRING_INT_INT_INT,
                                                                      G_TYPE_NONE,
                                                                      5,
                                                                      G_TYPE_STRING,
                                                                      G_TYPE_STRING,
                                                                      G_TYPE_INT,
                                                                      G_TYPE_INT,
                                                                      G_TYPE_INT);
}
