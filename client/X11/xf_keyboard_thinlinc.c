/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * X11 Keyboard Mapping in ThinLinc Environment
 *
 * Copyright 2014-2015 Vincent Sourin <sourin-v@bridgestone-bae.com>
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

BOOL thinlinc_is_modifiers(UINT32 keysym)
{
	switch(keysym)
	{
	case XK_Shift_L:
	case XK_Shift_R:
	case XK_Alt_L:
	case XK_Alt_R:
	case XK_Control_L:
	case XK_Control_R:
	case XK_Super_L:
	case XK_Super_R:
	case XK_Num_Lock:
		return TRUE;
	default:
		return FALSE;
	}
}

BOOL thinlinc_are_modifiers_are_differents(UINT32 modifier, UINT32 keyModifiers, UINT32 remote_modifiers)
{
	if ((keyModifiers & modifier) != (remote_modifiers & modifier))
		return TRUE;
	return FALSE;
}

void thinlinc_update_remote_modifiers_state(UINT32 keysym, BOOL keyPressed)
{
	switch(keysym)
	{
	case XK_Shift_L:
	case XK_Shift_R:
		DEBUG_THINLINC("Remote Modifier State Before = 0x%x", thinlinc_get_remote_modifiers_state());
		keyPressed ? thinlinc_set_remote_modifiers_state(SHIFT_MODIFIER) : thinlinc_remove_remote_modifiers_state(SHIFT_MODIFIER);
		DEBUG_THINLINC("Remote Modifier State After = 0x%x", thinlinc_get_remote_modifiers_state());
		break;
	case XK_Alt_R:
		DEBUG_THINLINC("Remote Modifier State Before = 0x%x", thinlinc_get_remote_modifiers_state());
		keyPressed ? thinlinc_set_remote_modifiers_state(ALTGR_MODIFIER) : thinlinc_remove_remote_modifiers_state(ALTGR_MODIFIER);
		DEBUG_THINLINC("Remote Modifier State After = 0x%x", thinlinc_get_remote_modifiers_state());
		break;
	case XK_Num_Lock:
		if (keyPressed) {
			if (thinlinc_are_modifiers_are_differents(NUMLOCK_MODIFIER, NUMLOCK_MODIFIER, thinlinc_get_remote_modifiers_state()))
				thinlinc_set_remote_modifiers_state(NUMLOCK_MODIFIER);
			else
				thinlinc_remove_remote_modifiers_state(NUMLOCK_MODIFIER);
		}
		break;
	}
}

void thinlinc_send_keyboard_event(rdpInput *input, BOOL down, UINT32 keysym, BYTE rdp_scancode)
{
	thinlinc_update_remote_modifiers_state(keysym, down);
	freerdp_input_send_keyboard_event(input,
				(THINLINC_RDP_SCANCODE_EXTENDED(rdp_scancode) ? KBD_FLAGS_EXTENDED : 0) |
				((down) ? KBD_FLAGS_DOWN : KBD_FLAGS_RELEASE),
				RDP_SCANCODE_CODE(rdp_scancode));
	DEBUG_THINLINC("freerdp_input_send_keyboard_event_ex : send %s for keysym = 0x%x", down ? "KEY_PRESSED" : "KEY_RELEASED", keysym);
}


void thinlinc_handle_remote_modifiers_state(xfContext *xfc, UINT32 keysym, UINT32 modifiers, BOOL down)
{
	rdpInput* input = NULL;
	UINT32 remote = 0;

	if (thinlinc_is_modifiers(keysym))
		return;

	input = xfc->instance->input;
	remote = thinlinc_get_remote_modifiers_state();

	/* NumLock */
	if (thinlinc_are_modifiers_are_differents(NUMLOCK_MODIFIER, modifiers, remote))
	{
		if (modifiers & NUMLOCK_MODIFIER)
		{
			thinlinc_set_remote_modifiers_state(NUMLOCK_MODIFIER);
			freerdp_input_send_synchronize_event(input, KBD_SYNC_NUM_LOCK);
		}
		else
		{
			thinlinc_remove_remote_modifiers_state(NUMLOCK_MODIFIER);
			freerdp_input_send_synchronize_event(input, 0);

		}
	}

	/* Shift */
	if (thinlinc_are_modifiers_are_differents(SHIFT_MODIFIER, modifiers, remote))
	{
		if (modifiers & SHIFT_MODIFIER)
		{
			thinlinc_send_keyboard_event(input, TRUE, 0xffe1, RDP_SCANCODE_LSHIFT);
		}
		else
		{
			thinlinc_send_keyboard_event(input, FALSE, 0xffe1, RDP_SCANCODE_LSHIFT);
		}
	}

	/* AltGr */
	if (thinlinc_are_modifiers_are_differents(ALTGR_MODIFIER, modifiers, remote))
	{
		if (modifiers & ALTGR_MODIFIER)
		{
			thinlinc_send_keyboard_event(input, TRUE, 0xffea, RDP_SCANCODE_ALTGR);
		}
		else
		{
			thinlinc_send_keyboard_event(input, FALSE, 0xffea, RDP_SCANCODE_ALTGR);
		}
	}
}



void xf_keyboard_send_key_thinlinc(xfContext *xfc, BOOL down, KeySym keysym)
{
	tlkeymap *keymap = NULL;
	thinlinc_key *key = NULL;
	thinlinc_sequence *sequence = NULL;
	rdpInput* input = xfc->instance->input;

	if (!(keymap = freerdp_keyboard_get_rdp_scancode_from_thinlinc(keysym)))
		return;

	key = keymap->key;
	sequence = keymap->sequence;

	DEBUG_THINLINC("Send %s : keyname = %s - keysym = 0x%x - Scancode = 0x%x - Modifiers = 0x%x // Remote Modifiers = 0x%x", down ? "KEY_PRESSED":"KEY_RELEASED", key->keyname, key->keysym, key->rdpscancode, key->modifiers, thinlinc_get_remote_modifiers_state());

	if (key != NULL)
	{
		if (key->rdpscancode == RDP_SCANCODE_UNKNOWN || (key->modifiers & INHIBIT_MODIFIER))
		{
			return;
		}
		if (!thinlinc_is_modifiers(key->keysym))
			thinlinc_set_saved_remote_modifiers_state();
		thinlinc_handle_remote_modifiers_state(xfc, key->keysym, key->modifiers, down);
		thinlinc_send_keyboard_event(input, down, key->keysym, key->rdpscancode);
		thinlinc_handle_remote_modifiers_state(xfc, key->keysym, thinlinc_get_saved_remote_modifiers_state(), down);
		return;
	}
	else if (sequence != NULL)
	{
		if (down)
		{
			do
			{
				xf_keyboard_send_key_thinlinc(xfc, TRUE, sequence->keysym);
				xf_keyboard_send_key_thinlinc(xfc, FALSE, sequence->keysym);
				sequence = sequence->next_key;
			} while (sequence != NULL);
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

	if (xf_keyboard_handle_special_keys(xfc, keysym))
			return;

	xf_keyboard_send_key_thinlinc(xfc, FALSE, keysym);
}


