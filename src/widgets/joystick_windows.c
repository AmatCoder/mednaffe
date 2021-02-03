/*
 * joystick_windows.c
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


#include <stdio.h>

#define DIRECTINPUT_VERSION 0x0500
#include <windows.h>
#include <windowsx.h>
#include <dinput.h>
#include <xinput.h>

#include "joystick.h"


static HMODULE x_dll_handle = NULL;
static DWORD (WINAPI *p_XInputGetState)(DWORD, XINPUT_STATE*) = NULL;
static DWORD (WINAPI *p_XInputGetCapabilities)(DWORD, DWORD, XINPUT_CAPABILITIES*) = NULL;

static LPDIRECTINPUT dii;


void
close_joys (GSList *list)
{
  GSList* it = NULL;
  for (it = list; it != NULL; it = it->next)
  {
    joy_s *joy = it->data;

    if (joy->type == 4)
    {
      LPDIRECTINPUTDEVICE2 dev = joy->data1;

      if(dev != NULL)
      {
        dev->lpVtbl->Unacquire(dev);
        dev->lpVtbl->Release(dev);
      }
    }

    g_free (joy->name);
    g_free (joy->id);
    g_free (joy);
  }

    if (x_dll_handle != NULL)
    {
      FreeLibrary (x_dll_handle);
      x_dll_handle = NULL;
    }
}


static GSList*
get_joy_xinput ()
{
  GSList *joy_list = NULL;
  int i = 0;

  while (i < XUSER_MAX_COUNT)
  {
    XINPUT_CAPABILITIES caps;
    ZeroMemory(&caps, sizeof(XINPUT_CAPABILITIES));

    if (p_XInputGetCapabilities(i, XINPUT_FLAG_GAMEPAD, &caps) == ERROR_SUCCESS)
    {
      joy_s *joy = g_new0 (joy_s, 1);
      joy->num = i;
      joy->type = 3;
      joy->name = NULL;
      joy->data1 = NULL;

      joy->id = g_strdup_printf ("0x%016x%02x%02x%04x%04x%04x", 0, caps.Type, caps.SubType, caps.Flags, caps.Gamepad.wButtons, 0);

      if (caps.Type == XINPUT_DEVTYPE_GAMEPAD)
      {
        switch (caps.SubType)
        {
          default: break;
          case XINPUT_DEVSUBTYPE_GAMEPAD: joy->name = g_strdup ("XInput Gamepad"); break;
          case XINPUT_DEVSUBTYPE_WHEEL: joy->name = g_strdup ("XInput Wheel"); break;
          case XINPUT_DEVSUBTYPE_ARCADE_STICK: joy->name = g_strdup ("XInput Arcade Stick"); break;
        #ifdef XINPUT_DEVSUBTYPE_FLIGHT_STICK
          case XINPUT_DEVSUBTYPE_FLIGHT_STICK: joy->name = g_strdup ("XInput Flight Stick"); break;
        #endif
          case XINPUT_DEVSUBTYPE_DANCE_PAD: joy->name = g_strdup ("XInput Dance Pad"); break;
        #ifdef XINPUT_DEVSUBTYPE_GUITAR_ALTERNATE
          case XINPUT_DEVSUBTYPE_GUITAR_ALTERNATE:
        #endif
        #ifdef XINPUT_DEVSUBTYPE_GUITAR_BASS
          case XINPUT_DEVSUBTYPE_GUITAR_BASS:
        #endif
          case XINPUT_DEVSUBTYPE_GUITAR: joy->name = g_strdup ("XInput Guitar"); break;
          case XINPUT_DEVSUBTYPE_DRUM_KIT: joy->name = g_strdup ("XInput Drum Kit"); break;
        #ifdef XINPUT_DEVSUBTYPE_ARCADE_PAD
          case XINPUT_DEVSUBTYPE_ARCADE_PAD: joy->name = g_strdup ("XInput Arcade Pad"); break;
        #endif
        }
      }
      if (joy->name == NULL)
        joy->name = g_strdup ("XInput Unknown Controller");

      joy_list = g_slist_append (joy_list, joy);
    }
    i++;
  }

  return joy_list;
}


static gboolean
IsXInputDevice (unsigned long guid)
{
  gboolean b = FALSE;

  HMODULE us32 = NULL;
  UINT WINAPI (*p_GetRawInputDeviceList)(PRAWINPUTDEVICELIST, PUINT, UINT) = NULL;
  UINT WINAPI (*p_GetRawInputDeviceInfo)(HANDLE, UINT, LPVOID, PUINT) = NULL;

  if((us32 = LoadLibrary("user32.dll")) == NULL)
    return b;

  p_GetRawInputDeviceList = (UINT WINAPI (*)(PRAWINPUTDEVICELIST, PUINT, UINT))GetProcAddress(us32, "GetRawInputDeviceList");
  p_GetRawInputDeviceInfo = (UINT WINAPI (*)(HANDLE, UINT, LPVOID, PUINT))GetProcAddress(us32, "GetRawInputDeviceInfoA");

  if(p_GetRawInputDeviceList && p_GetRawInputDeviceInfo)
  {
    unsigned int alloc_num_devices = 0;
    unsigned int num_devices = 0;

    p_GetRawInputDeviceList(NULL, &alloc_num_devices, sizeof(RAWINPUTDEVICELIST));
    RAWINPUTDEVICELIST ridl[alloc_num_devices+1];

    if ((num_devices = p_GetRawInputDeviceList(&ridl[0], &alloc_num_devices, sizeof(RAWINPUTDEVICELIST))) > 0)
    {
      for (unsigned i = 0; i < num_devices; i++)
      {
        if(ridl[i].dwType != RIM_TYPEHID)
          continue;

        RID_DEVICE_INFO devinfo;
        unsigned int sizepar;

        memset(&devinfo, 0, sizeof(devinfo));
        devinfo.cbSize = sizeof(RID_DEVICE_INFO);
        sizepar = sizeof(RID_DEVICE_INFO);
        if (p_GetRawInputDeviceInfo(ridl[i].hDevice, RIDI_DEVICEINFO, &devinfo, &sizepar) != sizeof(RID_DEVICE_INFO))
          continue;

        sizepar = 0;
        p_GetRawInputDeviceInfo(ridl[i].hDevice, RIDI_DEVICENAME, NULL, &sizepar);
        char devname[sizepar+1];
        devname[sizepar] ='\0';
        p_GetRawInputDeviceInfo(ridl[i].hDevice, RIDI_DEVICENAME, &devname[0], &sizepar);

        //printf("%s\n", devname);

        if(!strncmp(&devname[0], "IG_", 3) || strstr(&devname[0], "&IG_") != NULL)
        {
          if (guid == MAKELONG(devinfo.hid.dwVendorId, devinfo.hid.dwProductId))
          {
            //printf("ID Match: %08x\n",(unsigned int)MAKELONG(devinfo.hid.dwVendorId, devinfo.hid.dwProductId));
            b = TRUE;
            break;
          }
        }
      }
    }
  }

  FreeLibrary(us32);

  return b;
}


static BOOL CALLBACK
GLOB_EnumJoysticksProc (LPCDIDEVICEINSTANCE ddi, LPVOID private_data)
{
  GSList **list = (GSList **)private_data;

  if((ddi->dwDevType & 0xFF) == DIDEVTYPE_JOYSTICK)
  {
    if (!IsXInputDevice (ddi->guidProduct.Data1))
    {
      joy_s *joy = g_new0 (joy_s, 1);
      joy->num = g_slist_length (*list);
      joy->type = 4;

      joy->name = g_strndup (ddi->tszProductName, 260);
      joy->id = g_strdup_printf ("0x%08lx%04x%04x%02x%02x%02x%02x%02x%02x%02x%02x", ddi->guidProduct.Data1, ddi->guidProduct.Data2, ddi->guidProduct.Data3,
                                                                          ddi->guidProduct.Data4[0], ddi->guidProduct.Data4[1],
                                                                          ddi->guidProduct.Data4[2], ddi->guidProduct.Data4[3],
                                                                          ddi->guidProduct.Data4[4], ddi->guidProduct.Data4[5],
                                                                          ddi->guidProduct.Data4[6], ddi->guidProduct.Data4[7]);

    LPDIRECTINPUTDEVICE2 dev = NULL;
    LPDIRECTINPUTDEVICE tmp_dev = NULL;

    if (dii->lpVtbl->CreateDevice(dii, &(ddi->guidInstance), &tmp_dev, NULL) == DI_OK)
    {
      if (tmp_dev->lpVtbl->QueryInterface(tmp_dev, &IID_IDirectInputDevice2, (LPVOID *)&dev) == DI_OK)
      {
        dev->lpVtbl->SetDataFormat(dev, &c_dfDIJoystick2);
        dev->lpVtbl->SetCooperativeLevel(dev, GetDesktopWindow(), DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
        dev->lpVtbl->Acquire(dev);
      }
    }

    joy->data1 = dev;
    *list = g_slist_append (*list, joy);
    }
  }

  return DIENUM_CONTINUE;
}


static GSList*
get_joy_dinput (GSList* list)
{
  HRESULT hres = DirectInputCreate (GetModuleHandle(NULL), DIRECTINPUT_VERSION, &dii, NULL);
  if(hres != DI_OK) return list;

  dii->lpVtbl->EnumDevices (dii, DIDEVTYPE_JOYSTICK, GLOB_EnumJoysticksProc, &list, DIEDFL_ATTACHEDONLY);

  return list;
}


GSList*
init_joys (void)
{
  if (x_dll_handle == NULL)
  {
    if ((x_dll_handle = LoadLibrary("xinput1_3.dll")) == NULL)
    {
      if ((x_dll_handle = LoadLibrary("xinput1_4.dll")) == NULL)
      {
        if ((x_dll_handle = LoadLibrary("xinput9_1_0.dll")) == NULL)
        {
          return NULL;
        }
      }
    }
  }

  p_XInputGetState = (void*)GetProcAddress(x_dll_handle, (const char*)100/*"XInputGetStateEx"*/);

  if (p_XInputGetState == NULL)
    p_XInputGetState = (void*)GetProcAddress(x_dll_handle, "XInputGetState");

  p_XInputGetCapabilities = (void*)GetProcAddress(x_dll_handle, "XInputGetCapabilities");

  if ((p_XInputGetCapabilities == NULL) || (p_XInputGetState == NULL))
  {
    FreeLibrary (x_dll_handle);
    x_dll_handle = NULL;
    return NULL;
  }

  GSList *list = get_joy_xinput ();
  return get_joy_dinput (list);
}

void
discard_read (GSList *list)
{
}


/*
static gchar*
xinput_map (int b)
{
  gchar *string = NULL;

  switch (b)
  {
    case 0:  string = g_strdup("D-Pad Up"); break;
    case 1:  string = g_strdup("D-Pad Down"); break;
    case 2:  string = g_strdup("D-Pad Left"); break;
    case 3:  string = g_strdup("D-Pad Right"); break;
    case 4:  string = g_strdup("Button Start"); break;
    case 5:  string = g_strdup("Button Back"); break;
    case 6:  string = g_strdup("Stick Button Left"); break;
    case 7:  string = g_strdup("Stick Button Right"); break;
    case 8:  string = g_strdup("Shoulder Left"); break;
    case 9:  string = g_strdup("Shoulder Right"); break;
    case 12: string = g_strdup("Button A"); break;
    case 13: string = g_strdup("Button B"); break;
    case 14: string = g_strdup("Button X"); break;
    case 15: string = g_strdup("Button Y"); break;
    default : string = g_strdup("");break;
  }
  return string;
}


static gint
get_type_from_id (GSList* list,
                  const gchar* id)
{
  gint type = 0;

  GSList* it = NULL;
  for (it = list; it != NULL; it = it->next)
  {
    joy_s *joy = it->data;
    const gchar* joy_id = joy->id;

    if (g_strcmp0 (id, joy_id) == 0)
    {
      type = joy->type;
      break;
    }
  }

  return type;
}
*/


static const gchar*
get_name_from_id (GSList* list,
                  const gchar* id)
{
  g_return_val_if_fail (id != NULL, NULL);

  const gchar* name = "Unknown Device";

  GSList* it = NULL;
  for (it = list; it != NULL; it = it->next)
  {
    joy_s *joy = it->data;
    const gchar* joy_id = joy->id;

    if (g_strcmp0 (id, joy_id) == 0)
    {
      name = joy->name;
      break;
    }
  }

  return name;
}


gchar*
value_to_text (GSList* listjoy, const gchar* value)
{
  gchar* text = NULL;
  gchar** items = g_strsplit (value, " ", 4);

  if (g_strv_length (items) < 3)
    return text;

  if (items[2][0] == 'b')
  {
    if (strlen (items[2]) > 7)
    {
      /*
      gint type = get_type_from_id (listjoy, items[1]);

      if (type == 3)
      {
        gint b = g_ascii_strtoull ((items[2] + 7), NULL, 10);
        gchar* button = xinput_map (b);
        text = g_strconcat (button,
                            " (", get_name_from_id (listjoy, items[1]),
                            ")",  NULL);
        g_free (button);
      }
      else  */
        text = g_strconcat ("Button ", (items[2] + 7),
                            " (", get_name_from_id (listjoy, items[1]),
                            ")",  NULL);
    }
  }
  else if (items[2][0] == 'a')
  {
    if (strlen (items[2]) > 4)
    {
      const gchar* name = get_name_from_id (listjoy, items[1]);
      text = g_strconcat ("Axis ", (items[2] + 4), " (", name, ")", NULL);
    }
  }

  g_strfreev(items);

  return text;
}


static gchar*
read_joys_dinput (const joy_s *joy)
{
  gchar* value = NULL;

  DIJOYSTATE2 js;
  HRESULT hres;
  DIDEVCAPS DevCaps;
  LPDIRECTINPUTDEVICE2 dev = joy->data1;

  if (dev == NULL)
    return value;

  DevCaps.dwSize = sizeof(DevCaps);
  dev->lpVtbl->GetCapabilities(dev, &DevCaps);

  if(DevCaps.dwFlags & (DIDC_POLLEDDEVICE | DIDC_POLLEDDATAFORMAT))
  {
    do { hres = dev->lpVtbl->Poll(dev); } while((hres == DIERR_INPUTLOST) && ((hres = dev->lpVtbl->Acquire(dev)) == DI_OK));
    if (hres != DI_OK)
      return value;
  }

  do { hres = dev->lpVtbl->GetDeviceState(dev, sizeof(js), &js);  } while((hres == DIERR_INPUTLOST) && ((hres = dev->lpVtbl->Acquire(dev)) == DI_OK));
  if (hres != DI_OK)
    return value;

  gboolean p = FALSE;
  guint b;
  for (b = 0; b < DevCaps.dwButtons; b++)
  {
    if (js.rgbButtons[b] & 0x80)
    {
      p = TRUE;
      break;
    }
  }

  if (p)
  {
    gchar* n = g_strdup_printf ("%i", b);
    value = g_strconcat ("joystick ", joy->id, " button_", n, NULL);
    g_free (n);

    return value;
  }

  gint num_axes = 0;

  for (int rax = 0; rax < 8; rax++)
  {
    DIPROPRANGE diprg;

    diprg.diph.dwSize = sizeof(diprg);
    diprg.diph.dwHeaderSize = sizeof(diprg.diph);
    diprg.diph.dwObj = rax * sizeof(LONG);
    diprg.diph.dwHow = DIPH_BYOFFSET;

    hres = dev->lpVtbl->GetProperty(dev, DIPROP_RANGE, &diprg.diph);
    if (hres == DI_OK)
    {
      if (diprg.lMin < diprg.lMax)
      {
        num_axes++;
        gint64 rv = (&js.lX)[rax];
        rv = (((rv - diprg.lMin) * 32767 * 2) / (diprg.lMax - diprg.lMin)) - 32767;

        if (rv < -16000)
        {
          gchar* n = g_strdup_printf ("%i", rax);
          value = g_strconcat ("joystick ", joy->id, " abs_", n, "-", NULL);
          g_free (n);
          p = TRUE;
        }

        if (rv > 16000)
        {
          gchar* n = g_strdup_printf ("%i", rax);
          value = g_strconcat ("joystick ", joy->id, " abs_", n, "+", NULL);
          g_free (n);
          p = TRUE;
        }
      }
    }
  }

  if (p)
    return value;

 for (int hat = 0; hat < DevCaps.dwPOVs; hat++)
 {
   DWORD hat_value = js.rgdwPOV[hat];

    if (LOWORD(hat_value) == 0xFFFF)
      continue;

    hat_value += 4500 / 2;
    hat_value %= 36000;
    hat_value /= 4500;

    if (hat_value >= 8)
      continue;

    if (hat_value == 0 || hat_value == 1)
    {
      gchar* n = g_strdup_printf ("%i", ((hat*2) + num_axes+1));
      value = g_strconcat ("joystick ", joy->id, " abs_", n, "-", NULL);
      g_free (n);
    }
    else if (hat_value == 2 || hat_value == 3)
    {
      gchar* n = g_strdup_printf ("%i", ((hat*2) + num_axes));
      value = g_strconcat ("joystick ", joy->id, " abs_", n, "+", NULL);
      g_free (n);
    }
    else if (hat_value == 4 || hat_value == 5)
    {
      gchar* n = g_strdup_printf ("%i", ((hat*2) + num_axes+1));
      value = g_strconcat ("joystick ", joy->id, " abs_", n, "+", NULL);
      g_free (n);
    }
    else if (hat_value == 6 || hat_value == 7)
    {
      gchar* n = g_strdup_printf ("%i", ((hat*2) + num_axes));
      value = g_strconcat ("joystick ", joy->id, " abs_", n, "-", NULL);
      g_free (n);
    }
  }

  return value;
}


static gchar*
read_joys_xinput (const joy_s *joy)
{
  gchar* value = NULL;

  XINPUT_STATE joy_state;
  memset(&joy_state, 0, sizeof(XINPUT_STATE));

  p_XInputGetState (joy->num, &joy_state);

  gboolean p = FALSE;
  guint num_buttons = sizeof(((XINPUT_GAMEPAD*)0)->wButtons) * 8;

  guint b;
  for (b = 0; b < num_buttons; b++)
  {
    if ((joy_state.Gamepad.wButtons >> b) & 1)
    {
      p = TRUE;
      break;
    }
  }

  if (p)
  {
    gchar* n = g_strdup_printf ("%i", b);
    value = g_strconcat ("joystick ", joy->id, " button_", n, NULL);
    g_free (n);

    return value;
  }

  if (joy_state.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
    value = g_strconcat ("joystick ", joy->id, " abs_0+", NULL);
  else if (joy_state.Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
    value = g_strconcat ("joystick ", joy->id, " abs_0-", NULL);
  else if (joy_state.Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
    value = g_strconcat ("joystick ", joy->id, " abs_1+", NULL);
  else if (joy_state.Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
    value = g_strconcat ("joystick ", joy->id, " abs_1-", NULL);
  else if (joy_state.Gamepad.sThumbRX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
    value = g_strconcat ("joystick ", joy->id, " abs_2+", NULL);
  else if (joy_state.Gamepad.sThumbRX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
    value = g_strconcat ("joystick ", joy->id, " abs_2-", NULL);
  else if (joy_state.Gamepad.sThumbRY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
    value = g_strconcat ("joystick ", joy->id, " abs_3+", NULL);
  else if (joy_state.Gamepad.sThumbRY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
    value = g_strconcat ("joystick ", joy->id, " abs_3-", NULL);
  else if (joy_state.Gamepad.bLeftTrigger > 128)
    value = g_strconcat ("joystick ", joy->id, " abs_4+", NULL);
  else if (joy_state.Gamepad.bRightTrigger > 128)
    value = g_strconcat ("joystick ", joy->id, " abs_5+", NULL);

  return value;
}


gchar*
read_joys (GSList *list)
{
  gchar* value = NULL;

  GSList* it = NULL;
  for (it = list; it != NULL; it = it->next)
  {
    joy_s *joy = it->data;

    if (joy->type == 3)
      value = read_joys_xinput (joy);
    else if (joy->type == 4)
      value = read_joys_dinput (joy);

    if (value)
      break;
  }

  return value;
}
