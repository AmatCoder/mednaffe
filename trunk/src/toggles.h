/*
 * toggles.h
 * 
 * Copyright 2013 AmatCoder
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
 
#ifndef TOGGLES_H
#define TOGGLES_H

  void select_rows(guidata *gui);
  void set_toogle(gpointer widget, guidata *gui);
  void set_combo(gpointer widget, guidata *gui);
  void set_spin(gpointer widget, guidata *gui);
  void set_values(GtkBuilder *builder, guidata *gui);
  gboolean read_cfg(gchar * cfg_path, guidata *gui);

#endif /* TOGGLES_H */
