/*
 * panedimage.h
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


#ifndef __PANEDIMAGE_H__
#define __PANEDIMAGE_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS


typedef struct _PanedImage PanedImage;

struct _PanedImage {
  GtkPaned parent_instance;
};


GType paned_image_get_type (void) G_GNUC_CONST;
PanedImage* paned_image_new (GtkOrientation orientation);

void paned_image_set_images (PanedImage* self,const gchar* str);
void paned_image_set_paths (PanedImage* self, const gchar* str_a, const gchar* str_b);


G_END_DECLS

#endif
