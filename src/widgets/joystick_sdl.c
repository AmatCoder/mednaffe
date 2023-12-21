/* joystick_sdl.c - Copyright 2023 a dinosaur - SPDX: GPL-3.0-or-later */
#include "joystick.h"
#include <SDL2/SDL.h>

// Enable experimental SDL_GameController support
#define ENABLE_SDL_GAMECONTROLLER

static joy_s *OpenJoystick(int id);
static gchar *ReadJoystick(const gchar *joyid, SDL_Joystick *sdljoy);
static gchar *BuildGuid(const gchar *name, gssize namelen, SDL_Joystick *sdljoy);
static const joy_s *GetJoystickFromGUID(GSList *restrict list, const gchar *id);

#ifdef ENABLE_SDL_GAMECONTROLLER

typedef struct _GameController {
  SDL_GameController *sdlpad;
  SDL_Joystick *sdljoy;
  SDL_GameControllerButton map_button[SDL_CONTROLLER_BUTTON_MAX];
  SDL_GameControllerAxis map_axis[SDL_CONTROLLER_AXIS_MAX];
} GameController;

static joy_s *OpenGameController(int id);
static gchar *ReadGameController(const gchar *joyid, GameController *restrict pad);
static int IntFromStr(const char *str, long *restrict out);
static gchar *GameControllerButtonDescription(SDL_GameControllerType type, SDL_GameControllerButton button);
static gchar *GameControllerAxisDescription(SDL_GameControllerType type, SDL_GameControllerAxis axis);
static int GetGameControllerMapping(GameController *restrict pad);

#endif


gchar *value_to_text(GSList *listjoy, const gchar *value)
{
  gchar **items = g_strsplit(value, " ", 4);
  if (g_strv_length(items) < 3)
  {
    g_strfreev(items);
    return NULL;
  }

  const joy_s *joy = GetJoystickFromGUID(listjoy, items[1]);
  const gchar *devname = (joy) ? joy->name : "Unknown Device";

  gchar *text = NULL;
#ifdef ENABLE_SDL_GAMECONTROLLER
  if (joy && joy->type == 2)
  {
    const GameController *pad = joy->data1;
    long joyvalue;
    if (items[2][0] == 'b' && !IntFromStr(&items[2][7], &joyvalue))
    {
      for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i)
        if (pad->map_button[i] == joyvalue)
        {
          const gchar *description = GameControllerButtonDescription(SDL_GameControllerGetType(pad->sdlpad), i);
          if (description)
            text = g_strconcat("Button ", description, " (", devname, ")", NULL);
          break;
        }
    }
    else if (items[2][0] == 'a' && !IntFromStr(&items[2][4], &joyvalue))
    {
      for (int i = 0; i < SDL_CONTROLLER_AXIS_MAX; ++i)
        if (pad->map_axis[i] == joyvalue)
        {
          const gchar *description = GameControllerAxisDescription(SDL_GameControllerGetType(pad->sdlpad), i);
          if (description)
          {
            const gchar *findpos = strchr(&items[2][4], '+');
            const gchar *findneg = strchr(&items[2][4], '-');
            const gchar *polarity = "";
            if (findpos && findneg)
              polarity = findpos < findneg ? findpos : findneg;
            else if (findpos)
              polarity = findpos;
            else if (findneg)
              polarity = findneg;
            text = g_strconcat("Axis ", description, polarity, " (", devname, ")", NULL);
          }
          break;
        }
    }
  }

  if (!text)
  {
#endif
    if (items[2][0] == 'b')
      text = g_strconcat("Button ", &items[2][7], " (", devname, ")", NULL);
    else if (items[2][0] == 'a')
      text = g_strconcat("Axis ", &items[2][4], " (", devname, ")", NULL);
#ifdef ENABLE_SDL_GAMECONTROLLER
  }
#endif

  g_strfreev(items);
  return text;
}

GSList *init_joys(void)
{
  if (SDL_Init(SDL_INIT_GAMECONTROLLER) < 0)
    return NULL;

  GSList *joylist = NULL;
  const int numjoy = SDL_NumJoysticks();
  for (int i = 0; i < numjoy; ++i)
  {
#ifdef ENABLE_SDL_GAMECONTROLLER
    joy_s *joy = SDL_IsGameController(i) ? OpenGameController(i) : OpenJoystick(i);
#else
    joy_s *joy = OpenJoystick(i);
#endif
    if (!joy)
      continue;

    joylist = g_slist_append(joylist, joy);
  }
  return joylist;
}

void close_joys(GSList *list)
{
  for (GSList *it = list; it != NULL; it = it->next)
  {
    joy_s *joy = it->data;
    if (joy->type == 1)
    {
      SDL_JoystickClose(joy->data1);
    }
#ifdef ENABLE_SDL_GAMECONTROLLER
    else if (joy->type == 2)
    {
      GameController *pad = (GameController*)joy->data1;
      SDL_GameControllerClose(pad->sdlpad);
      g_free(pad);
    }
#endif
    g_free(joy->name);
    g_free(joy->id);
    g_free(joy);
  }

  SDL_Quit();
}

gchar *read_joys(GSList *list)
{
  SDL_PumpEvents();

  gchar *read = NULL;
  for (GSList *it = list; it != NULL; it = it->next)
  {
    joy_s *joy = it->data;
    if (joy->type == 1)
      read = ReadJoystick(joy->id, (SDL_Joystick*)joy->data1);
#ifdef ENABLE_SDL_GAMECONTROLLER
    else if (joy->type == 2)
      read = ReadGameController(joy->id, (GameController*)joy->data1);
#endif
    if (read)
      break;
  }
  return read;
}

void discard_read(GSList *list)
{
}


joy_s *OpenJoystick(int id)
{
  SDL_Joystick *sdljoy = SDL_JoystickOpen(id);
  if (!sdljoy)
    return NULL;

  gchar *joyname = NULL, *guid = NULL;

  const char *name = SDL_JoystickName(sdljoy);
  if (!name)
    goto OpenJoystickError;
  const gsize namelen = strlen(name);
  if (!(joyname = g_strndup(name, namelen)))
    goto OpenJoystickError;

  guid = BuildGuid(name, namelen, sdljoy);
  if (!guid)
    goto OpenJoystickError;

  joy_s *joy = g_new0(joy_s, 1);
  if (!joy)
    goto OpenJoystickError;
  joy->num   = (gint)SDL_JoystickGetDeviceInstanceID(id);
  joy->type  = 1;
  joy->name  = joyname;
  joy->id    = guid;
  joy->data1 = (gpointer)sdljoy;
  return joy;

OpenJoystickError:
  g_free(guid);
  g_free(joyname);
  SDL_JoystickClose(sdljoy);
  return NULL;
}

gchar *ReadJoystick(const gchar *joyid, SDL_Joystick *sdljoy)
{
  unsigned numaxes = SDL_JoystickNumAxes(sdljoy);
  unsigned numbtns = SDL_JoystickNumButtons(sdljoy);

  for (unsigned i = 0; i < numaxes; ++i)
  {
    Sint16 zero;
    if (SDL_JoystickGetAxisInitialState(sdljoy, i, &zero) == SDL_FALSE)
      zero = 0;

    Sint16 axis =  SDL_JoystickGetAxis(sdljoy, i);
    if (zero <= -0x4000)
    {
      if (axis >= 0)
        return g_strdup_printf("joystick %s abs_%u-+", joyid, i);
    }
    else if (zero >= 0x4000)
    {
      if (axis < 0)
        return g_strdup_printf("joystick %s abs_%u+-", joyid, i);
    }
    else if (axis <= -0x4000 || axis >= 0x4000)
    {
      const gchar polarity = axis < 0 ? '-' : '+';
      return g_strdup_printf("joystick %s abs_%u%c", joyid, i, polarity);
    }
  }

  for (unsigned i = 0; i < numbtns; ++i)
  {
    if (SDL_JoystickGetButton(sdljoy, i))
      return g_strdup_printf("joystick %s button_%u", joyid, i);
  }

  return NULL;
}


gchar *BuildGuid(const gchar *name, gssize namelen, SDL_Joystick *sdljoy)
{
  // Compute MD5 hash of Joystick name
  GChecksum *hash = g_checksum_new(G_CHECKSUM_MD5);
  if (!hash)
    return NULL;
  guint8 digest[16];
  gsize digestlen = 16;
  g_checksum_update(hash, (guchar*)name, namelen);
  g_checksum_get_digest(hash, &digest[0], &digestlen);
  g_checksum_free(hash);

  // Build unique ID, a joystick ID is computed by taking the first 64 bits of
  //  the MD5 hash of the name and appending total axis, buttons, hats, and balls.
  gchar *guid = g_malloc(35);
  if (!guid)
    return NULL;
  snprintf(&guid[0], 3, "0x");
  for (int i = 0; i < 8; ++i)
    snprintf(&guid[2 + i * 2], 3, "%02x", (unsigned char)digest[i]);

  unsigned numaxes = SDL_JoystickNumAxes(sdljoy);
  unsigned numbtns = SDL_JoystickNumButtons(sdljoy);
  unsigned numhats = SDL_JoystickNumHats(sdljoy);
  unsigned numball = SDL_JoystickNumBalls(sdljoy);
  snprintf(&guid[18], 17, "%04x%04x%04x%04x", numaxes, numbtns, numhats, numball);

  return guid;
}

const joy_s *GetJoystickFromGUID(GSList *restrict list, const gchar *id)
{
  if (!list || !id)
    return NULL;
  for (GSList *it = list; it != NULL; it = it->next)
  {
    const joy_s *joy = it->data;
    if (g_strcmp0(id, joy->id) == 0)
      return joy;
  }
  return NULL;
}


#ifdef ENABLE_SDL_GAMECONTROLLER

joy_s *OpenGameController(int id)
{
  SDL_GameController *sdlpad = SDL_GameControllerOpen(id);
  if (!sdlpad)
    return NULL;

  GameController *pad = NULL;
  gchar *joyname = NULL, *guid = NULL;

  pad = g_new0(GameController, 1);
  pad->sdlpad = sdlpad;
  pad->sdljoy = SDL_GameControllerGetJoystick(sdlpad);
  for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i)
    pad->map_button[i] = SDL_CONTROLLER_BUTTON_INVALID;
  for (int i = 0; i < SDL_CONTROLLER_AXIS_MAX; ++i)
    pad->map_axis[i] = SDL_CONTROLLER_AXIS_INVALID;

  const char *name = SDL_GameControllerName(sdlpad);
  if (!name)
    goto OpenGameControllerError;
  const gsize namelen = strlen(name);
  if (!(joyname = g_strndup(name, namelen)))
    goto OpenGameControllerError;

  guid = BuildGuid(name, namelen, pad->sdljoy);
  if (!guid || GetGameControllerMapping(pad))
    goto OpenGameControllerError;

  joy_s *joy = g_new0(joy_s, 1);
  if (!joy)
    goto OpenGameControllerError;
  joy->num   = (gint)SDL_JoystickGetDeviceInstanceID(id);
  joy->type  = 2;
  joy->name  = joyname;
  joy->id    = guid;
  joy->data1 = (gpointer)pad;
  return joy;

OpenGameControllerError:
  g_free(guid);
  g_free(joyname);
  g_free(pad);
  SDL_GameControllerClose(sdlpad);
  return NULL;
}

gchar *ReadGameController(const gchar *joyid, GameController *restrict pad)
{
  SDL_GameController *sdlpad = pad->sdlpad;

  for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i)
  {
    if (SDL_GameControllerGetButton(sdlpad, i) == SDL_PRESSED && pad->map_button[i] > SDL_CONTROLLER_BUTTON_INVALID)
      return g_strdup_printf("joystick %s button_%u", joyid, pad->map_button[i]);
  }

  for (int i = 0; i < SDL_CONTROLLER_AXIS_MAX; ++i)
  {
    if (pad->map_axis[i] <= SDL_CONTROLLER_AXIS_INVALID)
      continue;
    Sint16 axis =  SDL_GameControllerGetAxis(sdlpad, i);
    if ((i == SDL_CONTROLLER_AXIS_TRIGGERLEFT || i == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) && axis >= 0x4000)
    {
      return g_strdup_printf("joystick %s abs_%u-+", joyid, pad->map_axis[i]);
    }
    else if (axis <= -0x4000 || axis >= 0x4000)
    {
      const gchar polarity = axis < 0 ? '-' : '+';
      return g_strdup_printf("joystick %s abs_%u%c", joyid, pad->map_axis[i], polarity);
    }
  }

  return NULL;
}


int IntFromStr(const char *str, long *restrict out)
{
  errno = 0;
  char *end;
  long value = strtol(str, &end, 10);
  if (end == str)
    return 1;
  if (errno == ERANGE)
    return 2;
  *out = value;
  return 0;
}


gchar *GameControllerButtonDescription(SDL_GameControllerType type, SDL_GameControllerButton button)
{
  if (type == SDL_CONTROLLER_TYPE_PS3 || type == SDL_CONTROLLER_TYPE_PS4 || type == SDL_CONTROLLER_TYPE_PS5)
  {
    switch (button)
    {
    case SDL_CONTROLLER_BUTTON_A: return "X";
    case SDL_CONTROLLER_BUTTON_B: return "◯";
    case SDL_CONTROLLER_BUTTON_X: return "◻";
    case SDL_CONTROLLER_BUTTON_Y: return "△";
    case SDL_CONTROLLER_BUTTON_GUIDE: return "PS";
    case SDL_CONTROLLER_BUTTON_LEFTSTICK: return "L3";
    case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return "R3";
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return "L1";
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return "R1";
    case SDL_CONTROLLER_BUTTON_MISC1: return "Mic Mute";
    default: break;
    }
    if (button == SDL_CONTROLLER_BUTTON_START && (type == SDL_CONTROLLER_TYPE_PS4 || type == SDL_CONTROLLER_TYPE_PS5))
      return "Options";
    if (button == SDL_CONTROLLER_BUTTON_BACK)
      switch (type)
      {
      case SDL_CONTROLLER_TYPE_PS3: return "Select";
      case SDL_CONTROLLER_TYPE_PS4: return "Share";
      case SDL_CONTROLLER_TYPE_PS5: return "Create";
      default: break;
      }
  }
  if (type == SDL_CONTROLLER_TYPE_XBOX360 || type == SDL_CONTROLLER_TYPE_XBOXONE)
  {
    switch (button)
    {
    case SDL_CONTROLLER_BUTTON_LEFTSTICK: return "LS";
    case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return "RS";
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return "LB";
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return "RB";
    case SDL_CONTROLLER_BUTTON_MISC1: return "Share";
    case SDL_CONTROLLER_BUTTON_PADDLE1: return "P1";
    case SDL_CONTROLLER_BUTTON_PADDLE2: return "P2";
    case SDL_CONTROLLER_BUTTON_PADDLE3: return "P3";
    case SDL_CONTROLLER_BUTTON_PADDLE4: return "P4";
    default: break;
    }
  }
  if (type == SDL_CONTROLLER_TYPE_XBOXONE)
  {
    switch (button)
    {
    case SDL_CONTROLLER_BUTTON_BACK: return "View";
    case SDL_CONTROLLER_BUTTON_GUIDE: return "Xbox";
    case SDL_CONTROLLER_BUTTON_START: return "Option";
    default: break;
    }
  }

  switch (button)
  {
  case SDL_CONTROLLER_BUTTON_A: return "A";
  case SDL_CONTROLLER_BUTTON_B: return "B";
  case SDL_CONTROLLER_BUTTON_X: return "X";
  case SDL_CONTROLLER_BUTTON_Y: return "Y";
  case SDL_CONTROLLER_BUTTON_BACK: return "Back";
  case SDL_CONTROLLER_BUTTON_GUIDE: return "Guide";
  case SDL_CONTROLLER_BUTTON_START: return "Start";
  case SDL_CONTROLLER_BUTTON_LEFTSTICK: return "Left Stick Press";
  case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return "Right Stick Press";
  case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return "Left Shoulder";
  case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return "Right Shoulder";
  case SDL_CONTROLLER_BUTTON_DPAD_UP: return "D-Pad Up";
  case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return "D-Pad Down";
  case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return "D-Pad Left";
  case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return "D-Pad Right";
  case SDL_CONTROLLER_BUTTON_MISC1: return "Misc 1";
  case SDL_CONTROLLER_BUTTON_PADDLE1: return "Paddle 1";
  case SDL_CONTROLLER_BUTTON_PADDLE2: return "Paddle 2";
  case SDL_CONTROLLER_BUTTON_PADDLE3: return "Paddle 3";
  case SDL_CONTROLLER_BUTTON_PADDLE4: return "Paddle 4";
  case SDL_CONTROLLER_BUTTON_TOUCHPAD: return "Touchpad";
  default: return NULL;
  }
}

gchar *GameControllerAxisDescription(SDL_GameControllerType type, SDL_GameControllerAxis axis)
{
  if (type == SDL_CONTROLLER_TYPE_PS3 || type == SDL_CONTROLLER_TYPE_PS4 || type == SDL_CONTROLLER_TYPE_PS5)
  {
    switch (axis)
    {
    case SDL_CONTROLLER_AXIS_TRIGGERLEFT: return "L2";
    case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: return "R2";
    default: break;
    }
  }
  else if (type == SDL_CONTROLLER_TYPE_XBOX360 || type == SDL_CONTROLLER_TYPE_XBOXONE)
  {
    switch (axis)
    {
    case SDL_CONTROLLER_AXIS_TRIGGERLEFT: return "LT";
    case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: return "RT";
    default: break;
    }
  }
  switch (axis)
  {
  case SDL_CONTROLLER_AXIS_LEFTX: return "Left Stick X";
  case SDL_CONTROLLER_AXIS_LEFTY: return "Left Stick Y";
  case SDL_CONTROLLER_AXIS_RIGHTX: return "Right Stick X";
  case SDL_CONTROLLER_AXIS_RIGHTY: return "Right Stick Y";
  case SDL_CONTROLLER_AXIS_TRIGGERLEFT: return "Left Trigger";
  case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: return "Right Trigger";
  default: return NULL;
  }
}

int GetGameControllerMapping(GameController *restrict pad)
{
  const char *const btnmap_table[SDL_CONTROLLER_BUTTON_MAX] = {
    [SDL_CONTROLLER_BUTTON_A] = "a",
    [SDL_CONTROLLER_BUTTON_B] = "b",
    [SDL_CONTROLLER_BUTTON_X] = "x",
    [SDL_CONTROLLER_BUTTON_Y] = "y",
    [SDL_CONTROLLER_BUTTON_BACK] = "back",
    [SDL_CONTROLLER_BUTTON_GUIDE] = "guide",
    [SDL_CONTROLLER_BUTTON_START] = "start",
    [SDL_CONTROLLER_BUTTON_LEFTSTICK] = "leftstick",
    [SDL_CONTROLLER_BUTTON_RIGHTSTICK] = "rightstick",
    [SDL_CONTROLLER_BUTTON_LEFTSHOULDER] = "leftshoulder",
    [SDL_CONTROLLER_BUTTON_RIGHTSHOULDER] = "rightshoulder",
    [SDL_CONTROLLER_BUTTON_DPAD_UP] = "dpup",
    [SDL_CONTROLLER_BUTTON_DPAD_DOWN] = "dpdown",
    [SDL_CONTROLLER_BUTTON_DPAD_LEFT] = "dpleft",
    [SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = "dpright",
    [SDL_CONTROLLER_BUTTON_MISC1] = "misc1",
    [SDL_CONTROLLER_BUTTON_PADDLE1] = "paddle1",
    [SDL_CONTROLLER_BUTTON_PADDLE2] = "paddle2",
    [SDL_CONTROLLER_BUTTON_PADDLE3] = "paddle3",
    [SDL_CONTROLLER_BUTTON_PADDLE4] = "paddle4",
    [SDL_CONTROLLER_BUTTON_TOUCHPAD] = "touchpad"
  };
  const char *const axismap_table[SDL_CONTROLLER_AXIS_MAX] = {
    [SDL_CONTROLLER_AXIS_LEFTX] = "leftx",
    [SDL_CONTROLLER_AXIS_LEFTY] = "lefty",
    [SDL_CONTROLLER_AXIS_RIGHTX] = "rightx",
    [SDL_CONTROLLER_AXIS_RIGHTY] = "righty",
    [SDL_CONTROLLER_AXIS_TRIGGERLEFT] = "lefttrigger",
    [SDL_CONTROLLER_AXIS_TRIGGERRIGHT] = "righttrigger"
  };

  char *mapping = SDL_GameControllerMapping(pad->sdlpad);
  if (!mapping)
    return 1;

  gchar **items = g_strsplit(mapping, ",", 0);
  const unsigned numitems = g_strv_length(items);
  for (unsigned i = 2; i < numitems; ++i)
  {
    gchar **map = g_strsplit(items[i], ":", 2);
    if (g_strv_length(items) < 2)
      continue;
    long joyval;
    if (map[1][0] == 'b')
    {
      for (int j = 0; j < SDL_CONTROLLER_BUTTON_MAX; ++j)
        if (g_strcmp0(map[0], btnmap_table[j]) == 0 && !IntFromStr(&map[1][1], &joyval))
          pad->map_button[j] = joyval;
    }
    if (map[1][0] == 'a')
    {
      for (int j = 0; j < SDL_CONTROLLER_AXIS_MAX; ++j)
        if (g_strcmp0(map[0], axismap_table[j]) == 0 && !IntFromStr(&map[1][1], &joyval))
          pad->map_axis[j] = joyval;
    }
    g_strfreev(map);
  }

  g_strfreev(items);
  SDL_free(mapping);
  return 0;
}

#endif
