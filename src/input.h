/*
 * input.h
 * 
 * Copyright 2013-2015 AmatCoder
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
 
#ifndef INPUT_H
#define INPUT_H

static const guint const gdk_to_sdl[256] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 8, 9, 13, 12, 0, 13, 0, 0,
  0, 0, 0, 19, 302, 317, 0, 0, 0, 0, 0, 27, 0, 0, 0, 0,
  314, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  278, 276, 273, 275, 274, 280, 281, 279, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 316, 0, 277, 322, 0, 0, 319, 0, 315, 318, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 313, 300,
  261, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 271, 0, 0,
  0, 0, 0, 0, 0, 263, 260, 264, 262, 258, 265, 259, 257, 261, 256, 266,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 268, 270, 0, 269, 266, 267,
  256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 0, 0, 0, 272, 282, 283,
  284, 285, 286, 287, 288, 289, 290, 291, 292, 293, 294, 295, 296, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 304, 303, 306, 305, 301, 0, 310, 309, 308, 307, 311, 312, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 127            
};

static const char* const sdl_to_gdk[324] =
{
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  "Backspace", "Tab", NULL, NULL, "Clear", "Enter", NULL, NULL,
  NULL, NULL, NULL, "Pause", NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, "Escape", NULL, NULL, NULL, NULL,
  "Space", "!", "\"", "#", "$", NULL, "&", "'",
  "(", ")", "*", "+", ",", "-", ".", "/",
  "0", "1", "2", "3", "4", "5", "6", "7",
  "8", "9", ":", ";", "<", "=", ">", "?",
  "@", NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, "[", "\\", "]", "^", "_",
  "`", "A", "B", "C", "D", "E", "F", "G",
  "H", "I", "J", "K", "L", "M", "N", "O",
  "P", "Q", "R", "S", "T", "U", "V", "W",
  "X", "Y", "Z", "{", "|", "}", "~", "Delete",
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  "Numpad 0", "Numpad 1", "Numpad 2", "Numpad 3",
  "Numpad 4", "Numpad 5", "Numpad 6", "Numpad 7",
  "Numpad 8", "Numpad 9", "Numpad .", "Numpad /",
  "Numpad *", "Numpad -", "Numpad +", "Numpad Enter",
  "Numpad =", "Up", "Down", "Right", "Left", "Insert", "Home", "End",
  "Page Up", "PageDown", "F1", "F2", "F3", "F4", "F5", "F6",
  "F7", "F8", "F9", "F10", "F11", "F12", "F13", "F14",
  "F15", NULL, NULL, NULL, 
  "Num Lock", "Caps Lock", "Scroll Lock", "Right Shift",
  "Left Shift", "Right Ctrl", "Left Ctrl", "Right Alt",
  "Left Alt", "Right Meta", "Left Meta", "Left Super",
  "Right Super", "Alt Gr", "Compose", "Help",
  "Print Screen", "SysRq", "Break", "Menu",
  "Power", "Euro", "Undo", NULL
};

#endif /* INPUT_H */
