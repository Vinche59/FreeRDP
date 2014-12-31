/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * X11 Keyboard Mapping in ThinLinc Environment
 *
 * Copyright 2014 Vincent Sourin <sourin-v@bridgestone-bae.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <winpr/wtypes.h>
#include <locale/keyboard_thinlinc.h>
#include "xfreerdp.h"
#include "xf_keyboard.h"
#include "xf_keyboard_thinlinc.h"

void xf_keyboard_send_key_thinlinc(xfContext *xfc, BOOL down, KeySym keysym)
{
	XF_MODIFIER_KEYS mod = { 0 };
	UINT32 simulatedKeys = 0;
	UINT32 releasedKeys = 0;
	tlkeymap *keymap = NULL;
	rdpInput* input;

	input = xfc->instance->input;
	keymap = freerdp_keyboard_get_rdp_scancode_from_thinlinc(keysym);

	if (!keymap || keymap->rdpscancode == RDP_SCANCODE_UNKNOWN)
	{
		return;
	}
	else
	{
		xk_keyboard_get_modifier_keys(xfc, &mod);
		DEBUG_THINLINC("xf_keyboard_send_key_thinlinc : Shift state : 0x%08x", mod.Shift);
		if ((keymap->modifiers & SHIFT_MODIFIER) && !mod.Shift)
		{
			DEBUG_THINLINC("xf_keyboard_send_key_thinlinc : Send LSHIFT");
			simulatedKeys |= SHIFT_MODIFIER;
			freerdp_input_send_keyboard_event_ex(input, TRUE, RDP_SCANCODE_LSHIFT);
		}

		if ((!(keymap->modifiers & SHIFT_MODIFIER)) && mod.Shift)
		{
			DEBUG_THINLINC("xf_keyboard_send_key_thinlinc : Release LSHIFT");
			releasedKeys |= SHIFT_MODIFIER;
			freerdp_input_send_keyboard_event_ex(input, FALSE, RDP_SCANCODE_LSHIFT);
		}

		if ((keymap->modifiers & ALTGR_MODIFIER) && !mod.RightAlt)
		{
			DEBUG_THINLINC("xf_keyboard_send_key_thinlinc : Send RALT");
			simulatedKeys |= ALTGR_MODIFIER;
			freerdp_input_send_keyboard_event_ex(input, TRUE, RDP_SCANCODE_RMENU);
		}

		//DEBUG_THINLINC("xf_keyboard_send_key_thinlinc : name : %s - scancode : 0x%08x - keysym : 0x%08x", keymap->name, keymap->rdpscancode, keymap->keysym);
		freerdp_input_send_keyboard_event_ex(input, down, keymap->rdpscancode);

		if (simulatedKeys)
		{
			if (simulatedKeys & SHIFT_MODIFIER)
				freerdp_input_send_keyboard_event_ex(input, FALSE, RDP_SCANCODE_LSHIFT);
			if (simulatedKeys & ALTGR_MODIFIER)
				freerdp_input_send_keyboard_event_ex(input, FALSE, RDP_SCANCODE_RMENU);
		}
	}
}

void xf_keyboard_key_press_thinlinc(xfContext* xfc, BYTE keycode, KeySym keysym)
{
	if (keycode < 8)
		return;

	xfc->KeyboardState[keycode] = keysym;

	if (xf_keyboard_handle_special_keys(xfc, keysym))
		return;

	xf_keyboard_send_key_thinlinc(xfc, TRUE, keysym);
}

void xf_keyboard_key_release_thinlinc(xfContext* xfc, BYTE keycode, KeySym keysym)
{
	if (keycode < 8)
		return;

	xfc->KeyboardState[keycode] = NoSymbol;
	xf_keyboard_send_key_thinlinc(xfc, FALSE, keysym);
}


