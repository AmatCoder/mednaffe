/*
 * medinput.h
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


#include "medlistjoy.h"
#include "menuinput.h"
#include "medinput.h"


typedef struct _MedInputClass MedInputClass;
typedef struct _MedInputPrivate MedInputPrivate;

struct _MedInputClass {
  GtkBoxClass parent_class;
};

struct _MedInputPrivate {
  GtkButton* entry;
  GtkLabel* entry_label;
  //GtkWidget* eventbox;
  MenuInput* menu;
  gchar* _command;
  gboolean _updated;
  gchar* _label;
  gboolean _modifier_keys;
  //GtkCssProvider* css_normal;
  //GtkCssProvider* css_active;
  //GtkCssProvider* css_disabled;
  //GtkStyleContext* stylecontext;
  gboolean is_active;
  gchar* old_text;
  gchar* internal_value;
  gchar* value;
  MedListJoy* listjoy;
};


enum  {
  MED_INPUT_0_PROPERTY,
  MED_INPUT_COMMAND_PROPERTY,
  MED_INPUT_UPDATED_PROPERTY,
  MED_INPUT_LABEL_PROPERTY,
  MED_INPUT_LABELWIDTH_PROPERTY,
  MED_INPUT_MODIFIER_KEYS_PROPERTY,
  MED_INPUT_NUM_PROPERTIES
};

static GParamSpec* med_input_properties[MED_INPUT_NUM_PROPERTIES];


static void med_input_med_widget_interface_init (MedWidgetInterface* iface);

static gboolean med_input_entry_key_press (GtkWidget* sender,
                                           GdkEventKey* event,
                                           gpointer self);

static void med_input_joy_event (MedListJoy* sender,
                                 const gchar* name,
                                 const gchar* id,
                                 gint type,
                                 gint value,
                                 gint number,
                                 gpointer self);


G_DEFINE_TYPE_WITH_CODE (MedInput, med_input, GTK_TYPE_BOX,
                         G_ADD_PRIVATE (MedInput)
                         G_IMPLEMENT_INTERFACE (med_widget_get_type(), med_input_med_widget_interface_init));


const guint sdl_to_gdk[245] = {
0,  0,  0,  0,  38,  56,  54,  40,  26,
41,  42,  43,  31,  44,  45,  46,  58,  57,  32,  33,  24,  27,  39,
28,  30,  55,  25,  53,  29,  53,  10,  11,  12,  13,  14,  15,  16,
17,  18,  19,  36,  9,  22,  23,  65,  20,  21,  34,  35,  51,  0,  47,
48,  49,  59,  60,  61,  66,  67,  68,  69,  70,  71,  72,  73,  74,
75,  76,  95,  96,  107,  78,  127,  118,  110,  112,  119,  115,  117,
114,  113,  116,  111,  77,  106,  63,  82,  86,  104,  87,  88,  89,
83,  84,  85,  79,  80,  81,  90,  91,  94,  135,  124,  125,  191,
192,  193,  194,  195,  196,  197,  198,  199,  200,  201,  202,  0,
146,  147,  0,  136,  137,  139,  145,  141,  143,  144,  121,  122,
123,  0,  0,  0,  129,  0,  97,  101,  132,  100,  102,  103,  0,  0,
0,  130,  131,  98,  99,  93,  0,  0,  0,  0,  230,  107,  231,  0,  0,
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  37,  50,  64,  133,
105,  62,  108,  134,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0
};

const guint gdk_to_sdl[245] = {
0,  0,  0,  0,  0,  0,  0,  0,  0,
41,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  45,  46,  42,
43,  20,  26,  8,  21,  23,  28,  24,  12,  18,  19,  47,  48,  40,
224,  4,  22,  7,  9,  10,  11,  13,  14,  15,  51,  52,  53,  225,
49,  29,  27,  6,  25,  5,  17,  16,  54,  55,  56,  229,  85,  226,
44,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  83,  71,
95,  96,  97,  86,  92,  93,  94,  87,  89,  90,  91,  98,  99,  0,
148,  100,  68,  69,  135,  146,  147,  138,  136,  139,  140,  88,
228,  84,  70,  230,  0,  74,  82,  75,  80,  79,  77,  81,  78,  73,
76,  0,  127,  129,  128,  102,  103,  215,  72,  0,  133,  144,  145,
137,  227,  231,  101,  120,  121,  0,  122,  0,  124,  0,  125,  126,
123,  117,  118,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  104,  105,  106,  107,
108,  109,  110,  111,  112,  113,  114,  115,  0,  0,  0,  0,  0,  0,
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
0,  0,  0,  153,  155,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0
};



static gchar*
med_input_get_sdl_key (MedInput* self,
                       const gchar* str)
{
  gchar* result;
  gchar** items;

  g_return_val_if_fail (self != NULL, NULL);
  g_return_val_if_fail (str != NULL, NULL);

  items = g_strsplit (str, "+", 0);
  result =g_strdup (items[0]);

  g_strfreev(items);

  return result;
}


static gchar*
med_input_check_mods (MedInput* self,
                      const gchar* str)
{
  gchar* mods;
  gchar** items;
  guint len;

  g_return_val_if_fail (self != NULL, NULL);
  g_return_val_if_fail (str != NULL, NULL);

  mods = g_strdup ("");
  items = g_strsplit (str, "+", 0);
  len = g_strv_length(items);

  if (len > 1)
  {
    guint i = 0;
    while (i < len)
    {
      switch (items[i][0])
      {
        case 'a':
        {
          gchar* tmp = g_strconcat (mods, "Alt+", NULL);
          g_free (mods);
          mods = tmp;
          break;
        }
        case 'c':
        {
          gchar* tmp = g_strconcat (mods, "Ctrl+", NULL);
          g_free (mods);
          mods = tmp;
          break;
        }
        case 's':
        {
          gchar* tmp = g_strconcat (mods, "Shift+", NULL);
          g_free (mods);
          mods = tmp;
          break;
        }
        default:
          break;
      }
      i++;
    }
  }

  g_strfreev(items);
  return mods;
}


static const gchar*
string_offset (const gchar* self,
               glong offset)
{
  g_return_val_if_fail (self != NULL, NULL);
  return (const gchar*) (((gchar*) self) + offset);
}


static gchar*
med_input_convert_to_text (MedInput* self,
                           const gchar* v,
                           gchar** p)
{
  g_return_val_if_fail (self != NULL, NULL);
  g_return_val_if_fail (v != NULL, NULL);

  MedInputPrivate* priv = med_input_get_instance_private (self);

  gchar* text = NULL;
  gchar** items = g_strsplit (v, " ", 4);

  if (v[0] ==  'k')
  {
    if (g_strcmp0 (items[1], "0x0") == 0)
    {
      guint keyval;
      gchar* key;
      gchar* mods;

      key = med_input_get_sdl_key (self, items[2]);
      mods = med_input_check_mods (self, items[2]);

      GdkKeymap* keymap = gdk_keymap_get_for_display (gdk_display_get_default ());
      gdk_keymap_translate_keyboard_state (keymap, sdl_to_gdk[atoi (key)], 0, 0, &keyval, NULL, NULL, NULL);

      text = g_strconcat (mods, gdk_keyval_name (gdk_keyval_to_upper (keyval)), " (Keyboard)", NULL);

      *p = g_strdup (items[3]);

      g_free (mods);
      g_free (key);
    }
    else
      text = g_strdup (v);

  }
  else if (v[0] == 'j')
  {
    GtkWidget* parent = gtk_widget_get_toplevel ((GtkWidget*) self);
    priv->listjoy = g_object_get_data ((GObject*) parent, "listjoy");

    if (items[2][0] == 'b')
    {
      text = g_strconcat ("Button ", string_offset (items[2], 7),
                          " (", med_list_joy_get_name_from_id (priv->listjoy, items[1]),
                          ")",  NULL);

    }
    else if (items[2][0] == 'a')
    {
      gchar* t = NULL;
      gint last = strlen (items[2]) - 1;
      gint number = items[2][last - 1];

      if (items[2][last] ==  '+')
      {
        if ((number % 2) != 0)
          t = g_strdup ("Down");
        else
          t = g_strdup ("Right");
      }
      else //if(items[2][last] ==  '-')
      {
        if ((number % 2) != 0)
          t = g_strdup ("Up");
        else
          t = g_strdup ("Left");
      }

      gchar* tmp = g_strdup(items[2]);

      tmp[last] = ' ';
      if ((tmp[last-1] == '+') || (tmp[last-1] == '-'))
        tmp[last-1] = ' ';

      const gchar* name = med_list_joy_get_name_from_id (priv->listjoy, items[1]);
      text = g_strconcat ("Axis ", string_offset (tmp, 4), t, " (", name, ")", NULL);

      g_free (tmp);
      g_free (t);
    }
    *p = g_strdup (items[3]);
  }
  else if (v[0] == 'm')
  {

    if (g_strcmp0 (items[1], "0x0") == 0)
    {
      gchar* tmp = g_strdup(items[2]);

      if (tmp[0] == 'b')
        text = g_strconcat ("Button ", string_offset (tmp, 7), " (Mouse)", NULL);
      else if (tmp[0] == 'r')
        text = g_strconcat ("Relative ", string_offset (tmp, 4), " (Mouse)", NULL);
      else if (tmp[0] == 'c')
        text = g_strconcat ("Cursor ", string_offset (tmp, 7), " (Mouse)", NULL);

      g_free (tmp);

      *p = g_strdup (items[3]);
    }
    else
      text = g_strdup (v);

  }
  else
    text = g_strdup (v);

  g_strfreev(items);

  return text;
}


static void
med_input_real_set_value (MedWidget* base,
                          const gchar* v)
{
  gchar* p = NULL;
  gchar* text;

  MedInput *self = (MedInput*) base;
  MedInputPrivate* priv = med_input_get_instance_private (self);

  g_return_if_fail (v != NULL);

  g_free (priv->internal_value);
  priv->internal_value = g_strdup (v);

  text = med_input_convert_to_text (self, v, &p);

  while (p != NULL)
  {
    if (g_str_has_prefix (p, "||"))
    {
      gchar* tmp = g_strconcat (text, " or ", NULL);
      g_free (text);
      text = tmp;
    }
    else if (g_str_has_prefix (p, "&&"))
    {
      gchar* tmp = g_strconcat (text, " and ", NULL);
      g_free (text);
      text = tmp;
    }
    else if (g_str_has_prefix (p, "&!"))
    {
      gchar* tmp = g_strconcat (text, " and not ", NULL);
      g_free (text);
      text = tmp;
    }
    else
      break;

    gchar* o = NULL;
    gchar* tmp2 = med_input_convert_to_text (self, string_offset (p, 3), &o);
    gchar* tmp3 = g_strconcat (text, tmp2, NULL);
    g_free (tmp2);

    g_free (text);
    text = tmp3;

    g_free (p);
    p = o;
  }

  gtk_button_set_label (priv->entry, text);

  g_free (text);
  g_free (p);

  med_widget_set_updated (base, TRUE);
}


static const gchar*
med_input_real_get_value (MedWidget* base)
{
  MedInput* self = (MedInput*) base;
  MedInputPrivate* priv = med_input_get_instance_private (self);

  g_free(priv->value);
  priv->value = g_strdup (priv->internal_value);
  return priv->value;
}


static void
med_input_close_input (MedInput* self)
{
  guint id1;
  guint id2;

  g_return_if_fail (self != NULL);
  MedInputPrivate* priv = med_input_get_instance_private (self);

  g_signal_parse_name ("joy-event", med_list_joy_get_type (), &id1, NULL, FALSE);

  g_signal_handlers_disconnect_matched (priv->listjoy,
                                        G_SIGNAL_MATCH_ID | G_SIGNAL_MATCH_FUNC | G_SIGNAL_MATCH_DATA,
                                        id1,
                                        0,
                                        NULL,
                                        (GCallback) med_input_joy_event,
                                        self);

  g_signal_parse_name ("key-press-event", gtk_widget_get_type (), &id2, NULL, FALSE);

  g_signal_handlers_disconnect_matched ((GtkWidget*) priv->entry,
                                        G_SIGNAL_MATCH_ID | G_SIGNAL_MATCH_FUNC | G_SIGNAL_MATCH_DATA,
                                        id2,
                                        0,
                                        NULL,
                                        (GCallback) med_input_entry_key_press,
                                        self);

  //gtk_style_context_add_provider (priv->stylecontext, (GtkStyleProvider*) priv->css_normal, 800);

  priv->is_active = FALSE;
}


static void
input_set_text (MedInput* mi, const gchar* text, const gchar* value)
{
  MedInputPrivate* priv = med_input_get_instance_private (mi);

  if (priv->is_active)
  {
    gchar* tmp;

    tmp = g_strconcat (gtk_button_get_label (priv->entry), text, NULL);
    gtk_button_set_label (priv->entry, tmp);
    g_free (tmp);

    tmp = g_strconcat (priv->internal_value, value, NULL);
    g_free (priv->internal_value);
    priv->internal_value = tmp;

    gtk_toggle_button_set_active ((GtkToggleButton *) priv->entry, FALSE);
    med_input_close_input (mi);
    med_widget_set_updated ((MedWidget*) mi, TRUE);
  }
}


static void
med_input_joy_event (MedListJoy* sender,
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

  MedInput* mi = self;
  gchar* n = g_strdup_printf ("%i", number);

  if (type == 1)
  {
    gchar* text = g_strconcat ("Button ", n, " (", name, ")", NULL);
    gchar* value = g_strconcat ("joystick ", id, " button_", n, NULL);

    input_set_text(mi, text, value);

    g_free(text);
    g_free(value);
  }
  else if (type == 2)
  {
    gchar* t;
    gchar* s;

    if (value > 0)
    {
      s = g_strdup ("+");

      if ((number % 2) != 0)
        t = g_strdup (" Down");
      else
        t = g_strdup (" Right");
    }
    else
    {
      s = g_strdup ("-");

      if ((number % 2) != 0)
        t = g_strdup (" Up");
      else
        t = g_strdup (" Left");
    }

    gchar* text = g_strconcat ("Axis ", n, t, " (", name, ")", NULL);
    gchar* value = g_strconcat ("joystick ", id, " abs_", n, s, NULL);

    g_free (s);
    g_free (t);

    input_set_text(mi, text, value);

    g_free(text);
    g_free(value);
  }
  g_free(n);
}


static gboolean
med_input_entry_key_press (GtkWidget* sender,
                           GdkEventKey* event,
                           gpointer self)
{
  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  MedInput* mi = self;
  MedInputPrivate* priv = med_input_get_instance_private (mi);

  if (priv->_modifier_keys)
  {
    if ((event->keyval == 65505) ||
        (event->keyval == 65506) ||
        (event->keyval == 65507) ||
        (event->keyval == 65508) ||
        (event->keyval == 65511) ||
        (event->keyval == 65512) ||
        (event->keyval == 65513) ||
        (event->keyval == 65514)
       ) return FALSE;
  }

  if (event->hardware_keycode > 244) return TRUE;
  if (gdk_to_sdl[event->hardware_keycode] ==  0) return TRUE;

  gchar* tmp = g_strdup_printf ("%u", gdk_to_sdl[event->hardware_keycode]);
  gchar* value = g_strconcat ("keyboard 0x0 ", tmp, NULL);
  g_free (tmp);

  gchar* key = g_strdup (gdk_keyval_name (gdk_keyval_to_upper (event->keyval)));

  if (key == NULL)
  {
    g_free (key);
    return TRUE;
  }

  gchar* mods = g_strdup ("");

  if (priv->_modifier_keys)
  {
    if ((event->state & GDK_CONTROL_MASK) == GDK_CONTROL_MASK)
    {
      gchar* tmp = g_strconcat (mods, "Ctrl+", NULL);
      g_free (mods);
      mods = tmp;

      gchar* tmp2 = g_strconcat (value, "+ctrl", NULL);
      g_free (value);
      value = tmp2;
    }

    if ((event->state & GDK_MOD1_MASK) == GDK_MOD1_MASK)
    {
      gchar* tmp = g_strconcat (mods, "Alt+", NULL);
      g_free (mods);
      mods = tmp;

      gchar* tmp2 = g_strconcat (value, "+alt", NULL);
      g_free (value);
      value = tmp2;
    }

    if ((event->state & GDK_SHIFT_MASK) == GDK_SHIFT_MASK)
    {
      gchar* tmp = g_strconcat (mods, "Shift+", NULL);
      g_free (mods);
      mods = tmp;

      gchar* tmp2 = g_strconcat (value, "+shift", NULL);
      g_free (value);
      value = tmp2;
    }
  }

  gchar* text = g_strconcat (mods, key, " (Keyboard)", NULL);

  input_set_text(mi, text, value);

  g_free(value);
  g_free(text);
  g_free (mods);
  g_free (key);

  return TRUE;
}


static void
med_input_open_input (MedInput* self)
{
  GtkWidget* parent;

  g_return_if_fail (self != NULL);
  MedInputPrivate* priv = med_input_get_instance_private (self);

  priv->is_active = TRUE;

  g_signal_connect_object ((GtkWidget*) priv->entry, "key-press-event", (GCallback) med_input_entry_key_press, self, 0);

  parent = gtk_widget_get_toplevel ((GtkWidget*) self);
  priv->listjoy = g_object_get_data ((GObject*) parent, "listjoy");

  g_signal_connect_object (priv->listjoy, "joy-event", (GCallback) med_input_joy_event, self, 0);
}


static const gchar*
med_input_real_get_command (MedWidget* base)
{
  MedInput* self = (MedInput*) base;
  MedInputPrivate* priv = med_input_get_instance_private (self);

  return priv->_command;
}


static void
med_input_real_set_command (MedWidget* base,
                            const gchar* value)
{
  MedInput* self = (MedInput*) base;
  MedInputPrivate* priv = med_input_get_instance_private (self);

  if (g_strcmp0 (value, med_input_real_get_command (base)) != 0)
  {
    g_free (priv->_command);
    priv->_command = g_strdup (value);
    g_object_notify_by_pspec ((GObject *) self, med_input_properties[MED_INPUT_COMMAND_PROPERTY]);
  }
}


static gboolean
med_input_real_get_updated (MedWidget* base)
{
  MedInput* self = (MedInput*) base;
  MedInputPrivate* priv = med_input_get_instance_private (self);

  return priv->_updated;
}


static void
med_input_real_set_updated (MedWidget* base,
                            gboolean value)
{
  MedInput* self = (MedInput*) base;
  MedInputPrivate* priv = med_input_get_instance_private (self);

  if (med_input_real_get_updated (base) != value)
  {
    priv->_updated = value;
    g_object_notify_by_pspec ((GObject *) self, med_input_properties[MED_INPUT_UPDATED_PROPERTY]);
  }
}


const gchar*
med_input_get_label (MedInput* self)
{
  g_return_val_if_fail (self != NULL, NULL);
  MedInputPrivate* priv = med_input_get_instance_private (self);

  return priv->_label;
}


static void
med_input_set_label (MedInput* self,
                     const gchar* value)
{
  g_return_if_fail (self != NULL);
  MedInputPrivate* priv = med_input_get_instance_private (self);

  if (g_strcmp0 (value, med_input_get_label (self)) != 0)
  {
    g_free (priv->_label);
    priv->_label = g_strdup (value);
    g_object_notify_by_pspec ((GObject *) self, med_input_properties[MED_INPUT_LABEL_PROPERTY]);
  }
}


static void
med_input_set_label_width (MedInput* self,
                           gint value)
{
  g_return_if_fail (self != NULL);
  MedInputPrivate* priv = med_input_get_instance_private (self);
  g_object_set ((GtkWidget*) priv->entry_label, "width-request", value, NULL);
}


gboolean
med_input_get_modifier_keys (MedInput* self)
{
  g_return_val_if_fail (self != NULL, FALSE);
  MedInputPrivate* priv = med_input_get_instance_private (self);

  return priv->_modifier_keys;
}


void
med_input_set_modifier_keys (MedInput* self,
                             gboolean value)
{
  g_return_if_fail (self != NULL);
  MedInputPrivate* priv = med_input_get_instance_private (self);

  if (med_input_get_modifier_keys (self) != value)
  {
    priv->_modifier_keys = value;
    g_object_notify_by_pspec ((GObject *) self, med_input_properties[MED_INPUT_MODIFIER_KEYS_PROPERTY]);
  }
}


static gboolean
med_input_entry_focus_out (GtkWidget* sender,
                           GdkEventFocus* event,
                           gpointer self)
{
  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  MedInput* mi = self;
  MedInputPrivate* priv = med_input_get_instance_private (mi);

  if (priv->is_active)
  {
    gtk_button_set_label (priv->entry, priv->old_text);
    gtk_toggle_button_set_active ((GtkToggleButton *) priv->entry, FALSE);
    med_input_close_input (mi);
  }

  return FALSE;
}


static void
med_input_menu_event (GtkMenuItem* sender,
                      gpointer self)
{
  g_return_if_fail (self != NULL);

  gchar* text;
  gchar* op;

  MedInput* mi = self;
  MedInputPrivate* priv = med_input_get_instance_private (mi);

  g_free (priv->old_text);
  priv->old_text  = g_strdup (gtk_button_get_label (priv->entry));

  text = g_object_get_data((GObject*) sender, "menu_text");
  op = g_object_get_data((GObject*) sender, "menu_op");

  if (text == NULL)
  {
    gtk_button_set_label (priv->entry, "");

    g_free (priv->internal_value);
    priv->internal_value = g_strdup ("");

    med_widget_set_updated ((MedWidget*) mi, TRUE);

    return;
  }

  gchar* tmp = g_strconcat (gtk_button_get_label (priv->entry), text, NULL);

  gtk_button_set_label (priv->entry, tmp);
  g_free (tmp);

  tmp = g_strconcat (priv->internal_value, op, NULL);
  g_free (priv->internal_value);
  priv->internal_value = tmp;

  med_input_open_input (mi);
}


static gboolean
med_input_entry_mouse_clicked (GtkWidget* sender,
                               GdkEventButton* event,
                               gpointer self)
{
  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  MedInput* mi = self;
  MedInputPrivate* priv = med_input_get_instance_private (mi);

  if (event->type == GDK_BUTTON_PRESS)
  {
    if (event->button == 1)
    {
      if (priv->is_active)
      {
        input_set_text(mi, "Button left (Mouse)", "mouse 0x0 button_left");
      }
      else
      {
        g_free (priv->old_text);
        priv->old_text = g_strdup (gtk_button_get_label (priv->entry));

        gtk_button_set_label (priv->entry, "");

        g_free (priv->internal_value);
        priv->internal_value = g_strdup ("");

        med_input_open_input (mi);
      }
    }
    else if (event->button == 2)
    {
      input_set_text(mi, "Button middle (Mouse)", "mouse 0x0 button_middle");
    }
    else if (event->button == 3)
    {
      if (priv->is_active)
      {
        input_set_text(mi, "Button right (Mouse)", "mouse 0x0 button_right");
      }
      else
      {
        priv->menu = menu_input_new ();

        g_signal_connect_object (priv->menu->clear, "activate", (GCallback) med_input_menu_event, self, 0);
	      g_signal_connect_object (priv->menu->or, "activate", (GCallback) med_input_menu_event, self, 0);
	      g_signal_connect_object (priv->menu->and, "activate", (GCallback) med_input_menu_event, self, 0);
	      g_signal_connect_object (priv->menu->and_not, "activate", (GCallback) med_input_menu_event, self, 0);

        menu_input_enable_all (priv->menu, (g_strcmp0 (gtk_button_get_label (priv->entry), "") != 0));
        g_object_set ((GtkWidget*) priv->entry, "is-focus", TRUE, NULL);
        gtk_menu_popup ((GtkMenu*) priv->menu, NULL, NULL, NULL, NULL, event->button, event->time);
      }
      return TRUE;
    }
  }
  return FALSE;
}


static void
med_input_finalize (GObject * obj)
{
  MedInput * self = G_TYPE_CHECK_INSTANCE_CAST (obj, med_input_get_type (), MedInput);
  MedInputPrivate* priv = med_input_get_instance_private (self);

  g_free (priv->_command);
  g_free (priv->_label);
  g_free (priv->old_text);
  g_free (priv->internal_value);

  g_free(priv->value);

  G_OBJECT_CLASS (med_input_parent_class)->finalize (obj);
}


MedInput*
med_input_new (void)
{
  MedInput * self = (MedInput*) g_object_new (med_input_get_type (), NULL);
  return self;
}


static void
med_input_init (MedInput * self)
{
}


static GObject *
med_input_constructor (GType type,
                       guint n_construct_properties,
                       GObjectConstructParam * construct_properties)
{
  GObjectClass* parent_class = G_OBJECT_CLASS (med_input_parent_class);
  GObject* obj = parent_class->constructor (type, n_construct_properties, construct_properties);

  MedInput* self = G_TYPE_CHECK_INSTANCE_CAST (obj, med_input_get_type (), MedInput);
  MedInputPrivate* priv = med_input_get_instance_private (self);

  priv->entry = (GtkButton*) gtk_toggle_button_new ();
  //gtk_button_set_relief ((GtkButton*) priv->entry, GTK_RELIEF_NONE);

  priv->entry_label = (GtkLabel*) gtk_label_new (priv->_label);
  //gtk_label_set_xalign(priv->entry_label,1.00);                    // This needs GTK 3.16
  gtk_misc_set_alignment ((GtkMisc*) priv->entry_label, 1.00, 0.50); // so I am using this deprecated one.

  gtk_box_pack_start ((GtkBox*) self, (GtkWidget*) priv->entry_label, FALSE, FALSE, 0);
  gtk_box_pack_start ((GtkBox*) self, (GtkWidget*) priv->entry, TRUE, TRUE, 12);

  gtk_widget_show ((GtkWidget*) priv->entry_label);
  gtk_widget_show ((GtkWidget*) priv->entry);

  g_signal_connect_object ((GtkWidget*) priv->entry, "focus-out-event", (GCallback) med_input_entry_focus_out, self, 0);
  g_signal_connect_object ((GtkWidget*) priv->entry, "button-press-event", (GCallback) med_input_entry_mouse_clicked, self, 0);

  priv->is_active = FALSE;

  priv->old_text = g_strdup ("");
  priv->internal_value = g_strdup ("");

  priv->value = g_strdup("");

  med_widget_init ((MedWidget*) self, (GtkWidget*) self);

  return obj;
}

static void
med_input_get_property (GObject * object,
                        guint property_id,
                        GValue * value,
                        GParamSpec * pspec)
{
  MedInput * self = G_TYPE_CHECK_INSTANCE_CAST (object, med_input_get_type (), MedInput);

  switch (property_id)
  {
    case MED_INPUT_COMMAND_PROPERTY:
      g_value_set_string (value, med_widget_get_command ((MedWidget*) self));
    break;
    case MED_INPUT_UPDATED_PROPERTY:
      g_value_set_boolean (value, med_widget_get_updated ((MedWidget*) self));
    break;
    case MED_INPUT_LABEL_PROPERTY:
      g_value_set_string (value, med_input_get_label (self));
    break;
    case MED_INPUT_MODIFIER_KEYS_PROPERTY:
      g_value_set_boolean (value, med_input_get_modifier_keys (self));
    break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
med_input_set_property (GObject * object,
                        guint property_id,
                        const GValue * value,
                        GParamSpec * pspec)
{
  MedInput * self = G_TYPE_CHECK_INSTANCE_CAST (object, med_input_get_type (), MedInput);

  switch (property_id)
{
    case MED_INPUT_COMMAND_PROPERTY:
      med_widget_set_command ((MedWidget*) self, g_value_get_string (value));
    break;
    case MED_INPUT_UPDATED_PROPERTY:
      med_widget_set_updated ((MedWidget*) self, g_value_get_boolean (value));
    break;
    case MED_INPUT_LABEL_PROPERTY:
      med_input_set_label (self, g_value_get_string (value));
    break;
    case MED_INPUT_LABELWIDTH_PROPERTY:
      med_input_set_label_width (self, g_value_get_int (value));
    break;
    case MED_INPUT_MODIFIER_KEYS_PROPERTY:
      med_input_set_modifier_keys (self, g_value_get_boolean (value));
    break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
med_input_class_init (MedInputClass * klass)
{
  G_OBJECT_CLASS (klass)->get_property = med_input_get_property;
  G_OBJECT_CLASS (klass)->set_property = med_input_set_property;
  G_OBJECT_CLASS (klass)->constructor = med_input_constructor;
  G_OBJECT_CLASS (klass)->finalize = med_input_finalize;

  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_INPUT_COMMAND_PROPERTY,
                                   med_input_properties[MED_INPUT_COMMAND_PROPERTY] = g_param_spec_string
                                   (
                                     "command",
                                     "command",
                                     "command",
                                     NULL,
                                     G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE
                                   ));

  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_INPUT_UPDATED_PROPERTY,
                                   med_input_properties[MED_INPUT_UPDATED_PROPERTY] = g_param_spec_boolean
                                   (
                                     "updated",
                                     "updated",
                                     "updated",
                                     FALSE,
                                     G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE
                                   ));

  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_INPUT_LABEL_PROPERTY,
                                   med_input_properties[MED_INPUT_LABEL_PROPERTY] = g_param_spec_string
                                   (
                                     "label",
                                     "label",
                                     "label",
                                     NULL,
                                     G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT
                                   ));

	g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_INPUT_LABELWIDTH_PROPERTY,
                                   med_input_properties[MED_INPUT_LABELWIDTH_PROPERTY] = g_param_spec_int
                                   (
                                    "label-width",
                                    "label-width",
                                    "label-width",
                                    G_MININT, G_MAXINT, 0,
                                    G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE
                                   ));

  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_INPUT_MODIFIER_KEYS_PROPERTY,
                                   med_input_properties[MED_INPUT_MODIFIER_KEYS_PROPERTY] = g_param_spec_boolean
                                   (
                                     "modifier-keys",
                                     "modifier-keys",
                                     "modifier-keys",
                                     FALSE,
                                     G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE
                                   ));
}


static void
med_input_med_widget_interface_init (MedWidgetInterface * iface)
{
  iface->set_value = (void (*) (MedWidget *, const gchar*)) med_input_real_set_value;
  iface->get_value = (const gchar* (*) (MedWidget *)) med_input_real_get_value;

  iface->get_command = med_input_real_get_command;
  iface->set_command = med_input_real_set_command;
  iface->get_updated = med_input_real_get_updated;
  iface->set_updated = med_input_real_set_updated;
}
