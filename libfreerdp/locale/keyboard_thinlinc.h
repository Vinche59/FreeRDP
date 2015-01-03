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

#ifndef KEYBOARD_THINLINC_H_
#define KEYBOARD_THINLINC_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define JUDYERROR_NOTEST 1
#include <Judy.h>
#include <freerdp/types.h>
#include <X11/Xlib.h>
#include <freerdp/log.h>
#include <winpr/environment.h>

#define THINLINC_TAG FREERDP_TAG("thinlinc")

#define ERROR_THINLINC(fmt, ...) WLog_ERR(THINLINC_TAG, fmt, ## __VA_ARGS__)

#ifdef WITH_DEBUG_THINLINC
#define DEBUG_THINLINC(fmt, ...) WLog_DBG(THINLINC_TAG, fmt, ## __VA_ARGS__)
#else
#define DEBUG_THINLINC(fmt, ...) do { } while (0)
#endif


#define SHIFT_MODIFIER 0x00000001
#define ALTGR_MODIFIER 0x00000010
#define LOCALSTATE_MODIFIER 0x00000100
#define NUMLOCK_MODIFIER 0x0001000
#define INHIBIT_MODIFIER 0x00010000
#define ADDUPPER_MODIFIER 0x00100000


struct list_keymap
{
	UINT32 keysym;
	UINT32 modifiers;
	DWORD rdpscancode;
#ifdef WITH_DEBUG_THINLINC
	char  *keyname;
#endif
};

typedef struct list_keymap tlkeymap;

int freerdp_keyboard_init_thinlinc(DWORD *keyboardLayoutId);
int thinlinc_add_keys(char *keyname, DWORD rdp_scancode, UINT32 modifiers);
tlkeymap *freerdp_keyboard_get_rdp_scancode_from_thinlinc(KeySym keysym);
char *thinlinc_get_keymaps_path(char *p);
UINT32 thinlinc_get_remote_modifiers_state();
void thinlinc_set_remote_modifiers_state(UINT32 remote_state);
void thinlinc_remove_remote_modifiers_state(UINT32 remote_state);
void thinlinc_set_saved_remote_modifiers_state();
UINT32 thinlinc_get_saved_remote_modifiers_state();

#endif /* KEYBOARD_THINLINC_H_ */
