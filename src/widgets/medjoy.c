/*
 * medjoy.c
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


#include "joystick_linux.h"
#include "marshallers.h"
#include "medjoy.h"

typedef struct _MedJoyClass MedJoyClass;
typedef struct _MedJoyPrivate MedJoyPrivate;


enum  {
  MED_JOY_JOY_EVENT_SIGNAL,
  MED_JOY_NUM_SIGNALS
};
static guint med_joy_signals[MED_JOY_NUM_SIGNALS] = {0};


struct _MedJoyClass {
  GObjectClass parent_class;
};

struct _MedJoyPrivate {
  gint fd;
  GIOChannel* channel;
  guint source_id;
};


G_DEFINE_TYPE_WITH_PRIVATE (MedJoy, med_joy, G_TYPE_OBJECT);


static gboolean
med_joy_watch (GIOChannel* source,
               GIOCondition condition,
               gpointer self)
{
  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (source != NULL, FALSE);

  gint type;
  gint value;
  gint number;
  gssize s;
  MedJoy* joy = self;
  MedJoyPrivate* priv = med_joy_get_instance_private (self);

  if (condition == G_IO_HUP)
    return FALSE;

  s = read_joy (priv->fd, &type, &value, &number);

  if (s > 0)
    g_signal_emit (joy, med_joy_signals[MED_JOY_JOY_EVENT_SIGNAL], 0, joy->name, joy->id, type, value, number);

  return TRUE;
}


void med_joy_enable_joy (MedJoy* self, gboolean enable)
{
  MedJoyPrivate* priv = med_joy_get_instance_private (self);

  if (enable)
  {
    discard_read (priv->fd);
    priv->source_id = g_io_add_watch (priv->channel, G_IO_IN | G_IO_HUP, med_joy_watch, self);
  }
  else
    g_source_remove (priv->source_id);
}


gint
med_joy_init_joy (MedJoy* self,
                  guint js)
{
  g_return_val_if_fail (self != NULL, 0);
  MedJoyPrivate* priv = med_joy_get_instance_private (self);

  guint buttons;
  guint axes;

  priv->fd = get_joy_fd (js);

  if (priv->fd > -1)
  {
    self->name = get_name (priv->fd);
    buttons = get_num_buttons (priv->fd);
    axes = get_num_axes (priv->fd);
    self->id = get_id (js, axes, buttons);

    priv->channel = g_io_channel_unix_new (priv->fd);
    g_io_channel_set_encoding(priv->channel, NULL, NULL);
    g_io_channel_set_close_on_unref (priv->channel, TRUE);
    med_joy_enable_joy (self, TRUE);
  }

  return priv->fd;
}


static void
med_joy_finalize (GObject * obj)
{
  MedJoy * self = G_TYPE_CHECK_INSTANCE_CAST (obj, med_joy_get_type (), MedJoy);
  MedJoyPrivate* priv = med_joy_get_instance_private (self);

  if (priv->channel)
    g_io_channel_unref (priv->channel);

  g_free (self->name);
  g_free (self->id);

  G_OBJECT_CLASS (med_joy_parent_class)->finalize (obj);
}


MedJoy*
med_joy_new (void)
{
  MedJoy * self  = (MedJoy*) g_object_new (med_joy_get_type (), NULL);
  return self;
}


static void
med_joy_init (MedJoy * self)
{
  MedJoyPrivate* priv = med_joy_get_instance_private (self);
  priv->channel = NULL;
}


static void
med_joy_class_init (MedJoyClass * klass)
{
  G_OBJECT_CLASS (klass)->finalize = med_joy_finalize;

  med_joy_signals[MED_JOY_JOY_EVENT_SIGNAL] = g_signal_new ("joy-event",
                                                            med_joy_get_type (),
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
