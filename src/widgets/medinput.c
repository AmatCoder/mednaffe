/*
 * medinput.h
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
  MenuInput* menu;
  gchar* _command;
  gboolean _updated;
  gboolean _modified;
  gchar* _label;
  gboolean _modifier_keys;
  gboolean _only_joy;
  gboolean is_active;
  gboolean is_mouse;
  gchar* old_text;
  gchar* internal_value;
  gchar* value;
  MedListJoy* listjoy;
};


enum  {
  MED_INPUT_0_PROPERTY,
  MED_INPUT_COMMAND_PROPERTY,
  MED_INPUT_LABEL_PROPERTY,
  MED_INPUT_LABELWIDTH_PROPERTY,
  MED_INPUT_MODIFIER_KEYS_PROPERTY,
  MED_INPUT_ONLY_JOY_PROPERTY,
  MED_INPUT_NUM_PROPERTIES
};

static GParamSpec* med_input_properties[MED_INPUT_NUM_PROPERTIES];


static void med_input_med_widget_interface_init (MedWidgetInterface* iface);

static gboolean med_input_entry_key_press (GtkWidget* sender,
                                           GdkEventKey* event,
                                           gpointer self);

static void med_input_joy_event (MedListJoy* sender,
                                 const gchar* text,
                                 const gchar* value,
                                 gpointer self);


G_DEFINE_TYPE_WITH_CODE (MedInput, med_input, GTK_TYPE_BOX,
                         G_ADD_PRIVATE (MedInput)
                         G_IMPLEMENT_INTERFACE (med_widget_get_type(), med_input_med_widget_interface_init));

#ifdef G_OS_UNIX
#ifdef __APPLE__
static const guint sdl_to_gdk[245] = {
  0,  /* SDL_SCANCODE_UNKNOWN = 0 */
  0,  /* SDL_SCANCODE_UNKNOWN = 1 */
  0,  /* SDL_SCANCODE_UNKNOWN = 2 */
  0,  /* SDL_SCANCODE_UNKNOWN = 3 */
  0,  /* SDL_SCANCODE_A = 4 */
  11,  /* SDL_SCANCODE_B = 5 */
  8,  /* SDL_SCANCODE_C = 6 */
  2,  /* SDL_SCANCODE_D = 7 */
  14,  /* SDL_SCANCODE_E = 8 */
  3,  /* SDL_SCANCODE_F = 9 */
  5,  /* SDL_SCANCODE_G = 10 */
  4,  /* SDL_SCANCODE_H = 11 */
  34,  /* SDL_SCANCODE_I = 12 */
  38,  /* SDL_SCANCODE_J = 13 */
  40,  /* SDL_SCANCODE_K = 14 */
  37,  /* SDL_SCANCODE_L = 15 */
  46,  /* SDL_SCANCODE_M = 16 */
  45,  /* SDL_SCANCODE_N = 17 */
  31,  /* SDL_SCANCODE_O = 18 */
  35,  /* SDL_SCANCODE_P = 19 */
  12,  /* SDL_SCANCODE_Q = 20 */
  15,  /* SDL_SCANCODE_R = 21 */
  1,  /* SDL_SCANCODE_S = 22 */
  17,  /* SDL_SCANCODE_T = 23 */
  32,  /* SDL_SCANCODE_U = 24 */
  9,  /* SDL_SCANCODE_V = 25 */
  13,  /* SDL_SCANCODE_W = 26 */
  7,  /* SDL_SCANCODE_X = 27 */
  16,  /* SDL_SCANCODE_Y = 28 */
  6,  /* SDL_SCANCODE_Z = 29 */
  18,  /* SDL_SCANCODE_1 = 30 */
  19,  /* SDL_SCANCODE_2 = 31 */
  20,  /* SDL_SCANCODE_3 = 32 */
  21,  /* SDL_SCANCODE_4 = 33 */
  23,  /* SDL_SCANCODE_5 = 34 */
  22,  /* SDL_SCANCODE_6 = 35 */
  26,  /* SDL_SCANCODE_7 = 36 */
  28,  /* SDL_SCANCODE_8 = 37 */
  25,  /* SDL_SCANCODE_9 = 38 */
  29,  /* SDL_SCANCODE_0 = 39 */
  36,  /* SDL_SCANCODE_RETURN = 40 */
  53,  /* SDL_SCANCODE_ESCAPE = 41 */
  51,  /* SDL_SCANCODE_BACKSPACE = 42 */
  48,  /* SDL_SCANCODE_TAB = 43 */
  49,  /* SDL_SCANCODE_SPACE = 44 */
  27,  /* SDL_SCANCODE_MINUS = 45 */
  24,  /* SDL_SCANCODE_EQUALS = 46 */
  33,  /* SDL_SCANCODE_LEFTBRACKET = 47 */
  30,  /* SDL_SCANCODE_RIGHTBRACKET = 48 */
  42,  /* SDL_SCANCODE_BACKSLASH = 49 */
  0,  /* SDL_SCANCODE_NONUSHASH = 50 */
  41,  /* SDL_SCANCODE_SEMICOLON = 51 */
  39,  /* SDL_SCANCODE_APOSTROPHE = 52 */
  50,  /* SDL_SCANCODE_GRAVE = 53 */
  43,  /* SDL_SCANCODE_COMMA = 54 */
  47,  /* SDL_SCANCODE_PERIOD = 55 */
  44,  /* SDL_SCANCODE_SLASH = 56 */
  57,  /* SDL_SCANCODE_CAPSLOCK = 57 */
  122,  /* SDL_SCANCODE_F1 = 58 */
  120,  /* SDL_SCANCODE_F2 = 59 */
  99,  /* SDL_SCANCODE_F3 = 60 */
  118,  /* SDL_SCANCODE_F4 = 61 */
  96,  /* SDL_SCANCODE_F5 = 62 */
  97,  /* SDL_SCANCODE_F6 = 63 */
  98,  /* SDL_SCANCODE_F7 = 64 */
  100,  /* SDL_SCANCODE_F8 = 65 */
  101,  /* SDL_SCANCODE_F9 = 66 */
  109,  /* SDL_SCANCODE_F10 = 67 */
  103,  /* SDL_SCANCODE_F11 = 68 */
  111,  /* SDL_SCANCODE_F12 = 69 */
  105,  /* SDL_SCANCODE_PRINTSCREEN = 70 */
  107,  /* SDL_SCANCODE_SCROLLOCK = 71 */
  113,  /* SDL_SCANCODE_PAUSE = 72 */
  114,  /* SDL_SCANCODE_INSERT = 73 */
  115,  /* SDL_SCANCODE_HOME = 74 */
  116,  /* SDL_SCANCODE_PAGEUP = 75 */
  117,  /* SDL_SCANCODE_DELETE = 76 */
  119,  /*   SDL_SCANCODE_END = 77 */
  121,  /* SDL_SCANCODE_PAGEDOWN = 78 */
  124,  /* SDL_SCANCODE_RIGHT = 79 */
  123,  /* SDL_SCANCODE_LEFT = 80 */
  125,  /* SDL_SCANCODE_DOWN = 81 */
  126,  /* SDL_SCANCODE_UP = 82 */
  71,  /* SDL_SCANCODE_NUMLOCKCLEAR = 83 */
  75,  /* SDL_SCANCODE_KP_DIVIDE = 84 */
  67,  /* SDL_SCANCODE_KP_MULTIPLY = 85 */
  78,  /* SDL_SCANCODE_KP_MINUS = 86 */
  69,  /* SDL_SCANCODE_KP_PLUS = 87 */
  76,  /* SDL_SCANCODE_KP_ENTER = 88 */
  83,  /* SDL_SCANCODE_KP_1 = 89 */
  84,  /* SDL_SCANCODE_KP_2 = 90 */
  85,  /* SDL_SCANCODE_KP_3 = 91 */
  86,  /* SDL_SCANCODE_KP_4 = 92 */
  87,  /* SDL_SCANCODE_KP_5 = 93 */
  88,  /* SDL_SCANCODE_KP_6 = 94 */
  89,  /* SDL_SCANCODE_KP_7 = 95 */
  91,  /* SDL_SCANCODE_KP_8 = 96 */
  92,  /* SDL_SCANCODE_KP_9 = 97 */
  82,  /* SDL_SCANCODE_KP_0 = 98 */
  65,  /* SDL_SCANCODE_KP_PERIOD = 99 */
  10,  /* SDL_SCANCODE_NONUSBACKSLASH = 100 */
  110,  /* SDL_SCANCODE_APPLICATION = 101 */
  0,  /* SDL_SCANCODE_POWER = 102 */
  81,  /*   SDL_SCANCODE_KP_EQUALS = 103 */
  0,  /* SDL_SCANCODE_F13 = 104 */
  0,  /* SDL_SCANCODE_F14 = 105 */
  0,  /* SDL_SCANCODE_F15 = 106 */
  106,  /* SDL_SCANCODE_F16 = 107 */
  64,  /* SDL_SCANCODE_F17 = 108 */
  79,  /* SDL_SCANCODE_F18 = 109 */
  80,  /* SDL_SCANCODE_F19 = 110 */
  90,  /* SDL_SCANCODE_F20 = 111 */
  0,  /* SDL_SCANCODE_F21 = 112 */
  0,  /* SDL_SCANCODE_F22 = 113 */
  0,  /* SDL_SCANCODE_F23 = 114 */
  0,  /* SDL_SCANCODE_F24 = 115 */
  0,  /* SDL_SCANCODE_EXECUTE = 116 */
  0,  /* SDL_SCANCODE_HELP = 117 */
  0,  /* SDL_SCANCODE_MENU = 118 */
  0,  /*   SDL_SCANCODE_SELECT = 119 */
  0,  /* SDL_SCANCODE_STOP = 120 */
  0,  /* SDL_SCANCODE_AGAIN = 121 */
  0,  /* SDL_SCANCODE_UNDO = 122 */
  0,  /* SDL_SCANCODE_CUT = 123 */
  0,  /* SDL_SCANCODE_COPY = 124 */
  0,  /* SDL_SCANCODE_PASTE = 125 */
  0,  /* SDL_SCANCODE_FIND = 126 */
  0,  /* SDL_SCANCODE_MUTE = 127 */
  0,  /* SDL_SCANCODE_VOLUMEUP = 128 */
  0,  /* SDL_SCANCODE_VOLUMEDOWN = 129 */
  0,  /* SDL_SCANCODE_LOCKINGCAPSLOCK = 130 */
  0,  /* SDL_SCANCODE_LOCKINGNUMLOCK = 131 */
  0,  /* SDL_SCANCODE_LOCKINGSCROLLLOCK = 132 */
  0,  /* SDL_SCANCODE_KP_COMMA = 133 */
  0,  /* SDL_SCANCODE_KP_EQUALSAS400 = 134 */
  0,  /* SDL_SCANCODE_INTERNATIONAL1 = 135 */
  0,  /* SDL_SCANCODE_INTERNATIONAL2 = 136 */
  0,  /* SDL_SCANCODE_INTERNATIONAL3 = 137 */
  0,  /* SDL_SCANCODE_INTERNATIONAL4 = 138 */
  0,  /* SDL_SCANCODE_INTERNATIONAL5 = 139 */
  0,  /* SDL_SCANCODE_INTERNATIONAL6 = 140 */
  0,  /* SDL_SCANCODE_INTERNATIONAL7 = 141 */
  0,  /* SDL_SCANCODE_INTERNATIONAL8 = 142 */
  0,  /* SDL_SCANCODE_INTERNATIONAL9 = 143 */
  0,  /* SDL_SCANCODE_LANG1 = 144 */
  0,  /* SDL_SCANCODE_LANG2 = 145 */
  0,  /* SDL_SCANCODE_LANG3 = 146 */
  0,  /* SDL_SCANCODE_LANG4 = 147 */
  0,  /* SDL_SCANCODE_LANG5 = 148 */
  0,  /* SDL_SCANCODE_LANG6 = 149 */
  0,  /* SDL_SCANCODE_LANG7 = 150 */
  0,  /* SDL_SCANCODE_LANG8 = 151 */
  0,  /* SDL_SCANCODE_LANG9 = 152 */
  0,  /* SDL_SCANCODE_ALTERASE = 153 */
  0,  /* SDL_SCANCODE_SYSREQ = 154 */
  0,  /* SDL_SCANCODE_CANCEL = 155 */
  0,  /* SDL_SCANCODE_CLEAR = 156 */
  0,  /* SDL_SCANCODE_PRIOR = 157 */
  0,  /* SDL_SCANCODE_RETURN2 = 158 */
  0,  /* SDL_SCANCODE_SEPARATOR = 159 */
  0,  /* SDL_SCANCODE_OUT = 160 */
  0,  /* SDL_SCANCODE_OPER = 161 */
  0,  /* SDL_SCANCODE_CLEARAGAIN = 162 */
  0,  /* SDL_SCANCODE_CRSEL = 163 */
  0,  /* SDL_SCANCODE_EXSEL = 164 */
  0,  /* SDL_SCANCODE_UNKNOWN = 165 */
  0,  /* SDL_SCANCODE_UNKNOWN = 166 */
  0,  /* SDL_SCANCODE_UNKNOWN = 167 */
  0,  /* SDL_SCANCODE_UNKNOWN = 168 */
  0,  /* SDL_SCANCODE_UNKNOWN = 169 */
  0,  /* SDL_SCANCODE_UNKNOWN = 170 */
  0,  /* SDL_SCANCODE_UNKNOWN = 171 */
  0,  /* SDL_SCANCODE_UNKNOWN = 172 */
  0,  /* SDL_SCANCODE_UNKNOWN = 173 */
  0,  /* SDL_SCANCODE_UNKNOWN = 174 */
  0,  /* SDL_SCANCODE_UNKNOWN = 175 */
  0,  /* SDL_SCANCODE_KP_00 = 176 */
  0,  /* SDL_SCANCODE_KP_000 = 177 */
  0,  /* SDL_SCANCODE_THOUSANDSSEPARATOR = 178 */
  0,  /* SDL_SCANCODE_DECIMALSEPARATOR = 179 */
  0,  /* SDL_SCANCODE_CURRENCYUNIT = 180 */
  0,  /* SDL_SCANCODE_CURRENCYSUBUNIT = 181 */
  0,  /* SDL_SCANCODE_KP_LEFTPAREN = 182 */
  0,  /* SDL_SCANCODE_KP_RIGHTPAREN = 183 */
  0,  /* SDL_SCANCODE_KP_LEFTBRACE = 184 */
  0,  /* SDL_SCANCODE_KP_RIGHTBRACE = 185 */
  0,  /* SDL_SCANCODE_KP_TAB = 186 */
  0,  /* SDL_SCANCODE_KP_BACKSPACE = 187 */
  0,  /* SDL_SCANCODE_KP_A = 188 */
  0,  /* SDL_SCANCODE_KP_B = 189 */
  0,  /* SDL_SCANCODE_KP_C = 190 */
  0,  /* SDL_SCANCODE_KP_D = 191 */
  0,  /* SDL_SCANCODE_KP_E = 192 */
  0,  /* SDL_SCANCODE_KP_F = 193 */
  0,  /* SDL_SCANCODE_KP_XOR = 194 */
  0,  /* SDL_SCANCODE_KP_POWER = 195 */
  0,  /* SDL_SCANCODE_KP_PERCENT = 196 */
  0,  /* SDL_SCANCODE_KP_LESS = 197 */
  0,  /* SDL_SCANCODE_KP_GREATER = 198 */
  0,  /* SDL_SCANCODE_KP_AMPERSAND = 199 */
  0,  /* SDL_SCANCODE_KP_DBLAMPERSAND = 200 */
  0,  /* SDL_SCANCODE_KP_VERTICALBAR = 201 */
  0,  /* SDL_SCANCODE_KP_DBLVERTICALBAR = 202 */
  0,  /* SDL_SCANCODE_KP_COLON = 203 */
  0,  /* SDL_SCANCODE_KP_HASH = 204 */
  0,  /* SDL_SCANCODE_KP_SPACE = 205 */
  0,  /* SDL_SCANCODE_KP_AT = 206 */
  0,  /* SDL_SCANCODE_KP_EXCLAM = 207 */
  0,  /* SDL_SCANCODE_KP_MEMSTORE = 208 */
  0,  /* SDL_SCANCODE_KP_MEMRECALL = 209 */
  0,  /* SDL_SCANCODE_KP_MEMCLEAR = 210 */
  0,  /* SDL_SCANCODE_KP_MEMADD = 211 */
  0,  /* SDL_SCANCODE_KP_MEMSUBTRACT = 212 */
  0,  /* SDL_SCANCODE_KP_MEMMULTIPLY = 213 */
  0,  /* SDL_SCANCODE_KP_MEMDIVIDE = 214 */
  0,  /* SDL_SCANCODE_KP_PLUSMINUS = 215 */
  0,  /* SDL_SCANCODE_KP_CLEAR = 216 */
  0,  /* SDL_SCANCODE_KP_CLEARENTRY = 217 */
  0,  /* SDL_SCANCODE_KP_BINARY = 218 */
  0,  /* SDL_SCANCODE_KP_OCTAL = 219 */
  0,  /* SDL_SCANCODE_KP_DECIMAL = 220 */
  0,  /* SDL_SCANCODE_KP_HEXADECIMAL = 221 */
  0,  /* SDL_SCANCODE_UNKNOWN = 222 */
  0,  /* SDL_SCANCODE_UNKNOWN = 223 */
  59,  /* SDL_SCANCODE_LCTRL = 224 */
  56,  /* SDL_SCANCODE_LSHIFT = 225 */
  58,  /* SDL_SCANCODE_LALT = 226 */
  55,  /* SDL_SCANCODE_LGUI = 227 */
  62,  /* SDL_SCANCODE_RCTRL = 228 */
  60,  /* SDL_SCANCODE_RSHIFT = 229 */
  61,  /* SDL_SCANCODE_RALT = 230 */
  54,  /* SDL_SCANCODE_RGUI = 231 */
  0,  /* SDL_SCANCODE_UNKNOWN = 232 */
  0,  /* SDL_SCANCODE_UNKNOWN = 233 */
  0,  /* SDL_SCANCODE_UNKNOWN = 234 */
  0,  /* SDL_SCANCODE_UNKNOWN = 235 */
  0,  /* SDL_SCANCODE_UNKNOWN = 236 */
  0,  /* SDL_SCANCODE_UNKNOWN = 237 */
  0,  /* SDL_SCANCODE_UNKNOWN = 238 */
  0,  /* SDL_SCANCODE_UNKNOWN = 239 */
  0,  /* SDL_SCANCODE_UNKNOWN = 240 */
  0,  /* SDL_SCANCODE_UNKNOWN = 241 */
  0,  /* SDL_SCANCODE_UNKNOWN = 242 */
  0,  /* SDL_SCANCODE_UNKNOWN = 243 */
  0
};

static const guint gdk_to_sdl[245] = {
  4,  /* 0  kVK_ANSI_A */
  22,  /* 1  kVK_ANSI_S */
  7,  /* 2  kVK_ANSI_D */
  9,  /* 3  kVK_ANSI_F */
  11,  /* 4  kVK_ANSI_H */
  10,  /* 5  kVK_ANSI_G */
  29,  /* 6  kVK_ANSI_Z */
  27,  /* 7  kVK_ANSI_X */
  6,  /* 8  kVK_ANSI_C */
  25,  /* 9  kVK_ANSI_V */
  100,  /* 10  kVK_ISO_Section */
  5,  /* 11  kVK_ANSI_B */
  20,  /* 12  kVK_ANSI_Q */
  26,  /* 13  kVK_ANSI_W */
  8,  /* 14  kVK_ANSI_E */
  21,  /* 15  kVK_ANSI_R */
  28,  /* 16  kVK_ANSI_Y */
  23,  /* 17  kVK_ANSI_T */
  30,  /* 18  kVK_ANSI_1 */
  31,  /* 19  kVK_ANSI_2 */
  32,  /* 20  kVK_ANSI_3 */
  33,  /* 21  kVK_ANSI_4 */
  35,  /* 22  kVK_ANSI_6 */
  34,  /* 23  kVK_ANSI_5 */
  46,  /* 24  kVK_ANSI_Equal */
  38,  /* 25  kVK_ANSI_9 */
  36,  /* 26  kVK_ANSI_7 */
  45,  /* 27  kVK_ANSI_Minus */
  37,  /* 28  kVK_ANSI_8 */
  39,  /* 29  kVK_ANSI_0 */
  48,  /* 30  kVK_ANSI_RightBracket */
  18,  /* 31  kVK_ANSI_O */
  24,  /* 32  kVK_ANSI_U */
  47,  /* 33  kVK_ANSI_LeftBracket */
  12,  /* 34  kVK_ANSI_I */
  19,  /* 35  kVK_ANSI_P */
  40,  /* 36  kVK_Return */
  15,  /* 37  kVK_ANSI_L */
  13,  /* 38  kVK_ANSI_J */
  52,  /* 39  kVK_ANSI_Quote */
  14,  /* 40  kVK_ANSI_K */
  51,  /* 41  kVK_ANSI_Semicolon */
  49,  /* 42  kVK_ANSI_Backslash */
  54,  /* 43  kVK_ANSI_Comma */
  56,  /* 44  kVK_ANSI_Slash */
  17,  /* 45  kVK_ANSI_N */
  16,  /* 46  kVK_ANSI_M */
  55,  /* 47  kVK_ANSI_Period */
  43,  /* 48  kVK_Tab */
  44,  /* 49  kVK_Space */
  53,  /* 50  kVK_ANSI_Grave */
  42,  /* 51  kVK_Delete */
  0,  /* 52 */
  41,  /* 53  kVK_Escape */
  231,  /* 54  kVK_RightCommand */
  227,  /* 55  kVK_Command */
  225,  /* 56  kVK_Shift */
  57,  /* 57  kVK_CapsLock */
  226,  /* 58  kVK_Option */
  224,  /* 59  kVK_Control */
  229,  /* 60  kVK_RightShift */
  230,  /* 61  kVK_RightOption */
  228,  /* 62  kVK_RightControl */
  0,  /* 63  kVK_Function */
  108,  /* 64  kVK_F17 */
  99,  /* 65  kVK_ANSI_KeypadDecimal */
  0,  /* 66 */
  85,  /* 67  kVK_ANSI_KeypadMultiply */
  0,  /* 68 */
  87,  /* 69  kVK_ANSI_KeypadPlus */
  0,  /* 70 */
  83,  /* 71  kVK_ANSI_KeypadClear */
  0,  /* 72  kVK_VolumeUp */
  0,  /* 73  kVK_VolumeDown */
  0,  /* 74  kVK_Mute */
  84,  /* 75  kVK_ANSI_KeypadDivide */
  88,  /* 76  kVK_ANSI_KeypadEnter */
  0,  /* 77 */
  86,  /* 78  kVK_ANSI_KeypadMinus */
  109,  /* 79  kVK_F18 */
  110,  /* 80  kVK_F19 */
  103,  /* 81  kVK_ANSI_KeypadEquals */
  98,  /* 82  kVK_ANSI_Keypad0 */
  89,  /* 83  kVK_ANSI_Keypad1 */
  90,  /* 84  kVK_ANSI_Keypad2 */
  91,  /* 85  kVK_ANSI_Keypad3 */
  92,  /* 86  kVK_ANSI_Keypad4 */
  93,  /* 87  kVK_ANSI_Keypad5 */
  94,  /* 88  kVK_ANSI_Keypad6 */
  95,  /* 89  kVK_ANSI_Keypad7 */
  111,  /* 90  kVK_F20 */
  96,  /* 91  kVK_ANSI_Keypad8 */
  97,  /* 92  kVK_ANSI_Keypad9 */
  0,  /* 93 */
  0,  /* 94 */
  0,  /* 95 */
  62,  /* 96  kVK_F5 */
  63,  /* 97  kVK_F6 */
  64,  /* 98  kVK_F7 */
  60,  /* 99  kVK_F3 */
  65,  /* 100  kVK_F8 */
  66,  /* 101  kVK_F9 */
  0,  /* 102 */
  68,  /* 103  kVK_F11 */
  0,  /* 104 */
  70,  /* 105  kVK_F13 */
  107,  /* 106  kVK_F16 */
  71,  /* 107  kVK_F14 */
  0,  /* 108 */
  67,  /* 109  kVK_F10 */
  101,  /* 110  SDL_SCANCODE_APPLICATION */
  69,  /* 111  kVK_F12 */
  0,  /* 112 */
  72,  /* 113  kVK_F15 */
  73,  /* 114  kVK_Help */
  74,  /* 115  kVK_Home */
  75,  /* 116  kVK_PageUp */
  76,  /* 117  kVK_ForwardDelete */
  61,  /* 118  kVK_F4 */
  77,  /* 119  kVK_End */
  59,  /* 120  kVK_F2 */
  78,  /* 121  kVK_PageDown */
  58,  /* 122  kVK_F1 */
  80,  /* 123  kVK_LeftArrow */
  79,  /* 124  kVK_RightArrow */
  81,  /* 125  kVK_DownArrow */
  82,  /* 126  kVK_UpArrow */
  0,  /* 127 */
  0,  /* 128 */
  0,  /* 129 */
  0,  /* 130 */
  0,  /* 131 */
  0,  /* 132 */
  0,  /* 133 */
  0,  /* 134 */
  0,  /* 135 */
  0,  /* 136 */
  0,  /* 137 */
  0,  /* 138 */
  0,  /* 139 */
  0,  /* 140 */
  0,  /* 141 */
  0,  /* 142 */
  0,  /* 143 */
  0,  /* 144 */
  0,  /* 145 */
  0,  /* 146 */
  0,  /* 147 */
  0,  /* 148 */
  0,  /* 149 */
  0,  /* 150 */
  0,  /* 151 */
  0,  /* 152 */
  0,  /* 153 */
  0,  /* 154 */
  0,  /* 155 */
  0,  /* 156 */
  0,  /* 157 */
  0,  /* 158 */
  0,  /* 159 */
  0,  /* 160 */
  0,  /* 161 */
  0,  /* 162 */
  0,  /* 163 */
  0,  /* 164 */
  0,  /* 165 */
  0,  /* 166 */
  0,  /* 167 */
  0,  /* 168 */
  0,  /* 169 */
  0,  /* 170 */
  0,  /* 171 */
  0,  /* 172 */
  0,  /* 173 */
  0,  /* 174 */
  0,  /* 175 */
  0,  /* 176 */
  0,  /* 177 */
  0,  /* 178 */
  0,  /* 179 */
  0,  /* 180 */
  0,  /* 181 */
  0,  /* 182 */
  0,  /* 183 */
  0,  /* 184 */
  0,  /* 185 */
  0,  /* 186 */
  0,  /* 187 */
  0,  /* 188 */
  0,  /* 189 */
  0,  /* 190 */
  0,  /* 191 */
  0,  /* 192 */
  0,  /* 193 */
  0,  /* 194 */
  0,  /* 195 */
  0,  /* 196 */
  0,  /* 197 */
  0,  /* 198 */
  0,  /* 199 */
  0,  /* 200 */
  0,  /* 201 */
  0,  /* 202 */
  0,  /* 203 */
  0,  /* 204 */
  0,  /* 205 */
  0,  /* 206 */
  0,  /* 207 */
  0,  /* 208 */
  0,  /* 209 */
  0,  /* 210 */
  0,  /* 211 */
  0,  /* 212 */
  0,  /* 213 */
  0,  /* 214 */
  0,  /* 215 */
  0,  /* 216 */
  0,  /* 217 */
  0,  /* 218 */
  0,  /* 219 */
  0,  /* 220 */
  0,  /* 221 */
  0,  /* 222 */
  0,  /* 223 */
  0,  /* 224 */
  0,  /* 225 */
  0,  /* 226 */
  0,  /* 227 */
  0,  /* 228 */
  0,  /* 229 */
  0,  /* 230 */
  0,  /* 231 */
  0,  /* 232 */
  0,  /* 233 */
  0,  /* 234 */
  0,  /* 235 */
  0,  /* 236 */
  0,  /* 237 */
  0,  /* 238 */
  0,  /* 239 */
  0,  /* 240 */
  0,  /* 241 */
  0,  /* 242 */
  0,  /* 243 */
  0   /* 244 */
};
#else
static const guint sdl_to_gdk[245] = {
  0,  /*    SDL_SCANCODE_UNKNOWN = 0, */
  0,  /*    SDL_SCANCODE_UNKNOWN = 1,*/
  0,  /*    SDL_SCANCODE_UNKNOWN = 2,*/
  0,  /*    SDL_SCANCODE_UNKNOWN = 3,*/
  38,  /*    SDL_SCANCODE_A = 4, */
  56,  /*    SDL_SCANCODE_B = 5, */
  54,  /*    SDL_SCANCODE_C = 6, */
  40,  /*    SDL_SCANCODE_D = 7, */
  26,  /*    SDL_SCANCODE_E = 8, */
  41,  /*    SDL_SCANCODE_F = 9, */
  42,  /*    SDL_SCANCODE_G = 10, */
  43,  /*    SDL_SCANCODE_H = 11, */
  31,  /*    SDL_SCANCODE_I = 12, */
  44,  /*    SDL_SCANCODE_J = 13, */
  45,  /*    SDL_SCANCODE_K = 14, */
  46,  /*    SDL_SCANCODE_L = 15, */
  58,  /*    SDL_SCANCODE_M = 16, */
  57,  /*    SDL_SCANCODE_N = 17, */
  32,  /*    SDL_SCANCODE_O = 18, */
  33,  /*    SDL_SCANCODE_P = 19, */
  24,  /*    SDL_SCANCODE_Q = 20, */
  27,  /*    SDL_SCANCODE_R = 21, */
  39,  /*    SDL_SCANCODE_S = 22, */
  28,  /*    SDL_SCANCODE_T = 23, */
  30,  /*    SDL_SCANCODE_U = 24, */
  55,  /*    SDL_SCANCODE_V = 25, */
  25,  /*    SDL_SCANCODE_W = 26, */
  53,  /*    SDL_SCANCODE_X = 27, */
  29,  /*    SDL_SCANCODE_Y = 28, */
  52,  /*    SDL_SCANCODE_Z = 29, */
  10,  /*    SDL_SCANCODE_1 = 30, */
  11,  /*    SDL_SCANCODE_2 = 31, */
  12,  /*    SDL_SCANCODE_3 = 32, */
  13,  /*    SDL_SCANCODE_4 = 33, */
  14,  /*    SDL_SCANCODE_5 = 34, */
  15,  /*    SDL_SCANCODE_6 = 35, */
  16,  /*    SDL_SCANCODE_7 = 36, */
  17,  /*    SDL_SCANCODE_8 = 37, */
  18,  /*    SDL_SCANCODE_9 = 38, */
  19,  /*    SDL_SCANCODE_0 = 39, */
  36,  /*    SDL_SCANCODE_RETURN = 40, */
  9,  /*    SDL_SCANCODE_ESCAPE = 41, */
  22,  /*    SDL_SCANCODE_BACKSPACE = 42, */
  23,  /*    SDL_SCANCODE_TAB = 43, */
  65,  /*    SDL_SCANCODE_SPACE = 44, */
  20,  /*    SDL_SCANCODE_MINUS = 45, */
  21,  /*    SDL_SCANCODE_EQUALS = 46, */
  34,  /*    SDL_SCANCODE_LEFTBRACKET = 47, */
  35,  /*    SDL_SCANCODE_RIGHTBRACKET = 48, */
  51,  /*    SDL_SCANCODE_BACKSLASH = 49, */
  0,  /*    SDL_SCANCODE_NONUSHASH = 50, */
  47,  /*    SDL_SCANCODE_SEMICOLON = 51, */
  48,  /*    SDL_SCANCODE_APOSTROPHE = 52, */
  49,  /*    SDL_SCANCODE_GRAVE = 53, */
  59,  /*    SDL_SCANCODE_COMMA = 54, */
  60,  /*    SDL_SCANCODE_PERIOD = 55, */
  61,  /*    SDL_SCANCODE_SLASH = 56, */
  66,  /*    SDL_SCANCODE_CAPSLOCK = 57, */
  67,  /*    SDL_SCANCODE_F1 = 58, */
  68,  /*    SDL_SCANCODE_F2 = 59, */
  69,  /*    SDL_SCANCODE_F3 = 60, */
  70,  /*    SDL_SCANCODE_F4 = 61, */
  71,  /*    SDL_SCANCODE_F5 = 62, */
  72,  /*    SDL_SCANCODE_F6 = 63, */
  73,  /*    SDL_SCANCODE_F7 = 64, */
  74,  /*    SDL_SCANCODE_F8 = 65, */
  75,  /*    SDL_SCANCODE_F9 = 66, */
  76,  /*    SDL_SCANCODE_F10 = 67, */
  95,  /*    SDL_SCANCODE_F11 = 68, */
  96,  /*    SDL_SCANCODE_F12 = 69, */
  107,  /*    SDL_SCANCODE_PRINTSCREEN = 70, */
  78,  /*    SDL_SCANCODE_SCROLLOCK = 71, */
  127,  /*    SDL_SCANCODE_PAUSE = 72, */
  118,  /*    SDL_SCANCODE_INSERT = 73, */
  110,  /*    SDL_SCANCODE_HOME = 74, */
  112,  /*    SDL_SCANCODE_PAGEUP = 75, */
  119,  /*    SDL_SCANCODE_DELETE = 76, */
  115,  /*   SDL_SCANCODE_END = 77, */
  117,  /*    SDL_SCANCODE_PAGEDOWN = 78, */
  114,  /*    SDL_SCANCODE_RIGHT = 79, */
  113,  /*    SDL_SCANCODE_LEFT = 80, */
  116,  /*    SDL_SCANCODE_DOWN = 81, */
  111,  /*    SDL_SCANCODE_UP = 82, */
  77,  /*    SDL_SCANCODE_NUMLOCKCLEAR = 83, */
  106,  /*    SDL_SCANCODE_KP_DIVIDE = 84, */
  63,  /*    SDL_SCANCODE_KP_MULTIPLY = 85, */
  82,  /*    SDL_SCANCODE_KP_MINUS = 86, */
  86,  /*    SDL_SCANCODE_KP_PLUS = 87, */
  104,  /*    SDL_SCANCODE_KP_ENTER = 88, */
  87,  /*    SDL_SCANCODE_KP_1 = 89, */
  88,  /*    SDL_SCANCODE_KP_2 = 90, */
  89,  /*    SDL_SCANCODE_KP_3 = 91, */
  83,  /*    SDL_SCANCODE_KP_4 = 92, */
  84,  /*    SDL_SCANCODE_KP_5 = 93, */
  85,  /*    SDL_SCANCODE_KP_6 = 94, */
  79,  /*    SDL_SCANCODE_KP_7 = 95, */
  80,  /*    SDL_SCANCODE_KP_8 = 96, */
  81,  /*    SDL_SCANCODE_KP_9 = 97, */
  90,  /*    SDL_SCANCODE_KP_0 = 98, */
  91,  /*    SDL_SCANCODE_KP_PERIOD = 99, */
  94,  /*    SDL_SCANCODE_NONUSBACKSLASH = 100, */
  135,  /*    SDL_SCANCODE_APPLICATION = 101, */
  124,  /*    SDL_SCANCODE_POWER = 102, */
  125,  /*   SDL_SCANCODE_KP_EQUALS = 103, */
  191,  /*    SDL_SCANCODE_F13 = 104, */
  192,  /*    SDL_SCANCODE_F14 = 105, */
  193,  /*    SDL_SCANCODE_F15 = 106, */
  194,  /*    SDL_SCANCODE_F16 = 107, */
  195,  /*    SDL_SCANCODE_F17 = 108, */
  196,  /*    SDL_SCANCODE_F18 = 109, */
  197,  /*    SDL_SCANCODE_F19 = 110, */
  198,  /*    SDL_SCANCODE_F20 = 111, */
  199,  /*    SDL_SCANCODE_F21 = 112, */
  200,  /*    SDL_SCANCODE_F22 = 113, */
  201,  /*    SDL_SCANCODE_F23 = 114, */
  202,  /*    SDL_SCANCODE_F24 = 115, */
  0,  /*    SDL_SCANCODE_EXECUTE = 116, */
  146,  /*    SDL_SCANCODE_HELP = 117, */
  147,  /*    SDL_SCANCODE_MENU = 118, */
  0,  /*   SDL_SCANCODE_SELECT = 119, */
  136,  /*    SDL_SCANCODE_STOP = 120, */
  137,  /*    SDL_SCANCODE_AGAIN = 121, */
  139,  /*    SDL_SCANCODE_UNDO = 122, */
  145,  /*    SDL_SCANCODE_CUT = 123, */
  141,  /*    SDL_SCANCODE_COPY = 124, */
  143,  /*    SDL_SCANCODE_PASTE = 125, */
  144,  /*    SDL_SCANCODE_FIND = 126, */
  121,  /*    SDL_SCANCODE_MUTE = 127, */
  122,  /*    SDL_SCANCODE_VOLUMEUP = 128, */
  123,  /*    SDL_SCANCODE_VOLUMEDOWN = 129, */
  0,  /*    SDL_SCANCODE_LOCKINGCAPSLOCK = 130,  */
  0,  /*    SDL_SCANCODE_LOCKINGNUMLOCK = 131, */
  0,  /*    SDL_SCANCODE_LOCKINGSCROLLLOCK = 132, */
  129,  /*    SDL_SCANCODE_KP_COMMA = 133, */
  0,  /*    SDL_SCANCODE_KP_EQUALSAS400 = 134, */
  97,  /*    SDL_SCANCODE_INTERNATIONAL1 = 135, */
  101,  /*    SDL_SCANCODE_INTERNATIONAL2 = 136, */
  132,  /*    SDL_SCANCODE_INTERNATIONAL3 = 137, */
  100,  /*    SDL_SCANCODE_INTERNATIONAL4 = 138, */
  102,  /*    SDL_SCANCODE_INTERNATIONAL5 = 139, */
  103,  /*    SDL_SCANCODE_INTERNATIONAL6 = 140, */
  0,  /*    SDL_SCANCODE_INTERNATIONAL7 = 141, */
  0,  /*    SDL_SCANCODE_INTERNATIONAL8 = 142, */
  0,  /*    SDL_SCANCODE_INTERNATIONAL9 = 143, */
  130,  /*    SDL_SCANCODE_LANG1 = 144, */
  131,  /*    SDL_SCANCODE_LANG2 = 145, */
  98,  /*    SDL_SCANCODE_LANG3 = 146, */
  99,  /*    SDL_SCANCODE_LANG4 = 147, */
  93,  /*    SDL_SCANCODE_LANG5 = 148, */
  0,  /*    SDL_SCANCODE_LANG6 = 149, */
  0,  /*    SDL_SCANCODE_LANG7 = 150, */
  0,  /*    SDL_SCANCODE_LANG8 = 151, */
  0,  /*    SDL_SCANCODE_LANG9 = 152, */
  230,  /*    SDL_SCANCODE_ALTERASE = 153, */
  107,  /*    SDL_SCANCODE_SYSREQ = 154, */
  231,  /*    SDL_SCANCODE_CANCEL = 155, */
  0,  /*    SDL_SCANCODE_CLEAR = 156, */
  0,  /*    SDL_SCANCODE_PRIOR = 157, */
  0,  /*    SDL_SCANCODE_RETURN2 = 158, */
  0,  /*    SDL_SCANCODE_SEPARATOR = 159, */
  0,  /*    SDL_SCANCODE_OUT = 160, */
  0,  /*    SDL_SCANCODE_OPER = 161, */
  0,  /*    SDL_SCANCODE_CLEARAGAIN = 162, */
  0,  /*    SDL_SCANCODE_CRSEL = 163, */
  0,  /*    SDL_SCANCODE_EXSEL = 164, */
  0,  /*    SDL_SCANCODE_UNKNOWN = 165,  */
  0,  /*    SDL_SCANCODE_UNKNOWN = 166,  */
  0,  /*    SDL_SCANCODE_UNKNOWN = 167,  */
  0,  /*    SDL_SCANCODE_UNKNOWN = 168,  */
  0,  /*    SDL_SCANCODE_UNKNOWN = 169,  */
  0,  /*    SDL_SCANCODE_UNKNOWN = 170,  */
  0,  /*    SDL_SCANCODE_UNKNOWN = 171,  */
  0,  /*    SDL_SCANCODE_UNKNOWN = 172,  */
  0,  /*    SDL_SCANCODE_UNKNOWN = 173,  */
  0,  /*    SDL_SCANCODE_UNKNOWN = 174,  */
  0,  /*    SDL_SCANCODE_UNKNOWN = 175,  */
  0,  /*    SDL_SCANCODE_KP_00 = 176, */
  0,  /*    SDL_SCANCODE_KP_000 = 177, */
  0,  /*    SDL_SCANCODE_THOUSANDSSEPARATOR = 178, */
  0,  /*    SDL_SCANCODE_DECIMALSEPARATOR = 179, */
  0,  /*    SDL_SCANCODE_CURRENCYUNIT = 180, */
  0,  /*    SDL_SCANCODE_CURRENCYSUBUNIT = 181, */
  0,  /*    SDL_SCANCODE_KP_LEFTPAREN = 182, */
  0,  /*    SDL_SCANCODE_KP_RIGHTPAREN = 183, */
  0,  /*    SDL_SCANCODE_KP_LEFTBRACE = 184, */
  0,  /*    SDL_SCANCODE_KP_RIGHTBRACE = 185, */
  0,  /*    SDL_SCANCODE_KP_TAB = 186, */
  0,  /*    SDL_SCANCODE_KP_BACKSPACE = 187, */
  0,  /*    SDL_SCANCODE_KP_A = 188, */
  0,  /*    SDL_SCANCODE_KP_B = 189, */
  0,  /*    SDL_SCANCODE_KP_C = 190, */
  0,  /*    SDL_SCANCODE_KP_D = 191, */
  0,  /*    SDL_SCANCODE_KP_E = 192, */
  0,  /*    SDL_SCANCODE_KP_F = 193, */
  0,  /*    SDL_SCANCODE_KP_XOR = 194, */
  0,  /*    SDL_SCANCODE_KP_POWER = 195, */
  0,  /*    SDL_SCANCODE_KP_PERCENT = 196, */
  0,  /*    SDL_SCANCODE_KP_LESS = 197, */
  0,  /*    SDL_SCANCODE_KP_GREATER = 198, */
  0,  /*    SDL_SCANCODE_KP_AMPERSAND = 199, */
  0,  /*    SDL_SCANCODE_KP_DBLAMPERSAND = 200, */
  0,  /*    SDL_SCANCODE_KP_VERTICALBAR = 201, */
  0,  /*    SDL_SCANCODE_KP_DBLVERTICALBAR = 202, */
  0,  /*    SDL_SCANCODE_KP_COLON = 203, */
  0,  /*    SDL_SCANCODE_KP_HASH = 204, */
  0,  /*    SDL_SCANCODE_KP_SPACE = 205, */
  0,  /*    SDL_SCANCODE_KP_AT = 206, */
  0,  /*    SDL_SCANCODE_KP_EXCLAM = 207, */
  0,  /*    SDL_SCANCODE_KP_MEMSTORE = 208, */
  0,  /*    SDL_SCANCODE_KP_MEMRECALL = 209, */
  0,  /*    SDL_SCANCODE_KP_MEMCLEAR = 210, */
  0,  /*    SDL_SCANCODE_KP_MEMADD = 211, */
  0,  /*    SDL_SCANCODE_KP_MEMSUBTRACT = 212, */
  0,  /*    SDL_SCANCODE_KP_MEMMULTIPLY = 213, */
  0,  /*    SDL_SCANCODE_KP_MEMDIVIDE = 214, */
  0,  /*    SDL_SCANCODE_KP_PLUSMINUS = 215, */
  0,  /*    SDL_SCANCODE_KP_CLEAR = 216, */
  0,  /*    SDL_SCANCODE_KP_CLEARENTRY = 217, */
  0,  /*    SDL_SCANCODE_KP_BINARY = 218, */
  0,  /*    SDL_SCANCODE_KP_OCTAL = 219, */
  0,  /*    SDL_SCANCODE_KP_DECIMAL = 220, */
  0,  /*    SDL_SCANCODE_KP_HEXADECIMAL = 221, */
  0,  /*    SDL_SCANCODE_UNKNOWN = 222, */
  0,  /*    SDL_SCANCODE_UNKNOWN = 223,  */
  37,  /*    SDL_SCANCODE_LCTRL = 224, */
  50,  /*    SDL_SCANCODE_LSHIFT = 225, */
  64,  /*    SDL_SCANCODE_LALT = 226, */
  133,  /*    SDL_SCANCODE_LGUI = 227, */
  105,  /*    SDL_SCANCODE_RCTRL = 228, */
  62,  /*    SDL_SCANCODE_RSHIFT = 229, */
  108,  /*    SDL_SCANCODE_RALT = 230, */
  134,  /*    SDL_SCANCODE_RGUI = 231, */
  0,  /*    SDL_SCANCODE_UNKNOWN = 232,  */
  0,  /*    SDL_SCANCODE_UNKNOWN = 233,  */
  0,  /*    SDL_SCANCODE_UNKNOWN = 234,  */
  0,  /*    SDL_SCANCODE_UNKNOWN = 235,  */
  0,  /*    SDL_SCANCODE_UNKNOWN = 236,  */
  0,  /*    SDL_SCANCODE_UNKNOWN = 237,  */
  0,  /*    SDL_SCANCODE_UNKNOWN = 238,  */
  0,  /*    SDL_SCANCODE_UNKNOWN = 239,  */
  0,  /*    SDL_SCANCODE_UNKNOWN = 240,  */
  0,  /*    SDL_SCANCODE_UNKNOWN = 241,  */
  0,  /*    SDL_SCANCODE_UNKNOWN = 242,  */
  0,  /*    SDL_SCANCODE_UNKNOWN = 243,  */
  0
};

static const guint gdk_to_sdl[245] = {
  0,  /*  0    SDL_SCANCODE_UNKNOWN, */
  0,  /*  1    SDL_SCANCODE_UNKNOWN, */
  0,  /*  2    SDL_SCANCODE_UNKNOWN, */
  0,  /*  3    SDL_SCANCODE_UNKNOWN, */
  0,  /*  4    SDL_SCANCODE_UNKNOWN, */
  0,  /*  5    SDL_SCANCODE_UNKNOWN, */
  0,  /*  6    SDL_SCANCODE_UNKNOWN, */
  0,  /*  7    SDL_SCANCODE_UNKNOWN, */
  0,  /*  8    SDL_SCANCODE_UNKNOWN, */
  41,  /*  9    SDL_SCANCODE_ESCAPE, */
  30,  /*  10    SDL_SCANCODE_1, */
  31,  /*  11    SDL_SCANCODE_2, */
  32,  /*  12    SDL_SCANCODE_3, */
  33,  /*  13    SDL_SCANCODE_4, */
  34,  /*  14    SDL_SCANCODE_5, */
  35,  /*  15    SDL_SCANCODE_6, */
  36,  /*  16    SDL_SCANCODE_7, */
  37,  /*  17    SDL_SCANCODE_8, */
  38,  /*  18    SDL_SCANCODE_9, */
  39,  /*  19    SDL_SCANCODE_0, */
  45,  /*  20    SDL_SCANCODE_MINUS, */
  46,  /*  21    SDL_SCANCODE_EQUALS, */
  42,  /*  22    SDL_SCANCODE_BACKSPACE, */
  43,  /*  23    SDL_SCANCODE_TAB, */
  20,  /*  24    SDL_SCANCODE_Q, */
  26,  /*  25    SDL_SCANCODE_W, */
  8,  /*  26    SDL_SCANCODE_E, */
  21,  /*  27    SDL_SCANCODE_R, */
  23,  /*  28    SDL_SCANCODE_T, */
  28,  /*  29    SDL_SCANCODE_Y, */
  24,  /*  30    SDL_SCANCODE_U, */
  12,  /*  31    SDL_SCANCODE_I, */
  18,  /*  32    SDL_SCANCODE_O, */
  19,  /*  33    SDL_SCANCODE_P, */
  47,  /*  34    SDL_SCANCODE_LEFTBRACKET, */
  48,  /*  35    SDL_SCANCODE_RIGHTBRACKET, */
  40,  /*  36    SDL_SCANCODE_RETURN, */
  224,  /*  37    SDL_SCANCODE_LCTRL, */
  4,  /*  38    SDL_SCANCODE_A, */
  22,  /*  39    SDL_SCANCODE_S, */
  7,  /*  40    SDL_SCANCODE_D, */
  9,  /*  41    SDL_SCANCODE_F, */
  10,  /*  42    SDL_SCANCODE_G, */
  11,  /*  43    SDL_SCANCODE_H, */
  13,  /*  44    SDL_SCANCODE_J, */
  14,  /*  45    SDL_SCANCODE_K, */
  15,  /*  46    SDL_SCANCODE_L, */
  51,  /*  47    SDL_SCANCODE_SEMICOLON, */
  52,  /*  48    SDL_SCANCODE_APOSTROPHE, */
  53,  /*  49    SDL_SCANCODE_GRAVE, */
  225,  /*  50    SDL_SCANCODE_LSHIFT, */
  49,  /*  51    SDL_SCANCODE_BACKSLASH, */
  29,  /*  52    SDL_SCANCODE_Z, */
  27,  /*  53    SDL_SCANCODE_X, */
  6,  /*  54    SDL_SCANCODE_C, */
  25,  /*  55    SDL_SCANCODE_V, */
  5,  /*  56    SDL_SCANCODE_B, */
  17,  /*  57    SDL_SCANCODE_N, */
  16,  /*  58    SDL_SCANCODE_M, */
  54,  /*  59    SDL_SCANCODE_COMMA, */
  55,  /*  60    SDL_SCANCODE_PERIOD, */
  56,  /*  61    SDL_SCANCODE_SLASH, */
  229,  /*  62    SDL_SCANCODE_RSHIFT, */
  85,  /*  63    SDL_SCANCODE_KP_MULTIPLY, */
  226,  /*  64    SDL_SCANCODE_LALT, */
  44,  /*  65    SDL_SCANCODE_SPACE, */
  57,  /*  66    SDL_SCANCODE_CAPSLOCK, */
  58,  /*  67    SDL_SCANCODE_F1, */
  59,  /*  68    SDL_SCANCODE_F2, */
  60,  /*  69    SDL_SCANCODE_F3, */
  61,  /*  70    SDL_SCANCODE_F4, */
  62,  /*  71    SDL_SCANCODE_F5, */
  63,  /*  72    SDL_SCANCODE_F6, */
  64,  /*  73    SDL_SCANCODE_F7, */
  65,  /*  74    SDL_SCANCODE_F8, */
  66,  /*  75    SDL_SCANCODE_F9, */
  67,  /*  76    SDL_SCANCODE_F10, */
  83,  /*  77    SDL_SCANCODE_NUMLOCKCLEAR, */
  71,  /*  78    SDL_SCANCODE_SCROLLLOCK, */
  95,  /*  79    SDL_SCANCODE_KP_7, */
  96,  /*  80    SDL_SCANCODE_KP_8, */
  97,  /*  81    SDL_SCANCODE_KP_9, */
  86,  /*  82    SDL_SCANCODE_KP_MINUS, */
  92,  /*  83    SDL_SCANCODE_KP_4, */
  93,  /*  84    SDL_SCANCODE_KP_5, */
  94,  /*  85    SDL_SCANCODE_KP_6, */
  87,  /*  86    SDL_SCANCODE_KP_PLUS, */
  89,  /*  87    SDL_SCANCODE_KP_1, */
  90,  /*  88    SDL_SCANCODE_KP_2, */
  91,  /*  89    SDL_SCANCODE_KP_3, */
  98,  /*  90    SDL_SCANCODE_KP_0, */
  99,  /*  91    SDL_SCANCODE_KP_PERIOD, */
  0,   /*  92    SDL_SCANCODE_UNUSED, */
  148,  /*  93    SDL_SCANCODE_LANG5, KEY_ZENKAKUHANKAKU */
  100,  /*  94    SDL_SCANCODE_NONUSBACKSLASH, KEY_102ND */
  68,  /*  95    SDL_SCANCODE_F11, */
  69,  /*  96    SDL_SCANCODE_F12, */
  135,  /*  97    SDL_SCANCODE_INTERNATIONAL1, KEY_RO */
  146,  /*  98    SDL_SCANCODE_LANG3, KEY_KATAKANA */
  147,  /*  99    SDL_SCANCODE_LANG4, KEY_HIRAGANA */
  138,  /*  100    SDL_SCANCODE_INTERNATIONAL4, KEY_HENKAN */
  136,  /*  101    SDL_SCANCODE_INTERNATIONAL2, KEY_KATAKANAHIRAGANA */
  139,  /*  102    SDL_SCANCODE_INTERNATIONAL5, KEY_MUHENKAN */
  140,  /*  103    SDL_SCANCODE_INTERNATIONAL6, KEY_KPJPCOMMA */
  88,  /*  104    SDL_SCANCODE_KP_ENTER, */
  228,  /*  105    SDL_SCANCODE_RCTRL, */
  84,  /*  106    SDL_SCANCODE_KP_DIVIDE, */
  70,  /*  107    SDL_SCANCODE_SYSREQ, */
  230,  /*  108    SDL_SCANCODE_RALT, */
  0,  /*  109    SDL_SCANCODE_UNKNOWN, KEY_LINEFEED */
  74,  /*  110    SDL_SCANCODE_HOME, */
  82,  /*  111    SDL_SCANCODE_UP, */
  75,  /*  112    SDL_SCANCODE_PAGEUP, */
  80,  /*  113    SDL_SCANCODE_LEFT, */
  79,  /*  114    SDL_SCANCODE_RIGHT, */
  77,  /*  115    SDL_SCANCODE_END, */
  81,  /*  116    SDL_SCANCODE_DOWN, */
  78,  /*  117    SDL_SCANCODE_PAGEDOWN, */
  73,  /*  118    SDL_SCANCODE_INSERT, */
  76,  /*  119    SDL_SCANCODE_DELETE, */
  0,  /*  120    SDL_SCANCODE_UNKNOWN, KEY_MACRO */
  127,  /*  121    SDL_SCANCODE_MUTE, */
  129,  /*  122    SDL_SCANCODE_VOLUMEDOWN, */
  128,  /*  123    SDL_SCANCODE_VOLUMEUP, */
  102,  /*  124    SDL_SCANCODE_POWER, */
  103,  /*  125    SDL_SCANCODE_KP_EQUALS, */
  215,  /*  126    SDL_SCANCODE_KP_PLUSMINUS, */
  72,  /*  127    SDL_SCANCODE_PAUSE, */
  0,  /*  128    SDL_SCANCODE_UNUSED, */
  133,  /*  129    SDL_SCANCODE_KP_COMMA, */
  144,  /*  130    SDL_SCANCODE_LANG1, KEY_HANGUEL */
  145,  /*  131    SDL_SCANCODE_LANG2, KEY_HANJA */
  137,  /*  132    SDL_SCANCODE_INTERNATIONAL3, KEY_YEN */
  227,  /*  133    SDL_SCANCODE_LGUI, */
  231,  /*  134    SDL_SCANCODE_RGUI, */
  101,  /*  135    SDL_SCANCODE_APPLICATION, KEY_COMPOSE */
  120,  /*  136    SDL_SCANCODE_STOP, */
  121,  /*  137    SDL_SCANCODE_AGAIN, */
  0,  /*  138    SDL_SCANCODE_UNKNOWN, KEY_PROPS */
  122,  /*  139    SDL_SCANCODE_UNDO, */
  0,  /*  140    SDL_SCANCODE_UNKNOWN, KEY_FRONT */
  124,  /*  141    SDL_SCANCODE_COPY, */
  0,  /*  142    SDL_SCANCODE_UNKNOWN, KEY_OPEN */
  125,  /*  143    SDL_SCANCODE_PASTE, */
  126,  /*  144    SDL_SCANCODE_FIND, */
  123,  /*  145    SDL_SCANCODE_CUT, */
  117,  /*  146    SDL_SCANCODE_HELP, */
  118,  /*  147    SDL_SCANCODE_MENU, */
  0,  /*  148    SDL_SCANCODE_CALCULATOR, */
  0,  /*  149    SDL_SCANCODE_UNKNOWN, KEY_SETUP */
  0,  /*  150    SDL_SCANCODE_SLEEP, */
  0,    /*  151    SDL_SCANCODE_UNKNOWN, KEY_WAKEUP */
  0,    /*  152    SDL_SCANCODE_UNKNOWN, KEY_FILE */
  0,    /*  153    SDL_SCANCODE_UNKNOWN, KEY_SENDFILE */
  0,    /*  154    SDL_SCANCODE_UNKNOWN, KEY_DELETEFILE */
  0,    /*  155    SDL_SCANCODE_UNKNOWN, KEY_XFER */
  0,    /*  156    SDL_SCANCODE_APP1, KEY_PROG1 */
  0,    /*  157    SDL_SCANCODE_APP2, KEY_PROG2 */
  0,    /*  158    SDL_SCANCODE_WWW, KEY_WWW */
  0,    /*  159    SDL_SCANCODE_UNKNOWN, KEY_MSDOS */
  0,    /*  160    SDL_SCANCODE_UNKNOWN, KEY_COFFEE */
  0,    /*  161    SDL_SCANCODE_UNKNOWN, KEY_DIRECTION */
  0,    /*  162    SDL_SCANCODE_UNKNOWN, KEY_CYCLEWINDOWS */
  0,    /*  163    SDL_SCANCODE_MAIL, */
  0,    /*  164    SDL_SCANCODE_AC_BOOKMARKS, */
  0,    /*  165    SDL_SCANCODE_COMPUTER, */
  0,    /*  166    SDL_SCANCODE_AC_BACK, */
  0,    /*  167    SDL_SCANCODE_AC_FORWARD, */
  0,    /*  168    SDL_SCANCODE_UNKNOWN, KEY_CLOSECD */
  0,    /*  169    SDL_SCANCODE_EJECT, KEY_EJECTCD */
  0,    /*  170    SDL_SCANCODE_UNKNOWN, KEY_EJECTCLOSECD */
  0,    /*  171    SDL_SCANCODE_AUDIONEXT, KEY_NEXTSONG */
  0,    /*  172    SDL_SCANCODE_AUDIOPLAY, KEY_PLAYPAUSE */
  0,    /*  173    SDL_SCANCODE_AUDIOPREV, KEY_PREVIOUSSONG */
  0,    /*  174    SDL_SCANCODE_AUDIOSTOP, KEY_STOPCD */
  0,    /*  175    SDL_SCANCODE_UNKNOWN, KEY_RECORD */
  0,    /*  176    SDL_SCANCODE_AUDIOREWIND, KEY_REWIND */
  0,    /*  177    SDL_SCANCODE_UNKNOWN, KEY_PHONE */
  0,    /*  178    SDL_SCANCODE_UNKNOWN, KEY_ISO */
  0,    /*  179    SDL_SCANCODE_UNKNOWN, KEY_CONFIG */
  0,    /*  180    SDL_SCANCODE_AC_HOME, */
  0,    /*  181    SDL_SCANCODE_AC_REFRESH, */
  0,    /*  182    SDL_SCANCODE_UNKNOWN, KEY_EXIT */
  0,    /*  183    SDL_SCANCODE_UNKNOWN, KEY_MOVE */
  0,    /*  184    SDL_SCANCODE_UNKNOWN, KEY_EDIT */
  0,    /*  185    SDL_SCANCODE_UNKNOWN, KEY_SCROLLUP */
  0,    /*  186    SDL_SCANCODE_UNKNOWN, KEY_SCROLLDOWN */
  0,    /*  187    SDL_SCANCODE_KP_LEFTPAREN, */
  0,    /*  188    SDL_SCANCODE_KP_RIGHTPAREN, */
  0,    /*  189    SDL_SCANCODE_UNKNOWN, KEY_NEW */
  0,    /*  190    SDL_SCANCODE_UNKNOWN, KEY_REDO */
  104,  /*  191    SDL_SCANCODE_F13, */
  105,  /*  192    SDL_SCANCODE_F14, */
  106,  /*  193    SDL_SCANCODE_F15, */
  107,  /*  194    SDL_SCANCODE_F16, */
  108,  /*  195    SDL_SCANCODE_F17, */
  109,  /*  196    SDL_SCANCODE_F18, */
  110,  /*  197    SDL_SCANCODE_F19, */
  111,  /*  198    SDL_SCANCODE_F20, */
  112,  /*  199    SDL_SCANCODE_F21, */
  113,  /*  200    SDL_SCANCODE_F22, */
  114,  /*  201    SDL_SCANCODE_F23, */
  115,  /*  202    SDL_SCANCODE_F24, */
  0,    /*  203    SDL_SCANCODE_UNKNOWN, */
  0,    /*  204    SDL_SCANCODE_UNKNOWN, */
  0,    /*  205    SDL_SCANCODE_UNKNOWN, */
  0,    /*  206    SDL_SCANCODE_UNKNOWN, */
  0,    /*  207    SDL_SCANCODE_UNKNOWN, */
  0,    /*  208    SDL_SCANCODE_UNKNOWN, KEY_PLAYCD */
  0,    /*  209    SDL_SCANCODE_UNKNOWN, KEY_PAUSECD */
  0,    /*  210    SDL_SCANCODE_UNKNOWN, KEY_PROG3 */
  0,    /*  211    SDL_SCANCODE_UNKNOWN, KEY_PROG4 */
  0,    /*  212    SDL_SCANCODE_UNKNOWN, */
  0,    /*  213    SDL_SCANCODE_UNKNOWN, KEY_SUSPEND */
  0,    /*  214    SDL_SCANCODE_UNKNOWN, KEY_CLOSE */
  0,    /*  215    SDL_SCANCODE_UNKNOWN, KEY_PLAY */
  0,    /*  216    SDL_SCANCODE_AUDIOFASTFORWARD, KEY_FASTFORWARD */
  0,    /*  217    SDL_SCANCODE_UNKNOWN, KEY_BASSBOOST */
  0,    /*  218    SDL_SCANCODE_UNKNOWN, KEY_PRINT */
  0,    /*  219    SDL_SCANCODE_UNKNOWN, KEY_HP */
  0,    /*  220    SDL_SCANCODE_UNKNOWN, KEY_CAMERA */
  0,    /*  221    SDL_SCANCODE_UNKNOWN, KEY_SOUND */
  0,    /*  222    SDL_SCANCODE_UNKNOWN, KEY_QUESTION */
  0,    /*  223    SDL_SCANCODE_UNKNOWN, KEY_EMAIL */
  0,    /*  224    SDL_SCANCODE_UNKNOWN, KEY_CHAT */
  0,    /*  225    SDL_SCANCODE_AC_SEARCH, */
  0,    /*  226    SDL_SCANCODE_UNKNOWN, KEY_CONNECT */
  0,    /*  227    SDL_SCANCODE_UNKNOWN, KEY_FINANCE */
  0,    /*  228    SDL_SCANCODE_UNKNOWN, KEY_SPORT */
  0,    /*  229    SDL_SCANCODE_UNKNOWN, KEY_SHOP */
  153,    /*  230    SDL_SCANCODE_ALTERASE, */
  155,    /*  231    SDL_SCANCODE_CANCEL, */
  0,    /*  232    SDL_SCANCODE_BRIGHTNESSDOWN, */
  0,    /*  233    SDL_SCANCODE_BRIGHTNESSUP, */
  0,    /*  234    SDL_SCANCODE_UNKNOWN, KEY_MEDIA */
  0,    /*  235    SDL_SCANCODE_DISPLAYSWITCH, KEY_SWITCHVIDEOMODE */
  0,    /*  236    SDL_SCANCODE_KBDILLUMTOGGLE, */
  0,    /*  237    SDL_SCANCODE_KBDILLUMDOWN, */
  0,    /*  238    SDL_SCANCODE_KBDILLUMUP, */
  0,    /*  239    SDL_SCANCODE_UNKNOWN, KEY_SEND */
  0,    /*  240    SDL_SCANCODE_UNKNOWN, KEY_REPLY */
  0,    /*  241    SDL_SCANCODE_UNKNOWN, KEY_FORWARDMAIL */
  0,    /*  242    SDL_SCANCODE_UNKNOWN, KEY_SAVE */
  0,    /*  243    SDL_SCANCODE_UNKNOWN, KEY_DOCUMENTS */
  0    /*  244    SDL_SCANCODE_UNKNOWN,  KEY_BATTERY */
};
#endif
#endif

#ifdef G_OS_WIN32
static const guint sdl_to_gdk[245] = {
  0,   /*    SDL_SCANCODE_UNKNOWN = 0, */
  0,   /*    SDL_SCANCODE_UNKNOWN = 1,*/
  0,   /*    SDL_SCANCODE_UNKNOWN = 2,*/
  0,   /*    SDL_SCANCODE_UNKNOWN = 3,*/
  65,  /*    SDL_SCANCODE_A = 4, */
  66,  /*    SDL_SCANCODE_B = 5, */
  67,  /*    SDL_SCANCODE_C = 6, */
  68,  /*    SDL_SCANCODE_D = 7, */
  69,  /*    SDL_SCANCODE_E = 8, */
  70,  /*    SDL_SCANCODE_F = 9, */
  71,  /*    SDL_SCANCODE_G = 10, */
  72,  /*    SDL_SCANCODE_H = 11, */
  73,  /*    SDL_SCANCODE_I = 12, */
  74,  /*    SDL_SCANCODE_J = 13, */
  75,  /*    SDL_SCANCODE_K = 14, */
  76,  /*    SDL_SCANCODE_L = 15, */
  77,  /*    SDL_SCANCODE_M = 16, */
  78,  /*    SDL_SCANCODE_N = 17, */
  79,  /*    SDL_SCANCODE_O = 18, */
  80,  /*    SDL_SCANCODE_P = 19, */
  81,  /*    SDL_SCANCODE_Q = 20, */
  82,  /*    SDL_SCANCODE_R = 21, */
  83,  /*    SDL_SCANCODE_S = 22, */
  84,  /*    SDL_SCANCODE_T = 23, */
  85,  /*    SDL_SCANCODE_U = 24, */
  86,  /*    SDL_SCANCODE_V = 25, */
  87,  /*    SDL_SCANCODE_W = 26, */
  88,  /*    SDL_SCANCODE_X = 27, */
  89,  /*    SDL_SCANCODE_Y = 28, */
  90,  /*    SDL_SCANCODE_Z = 29, */
  49,  /*    SDL_SCANCODE_1 = 30, */
  50,  /*    SDL_SCANCODE_2 = 31, */
  51,  /*    SDL_SCANCODE_3 = 32, */
  52,  /*    SDL_SCANCODE_4 = 33, */
  53,  /*    SDL_SCANCODE_5 = 34, */
  54,  /*    SDL_SCANCODE_6 = 35, */
  55,  /*    SDL_SCANCODE_7 = 36, */
  56,  /*    SDL_SCANCODE_8 = 37, */
  57,  /*    SDL_SCANCODE_9 = 38, */
  48,  /*    SDL_SCANCODE_0 = 39, */
  13,  /*    SDL_SCANCODE_RETURN = 40, */
  27,  /*    SDL_SCANCODE_ESCAPE = 41, */
  8,   /*    SDL_SCANCODE_BACKSPACE = 42, */
  9,   /*    SDL_SCANCODE_TAB = 43, */
  32,  /*    SDL_SCANCODE_SPACE = 44, */
  189, /*    SDL_SCANCODE_MINUS = 45, */
  187, /*    SDL_SCANCODE_EQUALS = 46, */
  219, /*    SDL_SCANCODE_LEFTBRACKET = 47, */
  221, /*    SDL_SCANCODE_RIGHTBRACKET = 48, */
  220, /*    SDL_SCANCODE_BACKSLASH = 49, */
  0,   /*    SDL_SCANCODE_NONUSHASH = 50, */
  186, /*    SDL_SCANCODE_SEMICOLON = 51, */
  222, /*    SDL_SCANCODE_APOSTROPHE = 52, */
  192, /*    SDL_SCANCODE_GRAVE = 53, */
  188, /*    SDL_SCANCODE_COMMA = 54, */
  190, /*    SDL_SCANCODE_PERIOD = 55, */
  191, /*    SDL_SCANCODE_SLASH = 56, */
  20,  /*    SDL_SCANCODE_CAPSLOCK = 57, */
  112, /*    SDL_SCANCODE_F1 = 58, */
  113, /*    SDL_SCANCODE_F2 = 59, */
  114, /*    SDL_SCANCODE_F3 = 60, */
  115, /*    SDL_SCANCODE_F4 = 61, */
  116, /*    SDL_SCANCODE_F5 = 62, */
  117, /*    SDL_SCANCODE_F6 = 63, */
  118, /*    SDL_SCANCODE_F7 = 64, */
  119, /*    SDL_SCANCODE_F8 = 65, */
  120, /*    SDL_SCANCODE_F9 = 66, */
  121, /*    SDL_SCANCODE_F10 = 67, */
  122, /*    SDL_SCANCODE_F11 = 68, */
  123, /*    SDL_SCANCODE_F12 = 69, */
  44,  /*    SDL_SCANCODE_PRINTSCREEN = 70, */
  145, /*    SDL_SCANCODE_SCROLLOCK = 71, */
  19,  /*    SDL_SCANCODE_PAUSE = 72, */
  45,  /*    SDL_SCANCODE_INSERT = 73, */
  36,  /*    SDL_SCANCODE_HOME = 74, */
  33,  /*    SDL_SCANCODE_PAGEUP = 75, */
  46,  /*    SDL_SCANCODE_DELETE = 76, */
  35,  /*   SDL_SCANCODE_END = 77, */
  34,  /*    SDL_SCANCODE_PAGEDOWN = 78, */
  39,  /*    SDL_SCANCODE_RIGHT = 79, */
  37,  /*    SDL_SCANCODE_LEFT = 80, */
  40,  /*    SDL_SCANCODE_DOWN = 81, */
  38,  /*    SDL_SCANCODE_UP = 82, */
  144, /*    SDL_SCANCODE_NUMLOCKCLEAR = 83, */
  111, /*    SDL_SCANCODE_KP_DIVIDE = 84, */
  106, /*    SDL_SCANCODE_KP_MULTIPLY = 85, */
  109, /*    SDL_SCANCODE_KP_MINUS = 86, */
  107, /*    SDL_SCANCODE_KP_PLUS = 87, */
  0,   /*    SDL_SCANCODE_KP_ENTER = 88, */
  97,  /*    SDL_SCANCODE_KP_1 = 89, */
  98,  /*    SDL_SCANCODE_KP_2 = 90, */
  99,  /*    SDL_SCANCODE_KP_3 = 91, */
  100, /*    SDL_SCANCODE_KP_4 = 92, */
  101, /*    SDL_SCANCODE_KP_5 = 93, */
  102, /*    SDL_SCANCODE_KP_6 = 94, */
  103, /*    SDL_SCANCODE_KP_7 = 95, */
  104, /*    SDL_SCANCODE_KP_8 = 96, */
  105, /*    SDL_SCANCODE_KP_9 = 97, */
  96,  /*    SDL_SCANCODE_KP_0 = 98, */
  110, /*    SDL_SCANCODE_KP_PERIOD = 99, */
  226, /*    SDL_SCANCODE_NONUSBACKSLASH = 100, */
  93,  /*    SDL_SCANCODE_APPLICATION = 101, */
  0,   /*    SDL_SCANCODE_POWER = 102, */
  0,   /*   SDL_SCANCODE_KP_EQUALS = 103, */
  124, /*    SDL_SCANCODE_F13 = 104, */
  125, /*    SDL_SCANCODE_F14 = 105, */
  126, /*    SDL_SCANCODE_F15 = 106, */
  127, /*    SDL_SCANCODE_F16 = 107, */
  128, /*    SDL_SCANCODE_F17 = 108, */
  129, /*    SDL_SCANCODE_F18 = 109, */
  130, /*    SDL_SCANCODE_F19 = 110, */
  131, /*    SDL_SCANCODE_F20 = 111, */
  132, /*    SDL_SCANCODE_F21 = 112, */
  133, /*    SDL_SCANCODE_F22 = 113, */
  134, /*    SDL_SCANCODE_F23 = 114, */
  135, /*    SDL_SCANCODE_F24 = 115, */
  0,   /*    SDL_SCANCODE_EXECUTE = 116, */
  0,   /*    SDL_SCANCODE_HELP = 117, */
  0,   /*    SDL_SCANCODE_MENU = 118, */
  0,   /*    SDL_SCANCODE_SELECT = 119, */
  0,   /*    SDL_SCANCODE_STOP = 120, */
  0,   /*    SDL_SCANCODE_AGAIN = 121, */
  0,   /*    SDL_SCANCODE_UNDO = 122, */
  0,   /*    SDL_SCANCODE_CUT = 123, */
  0,   /*    SDL_SCANCODE_COPY = 124, */
  0,   /*    SDL_SCANCODE_PASTE = 125, */
  0,   /*    SDL_SCANCODE_FIND = 126, */
  173, /*    SDL_SCANCODE_MUTE = 127, */
  175, /*    SDL_SCANCODE_VOLUMEUP = 128, */
  174, /*    SDL_SCANCODE_VOLUMEDOWN = 129, */
  0,   /*    SDL_SCANCODE_LOCKINGCAPSLOCK = 130,  */
  0,   /*    SDL_SCANCODE_LOCKINGNUMLOCK = 131, */
  0,   /*    SDL_SCANCODE_LOCKINGSCROLLLOCK = 132, */
  0,   /*    SDL_SCANCODE_KP_COMMA = 133, */
  0,   /*    SDL_SCANCODE_KP_EQUALSAS400 = 134, */
  0,   /*    SDL_SCANCODE_INTERNATIONAL1 = 135, */
  0,   /*    SDL_SCANCODE_INTERNATIONAL2 = 136, */
  0,   /*    SDL_SCANCODE_INTERNATIONAL3 = 137, */
  0,   /*    SDL_SCANCODE_INTERNATIONAL4 = 138, */
  0,   /*    SDL_SCANCODE_INTERNATIONAL5 = 139, */
  0,   /*    SDL_SCANCODE_INTERNATIONAL6 = 140, */
  0,   /*    SDL_SCANCODE_INTERNATIONAL7 = 141, */
  0,   /*    SDL_SCANCODE_INTERNATIONAL8 = 142, */
  0,   /*    SDL_SCANCODE_INTERNATIONAL9 = 143, */
  0,   /*    SDL_SCANCODE_LANG1 = 144, */
  0,   /*    SDL_SCANCODE_LANG2 = 145, */
  0,   /*    SDL_SCANCODE_LANG3 = 146, */
  0,   /*    SDL_SCANCODE_LANG4 = 147, */
  0,   /*    SDL_SCANCODE_LANG5 = 148, */
  0,   /*    SDL_SCANCODE_LANG6 = 149, */
  0,   /*    SDL_SCANCODE_LANG7 = 150, */
  0,   /*    SDL_SCANCODE_LANG8 = 151, */
  0,   /*    SDL_SCANCODE_LANG9 = 152, */
  0,   /*    SDL_SCANCODE_ALTERASE = 153, */
  0,   /*    SDL_SCANCODE_SYSREQ = 154, */
  0,   /*    SDL_SCANCODE_CANCEL = 155, */
  12,  /*    SDL_SCANCODE_CLEAR = 156, */
  0,   /*    SDL_SCANCODE_PRIOR = 157, */
  0,   /*    SDL_SCANCODE_RETURN2 = 158, */
  0,   /*    SDL_SCANCODE_SEPARATOR = 159, */
  0,   /*    SDL_SCANCODE_OUT = 160, */
  0,   /*    SDL_SCANCODE_OPER = 161, */
  0,   /*    SDL_SCANCODE_CLEARAGAIN = 162, */
  0,   /*    SDL_SCANCODE_CRSEL = 163, */
  0,   /*    SDL_SCANCODE_EXSEL = 164, */
  0,   /*    SDL_SCANCODE_UNKNOWN = 165,  */
  0,   /*    SDL_SCANCODE_UNKNOWN = 166,  */
  0,   /*    SDL_SCANCODE_UNKNOWN = 167,  */
  0,   /*    SDL_SCANCODE_UNKNOWN = 168,  */
  0,   /*    SDL_SCANCODE_UNKNOWN = 169,  */
  0,   /*    SDL_SCANCODE_UNKNOWN = 170,  */
  0,   /*    SDL_SCANCODE_UNKNOWN = 171,  */
  0,   /*    SDL_SCANCODE_UNKNOWN = 172,  */
  0,   /*    SDL_SCANCODE_UNKNOWN = 173,  */
  0,   /*    SDL_SCANCODE_UNKNOWN = 174,  */
  0,   /*    SDL_SCANCODE_UNKNOWN = 175,  */
  0,   /*    SDL_SCANCODE_KP_00 = 176, */
  0,   /*    SDL_SCANCODE_KP_000 = 177, */
  0,   /*    SDL_SCANCODE_THOUSANDSSEPARATOR = 178, */
  0,   /*    SDL_SCANCODE_DECIMALSEPARATOR = 179, */
  0,   /*    SDL_SCANCODE_CURRENCYUNIT = 180, */
  0,   /*    SDL_SCANCODE_CURRENCYSUBUNIT = 181, */
  0,   /*    SDL_SCANCODE_KP_LEFTPAREN = 182, */
  0,   /*    SDL_SCANCODE_KP_RIGHTPAREN = 183, */
  0,   /*    SDL_SCANCODE_KP_LEFTBRACE = 184, */
  0,   /*    SDL_SCANCODE_KP_RIGHTBRACE = 185, */
  0,   /*    SDL_SCANCODE_KP_TAB = 186, */
  0,   /*    SDL_SCANCODE_KP_BACKSPACE = 187, */
  0,   /*    SDL_SCANCODE_KP_A = 188, */
  0,   /*    SDL_SCANCODE_KP_B = 189, */
  0,   /*    SDL_SCANCODE_KP_C = 190, */
  0,   /*    SDL_SCANCODE_KP_D = 191, */
  0,   /*    SDL_SCANCODE_KP_E = 192, */
  0,   /*    SDL_SCANCODE_KP_F = 193, */
  0,   /*    SDL_SCANCODE_KP_XOR = 194, */
  0,   /*    SDL_SCANCODE_KP_POWER = 195, */
  0,   /*    SDL_SCANCODE_KP_PERCENT = 196, */
  0,   /*    SDL_SCANCODE_KP_LESS = 197, */
  0,   /*    SDL_SCANCODE_KP_GREATER = 198, */
  0,   /*    SDL_SCANCODE_KP_AMPERSAND = 199, */
  0,   /*    SDL_SCANCODE_KP_DBLAMPERSAND = 200, */
  0,   /*    SDL_SCANCODE_KP_VERTICALBAR = 201, */
  0,   /*    SDL_SCANCODE_KP_DBLVERTICALBAR = 202, */
  0,   /*    SDL_SCANCODE_KP_COLON = 203, */
  0,   /*    SDL_SCANCODE_KP_HASH = 204, */
  0,   /*    SDL_SCANCODE_KP_SPACE = 205, */
  0,   /*    SDL_SCANCODE_KP_AT = 206, */
  0,   /*    SDL_SCANCODE_KP_EXCLAM = 207, */
  0,   /*    SDL_SCANCODE_KP_MEMSTORE = 208, */
  0,   /*    SDL_SCANCODE_KP_MEMRECALL = 209, */
  0,   /*    SDL_SCANCODE_KP_MEMCLEAR = 210, */
  0,   /*    SDL_SCANCODE_KP_MEMADD = 211, */
  0,   /*    SDL_SCANCODE_KP_MEMSUBTRACT = 212, */
  0,   /*    SDL_SCANCODE_KP_MEMMULTIPLY = 213, */
  0,   /*    SDL_SCANCODE_KP_MEMDIVIDE = 214, */
  0,   /*    SDL_SCANCODE_KP_PLUSMINUS = 215, */
  0,   /*    SDL_SCANCODE_KP_CLEAR = 216, */
  0,   /*    SDL_SCANCODE_KP_CLEARENTRY = 217, */
  0,   /*    SDL_SCANCODE_KP_BINARY = 218, */
  0,   /*    SDL_SCANCODE_KP_OCTAL = 219, */
  0,   /*    SDL_SCANCODE_KP_DECIMAL = 220, */
  0,   /*    SDL_SCANCODE_KP_HEXADECIMAL = 221, */
  0,   /*    SDL_SCANCODE_UNKNOWN = 222, */
  0,   /*    SDL_SCANCODE_UNKNOWN = 223,  */
  162, /*    SDL_SCANCODE_LCTRL = 224, */
  160, /*    SDL_SCANCODE_LSHIFT = 225, */
  164, /*    SDL_SCANCODE_LALT = 226, */
  91,  /*    SDL_SCANCODE_LGUI = 227, */
  163, /*    SDL_SCANCODE_RCTRL = 228, */
  161, /*    SDL_SCANCODE_RSHIFT = 229, */
  0,   /*    SDL_SCANCODE_RALT = 230, */
  0,   /*    SDL_SCANCODE_RGUI = 231, */
  0,   /*    SDL_SCANCODE_UNKNOWN = 232,  */
  0,   /*    SDL_SCANCODE_UNKNOWN = 233,  */
  0,   /*    SDL_SCANCODE_UNKNOWN = 234,  */
  0,   /*    SDL_SCANCODE_UNKNOWN = 235,  */
  0,   /*    SDL_SCANCODE_UNKNOWN = 236,  */
  0,   /*    SDL_SCANCODE_UNKNOWN = 237,  */
  0,   /*    SDL_SCANCODE_UNKNOWN = 238,  */
  0,   /*    SDL_SCANCODE_UNKNOWN = 239,  */
  0,   /*    SDL_SCANCODE_UNKNOWN = 240,  */
  0,   /*    SDL_SCANCODE_UNKNOWN = 241,  */
  0,   /*    SDL_SCANCODE_UNKNOWN = 242,  */
  0,   /*    SDL_SCANCODE_UNKNOWN = 243,  */
  0   /*    SDL_SCANCODE_UNKNOWN = 244,  */
};

static const guint gdk_to_sdl[245] = {
  0,   /*  0   */
  0,   /*  1   */
  0,   /*  2   */
  0,   /*  3   */
  0,   /*  4   */
  0,   /*  5   */
  0,   /*  6   */
  0,   /*  7   */
  42,  /*  8   BACK*/
  43,  /*  9   TAB*/
  0,   /*  10  */
  0,   /*  11  */
  156, /*  12  CLEAR*/
  40,  /*  13  RETURN*/
  0,   /*  14  */
  0,   /*  15  */
  225, /*  16  SHIFT*/
  224, /*  17  CONTROL*/
  226, /*  18  ALT*/
  72,  /*  19  PAUSE*/
  57,  /*  20  CAPSLOCKS*/
  0,   /*  21  */
  0,   /*  22  */
  0,   /*  23  */
  0,   /*  24  */
  0,   /*  25  */
  0,   /*  26  */
  41,  /*  27  ESCAPE*/
  0,   /*  28  */
  0,   /*  29  */
  0,   /*  30  */
  0,   /*  31  */
  44,  /*  32  SPACE*/
  75,  /*  33  PAGE UP*/
  78,  /*  34  PAGE DOWN*/
  77,  /*  35  END*/
  74,  /*  36  HOME*/
  80,  /*  37  LEFT*/
  82,  /*  38  UP*/
  79,  /*  39  RIGHT*/
  81,  /*  40  */
  0,   /*  41  */
  0,   /*  42  */
  0,   /*  43  */
  70,  /*  44  PRINTSCREEN*/
  73,  /*  45  INSERT*/
  76,  /*  46  DELETE */
  0,   /*  47  */
  39,  /*  48  1*/
  30,  /*  49  2*/
  31,  /*  50  3*/
  32,  /*  51  4*/
  33,  /*  52  5*/
  34,  /*  53  6*/
  35,  /*  54  7*/
  36,  /*  55  8*/
  37,  /*  56  9*/
  38,  /*  57  0*/
  0,   /*  58   */
  0,   /*  59   */
  0,   /*  60   */
  0,   /*  61   */
  0,   /*  62   */
  0,   /*  63   */
  0,   /*  64   */
  4,   /*  65   A*/
  5,   /*  66   B*/
  6,   /*  67   C*/
  7,   /*  68   D*/
  8,   /*  69   E*/
  9,   /*  70   F*/
  10,  /*  71   G*/
  11,  /*  72   H*/
  12,  /*  73   I*/
  13,  /*  74   J*/
  14,  /*  75   K*/
  15,  /*  76   L*/
  16,  /*  77   M*/
  17,  /*  78   N*/
  18,  /*  79   O*/
  19,  /*  80   P*/
  20,  /*  81   Q*/
  21,  /*  82   R*/
  22,  /*  83   S*/
  23,  /*  84   T*/
  24,  /*  85   U*/
  25,  /*  86   V*/
  26,  /*  87   W*/
  27,  /*  88   X*/
  28,  /*  89   Y*/
  29,  /*  90   Z*/
  227, /*  91   LEFT WIN*/
  231, /*  92   RIGHT WIN*/
  101, /*  93   APPLICATION*/
  0,   /*  94   */
  0,   /*  95   */
  98,  /*  96   KP_0*/
  89,  /*  97   KP_1*/
  90,  /*  98   KP_2*/
  91,  /*  99   KP_3*/
  92,  /*  100  KP_4*/
  93,  /*  101  KP_5*/
  94,  /*  102  KP_6*/
  95,  /*  103  KP_7*/
  96,  /*  104  KP_8*/
  97,  /*  105  KP_9*/
  85,  /*  106  KP_MULTIPLY*/
  87,  /*  107  KP_PLUS*/
  0,   /*  108  */
  86,  /*  109  KP_MINUS*/
  99,  /*  110  KP_PERIOD*/
  84,  /*  111  KP_DIVIDE*/
  58,  /*  112  F1*/
  59,  /*  113  F2*/
  60,  /*  114  F3*/
  61,  /*  115  F4*/
  62,  /*  116  F5*/
  63,  /*  117  F6*/
  64,  /*  118  F7*/
  65,  /*  119  F8*/
  66,  /*  120  F9*/
  67,  /*  121  F10*/
  68,  /*  122  F11*/
  69,  /*  123  F12*/
  104, /*  124  F13*/
  105, /*  125  F14*/
  106, /*  126  F15*/
  107, /*  127  F16*/
  108, /*  128  F17*/
  109, /*  129  F18*/
  110, /*  130  F19*/
  111, /*  131  F20*/
  112, /*  132  F21*/
  113, /*  133  F22*/
  114, /*  134  F23*/
  115, /*  135  F24*/
  0,   /*  136  */
  0,   /*  137  */
  0,   /*  138  */
  0,   /*  139  */
  0,   /*  140  */
  0,   /*  141  */
  0,   /*  142  */
  0,   /*  143  */
  83,  /*  144  NUM LOCK*/
  71,  /*  145  SCROLL LOCK*/
  0,   /*  146  */
  0,   /*  147  */
  0,   /*  148  */
  0,   /*  149  */
  0,   /*  150  */
  0,   /*  151  */
  0,   /*  152  */
  0,   /*  153  */
  0,   /*  154  */
  0,   /*  155  */
  0,   /*  156  */
  0,   /*  157  */
  0,   /*  158  */
  0,   /*  159  */
  225, /*  160  LEFT SHIFT*/
  229, /*  161  RIGHT SHIFT*/
  224, /*  162  LEFT CONTROL*/
  228, /*  163  RIGHT CONTROL*/
  226, /*  164  LEFT ALT*/
  230, /*  165  RIGHT ALT*/
  0,   /*  166  */
  0,   /*  167  */
  0,   /*  168  */
  0,   /*  169  */
  0,   /*  170  */
  0,   /*  171  */
  0,   /*  172  */
  127, /*  173  VOLUME MUTE*/
  129, /*  174  VOLUME DOWN*/
  128, /*  175  VOLUME UP*/
  0,   /*  176  */
  0,   /*  177  */
  0,   /*  178  */
  0,   /*  179  */
  0,   /*  180  */
  0,   /*  181  */
  0,   /*  182  */
  0,   /*  183  */
  0,   /*  184  */
  0,   /*  185  */
  51,  /*  186  SEMICOLON*/
  46,  /*  187  EQUALS*/
  54,  /*  188  COMMA*/
  45,  /*  189  MINUS*/
  55,  /*  190  PERIOD*/
  56,  /*  191  SLASH*/
  53,  /*  192  GRAVE*/
  0,   /*  193  */
  0,   /*  194  */
  0,   /*  195  */
  0,   /*  196  */
  0,   /*  197  */
  0,   /*  198  */
  0,   /*  199  */
  0,   /*  200  */
  0,   /*  201  */
  0,   /*  202  */
  0,   /*  203  */
  0,   /*  204  */
  0,   /*  205  */
  0,   /*  206  */
  0,   /*  207  */
  0,   /*  208  */
  0,   /*  209  */
  0,   /*  210  */
  0,   /*  211  */
  0,   /*  212  */
  0,   /*  213  */
  0,   /*  214  */
  0,   /*  215  */
  0,   /*  216  */
  0,   /*  217  */
  0,   /*  218  */
  47,  /*  219  LEFTBRACKET*/
  49,  /*  220  BACKSLASH*/
  48,  /*  221  RIGHTBRACKET*/
  52,  /*  222  APOSTROPHE*/
  0,   /*  223  */
  0,   /*  224  */
  0,   /*  225  */
  100, /*  226  NONUSBACKSLASH*/
  0,   /*  227  */
  0,   /*  228  */
  0,   /*  229  */
  0,   /*  230  */
  0,   /*  231  */
  0,   /*  232  */
  0,   /*  233  */
  0,   /*  234  */
  0,   /*  235  */
  0,   /*  236  */
  0,   /*  237  */
  0,   /*  238  */
  0,   /*  239  */
  0,   /*  240  */
  0,   /*  241  */
  0,   /*  242  */
  0,   /*  243  */
  0    /*  244  */
};
#endif


static gchar*
med_input_get_sdl_key (MedInput* self,
                       const gchar* str)
{
  gchar* result;
  gchar** items;

  g_return_val_if_fail (self != NULL, NULL);
  g_return_val_if_fail (str != NULL, NULL);

  items = g_strsplit (str, "+", 0);
  result = g_strdup (items[0]);

  g_strfreev (items);

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
  len = g_strv_length (items);

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

  g_strfreev (items);
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

  if (g_strv_length (items) < 3)
  {
    g_strfreev (items);
    return text;
  }

  if (v[0] ==  'k')
  {
    if (g_strcmp0 (items[1], "0x0") == 0)
    {
      guint keyval;
      gchar* key;
      gchar* mods;
      guint64 a;

      key = med_input_get_sdl_key (self, items[2]);
      mods = med_input_check_mods (self, items[2]);

      a = g_ascii_strtoull (key, NULL, 10);
      if (a > 244)
        a = 0;

      GdkKeymap* keymap = gdk_keymap_get_for_display (gdk_display_get_default ());
      gdk_keymap_translate_keyboard_state (keymap, sdl_to_gdk[a], 0, 0, &keyval, NULL, NULL, NULL);

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
    if (priv->listjoy == NULL)
    {
      GtkWidget *parent = gtk_widget_get_toplevel ((GtkWidget*) self);
      priv->listjoy = g_object_get_data ((GObject*) parent, "listjoy");
    }

    text = med_list_joy_value_to_text (priv->listjoy, v);
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
      {
        text = g_strconcat ("Relative ", string_offset (tmp, 4), " (Mouse)", NULL);
        priv->is_mouse = TRUE;
      }
      else if (tmp[0] == 'c')
      {
        text = g_strconcat ("Cursor ", string_offset (tmp, 7), " (Mouse)", NULL);
        priv->is_mouse = TRUE;
      }

      g_free (tmp);

      *p = g_strdup (items[3]);
    }
    else
      text = g_strdup (v);

  }
  else
    text = g_strdup (v);

  g_strfreev (items);

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

  med_list_joy_enable_all(priv->listjoy, FALSE);

  g_signal_parse_name ("key-press-event", gtk_widget_get_type (), &id2, NULL, FALSE);

  g_signal_handlers_disconnect_matched ((GtkWidget*) priv->entry,
                                        G_SIGNAL_MATCH_ID | G_SIGNAL_MATCH_FUNC | G_SIGNAL_MATCH_DATA,
                                        id2,
                                        0,
                                        NULL,
                                        (GCallback) med_input_entry_key_press,
                                        self);

  priv->is_active = FALSE;
  gtk_toggle_button_set_active ((GtkToggleButton *) priv->entry, FALSE);
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
    med_widget_set_modified ((MedWidget*) mi, TRUE);
  }
}


static void
med_input_joy_event (MedListJoy* sender,
                     const gchar* text,
                     const gchar* value,
                     gpointer self)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (text != NULL);
  g_return_if_fail (value != NULL);

  MedInput* mi = self;

  input_set_text(mi, text, value);
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

  if (priv->_only_joy)
    return FALSE;

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
    gchar* tmp1;
    gchar* tmp2;

    if ((event->state & GDK_CONTROL_MASK) == GDK_CONTROL_MASK)
    {
      tmp1 = g_strconcat (mods, "Ctrl+", NULL);
      g_free (mods);
      mods = tmp1;

      tmp2 = g_strconcat (value, "+ctrl", NULL);
      g_free (value);
      value = tmp2;
    }

    if ((event->state & GDK_MOD1_MASK) == GDK_MOD1_MASK)
    {
      tmp1 = g_strconcat (mods, "Alt+", NULL);
      g_free (mods);
      mods = tmp1;

      tmp2 = g_strconcat (value, "+alt", NULL);
      g_free (value);
      value = tmp2;
    }

    if ((event->state & GDK_SHIFT_MASK) == GDK_SHIFT_MASK)
    {
      tmp1 = g_strconcat (mods, "Shift+", NULL);
      g_free (mods);
      mods = tmp1;

      tmp2 = g_strconcat (value, "+shift", NULL);
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
  g_return_if_fail (self != NULL);
  MedInputPrivate* priv = med_input_get_instance_private (self);

  priv->is_active = TRUE;

  g_signal_connect_object ((GtkWidget*) priv->entry, "key-press-event", (GCallback) med_input_entry_key_press, self, 0);

  if (priv->listjoy == NULL)
  {
    GtkWidget* parent = gtk_widget_get_toplevel ((GtkWidget*) self);
    priv->listjoy = g_object_get_data ((GObject*) parent, "listjoy");
  }

  g_signal_connect_object (priv->listjoy, "joy-event", (GCallback) med_input_joy_event, self, 0);
  med_list_joy_enable_all (priv->listjoy, TRUE);
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

  priv->_updated = value;
}


static gboolean
med_input_real_get_modified (MedWidget* base)
{
  MedInput* self = (MedInput*) base;
  MedInputPrivate* priv = med_input_get_instance_private (self);

  return priv->_modified;
}


static void
med_input_real_set_modified (MedWidget* base,
                            gboolean value)
{
  MedInput* self = (MedInput*) base;
  MedInputPrivate* priv = med_input_get_instance_private (self);

  priv->_modified = value;
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
  g_object_set ((GtkWidget*) priv->entry_label, "width-chars", value, NULL);
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


gboolean
med_input_get_only_joy (MedInput* self)
{
  g_return_val_if_fail (self != NULL, FALSE);
  MedInputPrivate* priv = med_input_get_instance_private (self);

  return priv->_only_joy;
}


void
med_input_set_only_joy (MedInput* self,
                        gboolean value)
{
  g_return_if_fail (self != NULL);
  MedInputPrivate* priv = med_input_get_instance_private (self);

  if (med_input_get_only_joy (self) != value)
  {
    priv->_only_joy = value;
    g_object_notify_by_pspec ((GObject *) self, med_input_properties[MED_INPUT_ONLY_JOY_PROPERTY]);
  }
}



static gboolean
med_input_entry_focus_out (GtkWidget* sender,
                           GdkEvent* event,
                           gpointer self)
{
  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  MedInput* mi = self;
  MedInputPrivate* priv = med_input_get_instance_private (mi);

  if (priv->is_active)
  {
    gtk_button_set_label (priv->entry, priv->old_text);
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
    priv->internal_value = g_strdup (" ");

    med_widget_set_modified ((MedWidget*) mi, TRUE);

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

  if (priv->is_mouse)
    return TRUE;

  if (event->type == GDK_BUTTON_PRESS)
  {
    if (event->button == 1)
    {
      if (priv->is_active)
      {
        input_set_text(mi, "Button left (Mouse)", "mouse 0x0 button_left");
        gtk_toggle_button_set_active ((GtkToggleButton*) sender, FALSE);
        return TRUE;
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

        if (priv->_only_joy)
          menu_input_enable_clear (priv->menu, (g_strcmp0 (gtk_button_get_label (priv->entry), "") != 0));
        else
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
med_input_finalize (GObject* obj)
{
  MedInput* self = G_TYPE_CHECK_INSTANCE_CAST (obj, med_input_get_type(), MedInput);
  MedInputPrivate* priv = med_input_get_instance_private (self);

  g_free (priv->_command);
  g_free (priv->_label);
  g_free (priv->old_text);
  g_free (priv->internal_value);
  g_free (priv->value);

  G_OBJECT_CLASS (med_input_parent_class)->finalize (obj);
}


MedInput*
med_input_new (void)
{
  MedInput* self = (MedInput*) g_object_new (med_input_get_type(), NULL);
  return self;
}


static void
med_input_init (MedInput* self)
{
}


static GObject*
med_input_constructor (GType type,
                       guint n_construct_properties,
                       GObjectConstructParam * construct_properties)
{
  GObjectClass* parent_class = G_OBJECT_CLASS (med_input_parent_class);
  GObject* obj = parent_class->constructor (type, n_construct_properties, construct_properties);

  MedInput* self = G_TYPE_CHECK_INSTANCE_CAST (obj, med_input_get_type(), MedInput);
  MedInputPrivate* priv = med_input_get_instance_private (self);

  priv->entry = (GtkButton*) gtk_toggle_button_new();
  gtk_button_set_alignment ((GtkButton*)priv->entry, 0.00, 0.50);

  priv->entry_label = (GtkLabel*) gtk_label_new (priv->_label);
  gtk_label_set_xalign(priv->entry_label, 1.00);
  //gtk_label_set_ellipsize (priv->entry_label, PANGO_ELLIPSIZE_END);

  gtk_box_pack_start ((GtkBox*) self, (GtkWidget*) priv->entry_label, FALSE, FALSE, 0);
  gtk_box_pack_start ((GtkBox*) self, (GtkWidget*) priv->entry, TRUE, TRUE, 10);

  gtk_widget_show ((GtkWidget*) priv->entry_label);
  gtk_widget_show ((GtkWidget*) priv->entry);

  g_signal_connect_object ((GtkWidget*) priv->entry, "leave-notify-event", (GCallback) med_input_entry_focus_out, self, 0);
  g_signal_connect_object ((GtkWidget*) priv->entry, "button-press-event", (GCallback) med_input_entry_mouse_clicked, self, 0);

  priv->is_active = FALSE;
  priv->is_mouse = FALSE;

  priv->old_text = g_strdup ("");
  priv->internal_value = g_strdup ("");
  priv->value = g_strdup ("");

  priv->listjoy = NULL;

  med_widget_init ((MedWidget*) self, (GtkWidget*) self);

  return obj;
}

static void
med_input_get_property (GObject* object,
                        guint property_id,
                        GValue* value,
                        GParamSpec* pspec)
{
  MedInput* self = G_TYPE_CHECK_INSTANCE_CAST (object, med_input_get_type(), MedInput);

  switch (property_id)
  {
    case MED_INPUT_COMMAND_PROPERTY:
      g_value_set_string (value, med_widget_get_command ((MedWidget*) self));
    break;
    case MED_INPUT_LABEL_PROPERTY:
      g_value_set_string (value, med_input_get_label (self));
    break;
    case MED_INPUT_MODIFIER_KEYS_PROPERTY:
      g_value_set_boolean (value, med_input_get_modifier_keys (self));
    break;
    case MED_INPUT_ONLY_JOY_PROPERTY:
      g_value_set_boolean (value, med_input_get_only_joy (self));
    break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
med_input_set_property (GObject* object,
                        guint property_id,
                        const GValue* value,
                        GParamSpec* pspec)
{
  MedInput* self = G_TYPE_CHECK_INSTANCE_CAST (object, med_input_get_type(), MedInput);

  switch (property_id)
{
    case MED_INPUT_COMMAND_PROPERTY:
      med_widget_set_command ((MedWidget*) self, g_value_get_string (value));
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
    case MED_INPUT_ONLY_JOY_PROPERTY:
      med_input_set_only_joy (self, g_value_get_boolean (value));
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

  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   MED_INPUT_ONLY_JOY_PROPERTY,
                                   med_input_properties[MED_INPUT_ONLY_JOY_PROPERTY] = g_param_spec_boolean
                                   (
                                     "only-joy",
                                     "only-joy",
                                     "only-joy",
                                     FALSE,
                                     G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE
                                   ));
}


static void
med_input_med_widget_interface_init (MedWidgetInterface* iface)
{
  iface->set_value = (void (*) (MedWidget*, const gchar*)) med_input_real_set_value;
  iface->get_value = (const gchar* (*) (MedWidget*)) med_input_real_get_value;

  iface->set_modified = (void (*) (MedWidget*, gboolean)) med_input_real_set_modified;
  iface->get_modified = (gboolean (*) (MedWidget*)) med_input_real_get_modified;

  iface->set_updated = (void (*) (MedWidget*, gboolean)) med_input_real_set_updated;
  iface->get_updated = (gboolean (*) (MedWidget*)) med_input_real_get_updated;

  iface->get_command = med_input_real_get_command;
  iface->set_command = med_input_real_set_command;
}
