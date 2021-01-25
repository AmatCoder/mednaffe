/*
 * marshallers.c
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


#include <glib-object.h>


void
g_cclosure_user_marshal_VOID__STRING_STRING (GClosure     *closure,
                                             GValue       *return_value,
                                             guint         n_param_values,
                                             const GValue *param_values,
                                             gpointer      invocation_hint,
                                             gpointer      marshal_data)
{
  typedef void (*GMarshalFunc_VOID__STRING_STRING) (gpointer data1,
                                                    gpointer arg1,
                                                    gpointer arg2,
                                                    gpointer data2);
  GCClosure *cc = (GCClosure *) closure;
  gpointer data1, data2;
  GMarshalFunc_VOID__STRING_STRING callback;

  g_return_if_fail (n_param_values == 3);

  if (G_CCLOSURE_SWAP_DATA (closure))
  {
    data1 = closure->data;
    data2 = param_values->data[0].v_pointer;
  }
  else
  {
    data1 = param_values->data[0].v_pointer;
    data2 = closure->data;
  }

  callback = (GMarshalFunc_VOID__STRING_STRING) (marshal_data ? marshal_data : cc->callback);

  callback (data1,
            (char*) g_value_get_string (param_values + 1),
            (char*) g_value_get_string (param_values + 2),
            data2);
}


void
g_cclosure_user_marshal_VOID__INT_BOOLEAN (GClosure     *closure,
                                           GValue       *return_value,
                                           guint         n_param_values,
                                           const GValue *param_values,
                                           gpointer      invocation_hint,
                                           gpointer      marshal_data)
{
  typedef void (*GMarshalFunc_VOID__INT_BOOLEAN) (gpointer data1,
                                                  gint arg1,
                                                  gboolean arg2,
                                                  gpointer data2);
  GCClosure *cc = (GCClosure *) closure;
  gpointer data1, data2;
  GMarshalFunc_VOID__INT_BOOLEAN callback;

  g_return_if_fail (n_param_values == 3);

  if (G_CCLOSURE_SWAP_DATA (closure))
  {
    data1 = closure->data;
    data2 = param_values->data[0].v_pointer;
  }
  else
  {
    data1 = param_values->data[0].v_pointer;
    data2 = closure->data;
  }

  callback = (GMarshalFunc_VOID__INT_BOOLEAN) (marshal_data ? marshal_data : cc->callback);

  callback (data1,
            g_value_get_int (param_values + 1),
            g_value_get_boolean (param_values + 2),
            data2);
}


void
g_cclosure_user_marshal_VOID__INT_INT (GClosure     *closure,
                                       GValue       *return_value,
                                       guint         n_param_values,
                                       const GValue *param_values,
                                       gpointer      invocation_hint,
                                       gpointer      marshal_data)
{
  typedef void (*GMarshalFunc_VOID__INT_INT) (gpointer data1,
                                                  gint arg1,
                                                  gint arg2,
                                                  gpointer data2);
  GCClosure *cc = (GCClosure *) closure;
  gpointer data1, data2;
  GMarshalFunc_VOID__INT_INT callback;

  g_return_if_fail (n_param_values == 3);

  if (G_CCLOSURE_SWAP_DATA (closure))
  {
    data1 = closure->data;
    data2 = param_values->data[0].v_pointer;
  }
  else
  {
    data1 = param_values->data[0].v_pointer;
    data2 = closure->data;
  }

  callback = (GMarshalFunc_VOID__INT_INT) (marshal_data ? marshal_data : cc->callback);

  callback (data1,
            g_value_get_int (param_values + 1),
            g_value_get_int (param_values + 2),
            data2);
}


void
g_cclosure_user_marshal_VOID__STRING_STRING_INT_INT_INT (GClosure     *closure,
                                                         GValue       *return_value,
                                                         guint         n_param_values,
                                                         const GValue *param_values,
                                                         gpointer      invocation_hint,
                                                         gpointer      marshal_data)
{
  typedef void (*GMarshalFunc_VOID__STRING_STRING_INT_INT_INT) (gpointer data1,
                                                                gpointer arg1,
                                                                gpointer arg2,
                                                                gint arg3,
                                                                gint arg4,
                                                                gint arg5,
                                                                gpointer data2);
  GCClosure *cc = (GCClosure *) closure;
  gpointer data1, data2;
  GMarshalFunc_VOID__STRING_STRING_INT_INT_INT callback;

  g_return_if_fail (n_param_values == 6);

  if (G_CCLOSURE_SWAP_DATA (closure))
  {
    data1 = closure->data;
    data2 = param_values->data[0].v_pointer;
  }
  else
  {
    data1 = param_values->data[0].v_pointer;
    data2 = closure->data;
  }

  callback = (GMarshalFunc_VOID__STRING_STRING_INT_INT_INT) (marshal_data ? marshal_data : cc->callback);

  callback (data1,
            (char*) g_value_get_string (param_values + 1),
            (char*) g_value_get_string (param_values + 2),
            g_value_get_int (param_values + 3),
            g_value_get_int (param_values + 4),
            g_value_get_int (param_values + 5),
            data2);
}
