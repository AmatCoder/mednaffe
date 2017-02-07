/*
 * toggles.c
 *
 * Copyright 2013-2017 AmatCoder
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
 */

#include "common.h"
#include "log.h"
#include "string.h"

void select_rows(guidata *gui)
{
  GtkTreeIter iter;
  GList *chain = NULL;

  if (gtk_tree_model_get_iter_first(
        gtk_tree_view_get_model(GTK_TREE_VIEW(gui->systemlist)), &iter))
    gtk_tree_selection_select_iter(
      gtk_tree_view_get_selection(GTK_TREE_VIEW(gui->systemlist)), &iter);

  if (!gtk_tree_selection_get_selected(gtk_tree_view_get_selection(
                          GTK_TREE_VIEW(gui->systemlist)), NULL, NULL))
    gtk_widget_hide(gui->notebook); else gtk_widget_show(gui->notebook);

  if (gtk_tree_model_get_iter_first(
          gtk_tree_view_get_model(GTK_TREE_VIEW(gui->globalist)), &iter))
    gtk_tree_selection_select_iter(gtk_tree_view_get_selection(
                                     GTK_TREE_VIEW(gui->globalist)), &iter);

  chain = g_list_prepend(chain, gui->gamelist);

  gtk_container_set_focus_chain(GTK_CONTAINER(
    gtk_builder_get_object(gui->builder, "vbox2")), chain);

  g_list_free(chain);
}

gchar *add_to_list (GtkWidget *widget, guidata *gui)
{
  gchar *name;
  gchar *fullname;
  gchar *nameinc;

  name = g_object_get_data(G_OBJECT(widget), "cname");

  if (name[0] == '.')
    fullname = g_strconcat(gui->system, name, NULL);
  else
    fullname = g_strdup(name);

  if (gui->changing == FALSE)
    g_hash_table_replace(gui->clist, fullname, fullname);

  fullname++;
  nameinc = g_strdup(fullname);
  fullname--;
  if (gui->changing == TRUE)
    g_free(fullname);

  return nameinc;
}

void toggle_changed(GtkWidget *widget, guidata *gui)
{
  gchar *name;

  name = add_to_list(widget, gui);

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    g_hash_table_insert(gui->hash, name, g_strdup("1"));
  else
    g_hash_table_insert(gui->hash, name, g_strdup("0"));
}

void combo_changed(GtkWidget *widget, guidata *gui)
{
  gchar *name;
  GtkTreeIter iter;
  GtkTreeModel *combostore;
  gchar *state;

  name = add_to_list(widget, gui);
  combostore = gtk_combo_box_get_model(GTK_COMBO_BOX(widget));
  gtk_combo_box_get_active_iter(GTK_COMBO_BOX(widget), &iter);
  gtk_tree_model_get(combostore, &iter, 0 , &state, -1);
  g_hash_table_insert(gui->hash, name, state);
}

void adj_changed(GtkWidget *widget, guidata *gui)
{
  gpointer name;
  GObject *objadj;
  gdouble afloat;
  gchar *value;
  gint digits;

  name = add_to_list(widget, gui);
  g_object_get(G_OBJECT(widget), "adjustment", &objadj, NULL);
  afloat = gtk_adjustment_get_value(GTK_ADJUSTMENT(objadj));
  value = g_strdup_printf("%i", (gint)afloat);

  if (GTK_IS_SPIN_BUTTON(widget))
  {
    digits = gtk_spin_button_get_digits(GTK_SPIN_BUTTON(widget));
    if (digits > 0)
    {
      g_free(value);
      value = g_strdup_printf("%.*f", digits, afloat);
    }
  }

  else if (GTK_IS_SCALE(widget))
  {
    digits = gtk_scale_get_digits(GTK_SCALE(widget));
    if (digits > 0)
    {
      g_free(value);
      value = g_strdup_printf("%.*f", digits, afloat);
    }
  }

  g_hash_table_insert(gui->hash, name, value);
  g_object_unref (objadj);
}

void entry_changed(GtkWidget *widget, guidata *gui)
{
  gpointer name;
  gchar *copy;
  const gchar *path;

  name = add_to_list(widget, gui);
  path = gtk_entry_get_text(GTK_ENTRY(widget));
  copy = g_strdup(path);
  g_hash_table_insert(gui->hash, name, copy);
}

/*gboolean adj_focus_out(GtkWidget *widget, GdkEvent *event, guidata *gui)
{
    adj_changed(widget, gui);
    return FALSE;
}*/

void set_toogle(gpointer widget, guidata *gui)
{
  gchar *name;
  gchar *fullname;

  name = g_object_get_data(G_OBJECT(widget), "cname");

  if (name[0] == '.')
    fullname = g_strconcat(gui->system, name, NULL);
  else
    fullname = g_strdup(name);

  fullname++;

  if ((g_strcmp0(g_hash_table_lookup (gui->hash, fullname),"1") == 0))
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), TRUE);
  else
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), FALSE);

  fullname--;
  g_free(fullname);
}

void set_combo(gpointer widget, guidata *gui)
{
  gchar *name;
  gchar *fullname;
  GtkTreeIter iter;
  GtkTreeModel *combostore;
  gchar *combovalue;

  name = g_object_get_data(G_OBJECT(widget), "cname");

  if (name[0] == '.')
    fullname = g_strconcat(gui->system, name, NULL);
  else
    fullname = g_strdup(name);

  fullname++;
  combostore = gtk_combo_box_get_model(GTK_COMBO_BOX(widget));
  gtk_tree_model_get_iter_first(combostore, &iter);

  do
  {
    gtk_tree_model_get(combostore, &iter, 0 , &combovalue, -1);
    if (g_strcmp0(g_hash_table_lookup(gui->hash, fullname),
                                                       combovalue) == 0)
    {
      gtk_combo_box_set_active_iter(GTK_COMBO_BOX(widget), &iter);
      g_free(combovalue);
      break;
    }
    g_free(combovalue);
  }
  while (gtk_tree_model_iter_next(combostore, &iter));

  fullname--;
  g_free(fullname);
}

void set_spin(gpointer widget, guidata *gui)
{
  gchar *name;
  gchar *fullname;
  gdouble value;
  GObject *objadj;
  const gchar *cvalue;

  name = g_object_get_data(G_OBJECT(widget), "cname");

  if (name[0] == '.')
    fullname = g_strconcat(gui->system, name, NULL);
  else
    fullname = g_strdup(name);

  fullname++;

  cvalue = g_hash_table_lookup (gui->hash, fullname);
  value = g_ascii_strtod(cvalue, NULL);
  g_object_get(G_OBJECT(widget), "adjustment", &objadj, NULL);
  gtk_adjustment_set_value(GTK_ADJUSTMENT(objadj), value);
  g_object_unref (objadj);

  fullname--;
  g_free(fullname);
}

void set_values(GtkBuilder *builder, guidata *gui)
{
  const gchar *name;
  const gchar *cvalue;
  GSList *list = NULL;
  GSList *iterator = NULL;

  list = gtk_builder_get_objects (builder);

  /* Exceptions here */
  list = g_slist_remove(list, gui->cbpath);
  /*                 */

  for (iterator = list; iterator; iterator = iterator->next)
  {
    if (GTK_IS_TOGGLE_BUTTON(iterator->data))
    {
      name = gtk_buildable_get_name(GTK_BUILDABLE(iterator->data));
      g_object_set_data(G_OBJECT(iterator->data), "cname",(gpointer)name);

      if (name[0] == '-')
        set_toogle(iterator->data, gui);
      else
        gui->dinlist = g_slist_prepend(gui->dinlist, iterator->data);

      g_signal_connect(GTK_TOGGLE_BUTTON(iterator->data), "toggled",
                                       G_CALLBACK(toggle_changed), gui);

    }
    else if (GTK_IS_COMBO_BOX(iterator->data))
    {
      name = gtk_buildable_get_name(GTK_BUILDABLE(iterator->data));
      g_object_set_data(G_OBJECT(iterator->data), "cname",(gpointer)name);

      if (name[0] == '-')
        set_combo(iterator->data, gui);
      else
        gui->dinlist = g_slist_prepend (gui->dinlist,iterator->data);

      g_signal_connect(GTK_COMBO_BOX(iterator->data), "changed",
                                        G_CALLBACK(combo_changed), gui);

    }
    else if (GTK_IS_SPIN_BUTTON(iterator->data) ||
            (GTK_IS_SCALE(iterator->data)))
    {
      name = gtk_buildable_get_name(GTK_BUILDABLE(iterator->data));
      g_object_set_data(G_OBJECT(iterator->data), "cname", (gpointer)name);

      if (name[0] == '-')
        set_spin(iterator->data, gui);
      else
        gui->dinlist = g_slist_prepend (gui->dinlist, iterator->data);

      g_signal_connect(GTK_WIDGET(iterator->data),"value-changed",
                                          G_CALLBACK(adj_changed), gui);

      /*g_signal_connect(GTK_WIDGET(iterator->data), "focus-out-event",
       *                              G_CALLBACK(adj_focus_out), gui);*/
#ifdef GTK3_ENABLED
      if GTK_IS_SPIN_BUTTON(iterator->data)
        gtk_widget_set_size_request(GTK_WIDGET(iterator->data), 150, -1);
#endif
    }
    else if (GTK_IS_ENTRY(iterator->data))
    {
      name = gtk_buildable_get_name(GTK_BUILDABLE(iterator->data));
      g_object_set_data(G_OBJECT(iterator->data), "cname", (gpointer)name);
      name++;
      cvalue = g_hash_table_lookup (gui->hash, name);

      if (cvalue != NULL)
        gtk_entry_set_text(GTK_ENTRY(iterator->data), cvalue);

      g_signal_connect(GTK_ENTRY(iterator->data), "changed",
                                        G_CALLBACK(entry_changed), gui);
    }
  }
  g_slist_free(list);
}

/*void test_watch(GFileMonitor *mon,
GFile *first,
GFile *second,
GFileMonitorEvent event,
guidata *gui)
{
if (event == G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT)
  {
  gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(gui->builder, "inputbutton")), TRUE);
  }
}*/

gchar* get_cfg(const gchar *home, guidata *gui)
{
  gchar *cfg_path;

  /* Search for mednafen configuration file */
  #ifdef G_OS_WIN32
    cfg_path = g_strconcat(home, "\\mednafen-09x.cfg", NULL);
  #else
    cfg_path = g_strconcat(home, "/.mednafen/mednafen-09x.cfg", NULL);
  #endif

  if (g_file_test (cfg_path, G_FILE_TEST_IS_REGULAR))
    print_log("Starting Mednaffe 0.9.0beta\n\
Mednafen 09x configuration file found.\n", FE, gui);
  else return NULL;

  /*{
  #ifdef G_OS_WIN32
    cfg_path = g_strconcat(home, "\\mednafen.cfg", NULL);
  #else
    cfg_path = g_strconcat(home, "/.mednafen/mednafen.cfg", NULL);
  #endif
    if (g_file_test (cfg_path, G_FILE_TEST_IS_REGULAR))
      printf(" Mednafen 08x configuration file found\n");
    else
      cfg_path = NULL;
  }*/

  return cfg_path;
}

gboolean check_version(gchar *stout, guidata *gui)
{
  if (stout)
  {
    if (strlen(stout)<1) return FALSE;

    gchar **achar = g_strsplit(stout, "\n", 0);
    gchar **aline = g_strsplit(achar[0], " ", 2);

    if (g_strv_length(aline)<2) return FALSE;
    if (strlen(aline[1])<15) return FALSE;

    if (aline[1][11]!='9')
      return FALSE;
    if (aline[1][13]-'0'<4)
      return FALSE;
    /* if ((aline[1][13]=='3') && ((aline[1][14]-'0')<9))
      return FALSE; */
    /*
    if (strlen(aline[1])>16)
    {
      if ((aline[1][13]=='3') && (aline[1][14]=='6') && ((aline[1][16]-'0')<2))
        return FALSE;
    }
    else
    {
    if ((aline[1][13]=='3') && ((aline[1][14]-'0')<7))
      return FALSE;
    }
  */
    print_log("Version detected: ", FE, gui);
    print_log(aline[1], FE, gui);
    #ifdef G_OS_UNIX
      print_log("\n", FE, gui);
    #endif
    print_log("----\n", FE, gui);

    GtkStatusbar *sbversion = GTK_STATUSBAR(gtk_builder_get_object(gui->builder, "sbversion"));
    gtk_statusbar_push(GTK_STATUSBAR(sbversion), 1, aline[1]);

    gtk_widget_set_tooltip_text(GTK_WIDGET(sbversion), gui->binpath);

    g_strfreev(achar);
    g_strfreev(aline);

    return TRUE;
  }
  else return FALSE;
}

gboolean read_cfg(guidata *gui)
{
  gchar *string;
  gint num = 0;
  gint i;

  /*GFile *file = g_file_new_for_path(cfg_path);
  GFileMonitor *test = g_file_monitor (file, G_FILE_MONITOR_SEND_MOVED, NULL, NULL);
  g_signal_connect (test, "changed", G_CALLBACK (test_watch), gui);*/

  if (g_file_get_contents(gui->cfgfile, &string, NULL, NULL))
  {
    gchar **achar;
    gchar **aline;

    achar = g_strsplit(string, "\n", 0);
    num = g_strv_length(achar);

    for (i = 0; i < num; i++)
    {
      if ((*achar[i] != ';') && (*achar[i] != 0))
      {
        aline = g_strsplit(achar[i], " ", 2);
        g_hash_table_insert(gui->hash, aline[0], aline[1]);
        g_free(aline);
      }
    }
    g_free(string);
    g_strfreev(achar);
  }
  else return FALSE;

  return TRUE;
}
