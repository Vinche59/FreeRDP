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
#define JUDYERROR_NOTEST 1
#include <Judy.h>
#include <freerdp/locale/locale.h>
#include <freerdp/scancode.h>
#include "keyboard_thinlinc.h"
#include "liblocale.h"
#include "keyboard_thinlinc_parser.h"

tlkeymap *keymaps = NULL;

void push_keysym(char *name, tlkeymap **head, UINT32 keysym, DWORD rpdscancode, UINT32 modifiers)
{
	tlkeymap *node = calloc(1, sizeof(tlkeymap));
	if (node) {
		node->name = name;
		node->keysym = keysym;
		node->rdpscancode = rpdscancode;
		node->modifiers = modifiers;
		node->next = *head;
		*head = node;
	}
}

int freerdp_keyboard_init_thinlinc(DWORD *keyboardLayoutId)
{
	char *locale = NULL;

	freerdp_detect_keyboard_layout_from_system_locale_thinlinc(keyboardLayoutId, &locale);
	if (locale != NULL) {
		DEBUG_THINLINC("Locale found : %s", locale);
		keymaps = calloc(1, sizeof (tlkeymap));
		keymaps->name = "eacute";
		keymaps->keysym = 0x00e9;
		keymaps->rdpscancode = 0x03;
		keymaps->modifiers = 0;
		keymaps->next = NULL;

		push_keysym("ampersand", &keymaps, 0x0026, 0x02, 0);
		push_keysym("LShift", &keymaps, 0xffe1, 0x2a, 0);
		push_keysym("1",&keymaps,0x0031, 0x02, SHIFT_MODIFIER);
		push_keysym("bar", &keymaps, 0x007c, 0x02, ALTGR_MODIFIER);
		push_keysym("exclamdown", &keymaps, 0x00a1, 0x02, SHIFT_MODIFIER | ALTGR_MODIFIER);

		free(locale);
	}
	else
		return -1;

	return 1;
}

tlkeymap *freerdp_keyboard_get_rdp_scancode_from_thinlinc(KeySym keysym)
{
	tlkeymap *curr = keymaps;
	while (curr)
	{
		if (curr->keysym == keysym)
			return curr;
		curr = curr->next;
	}
	return NULL;
}


