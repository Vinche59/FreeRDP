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
#include "keyboard_thinlinc_parser.h"
#include "keyboard_thinlinc.h"
#include "liblocale.h"

Pvoid_t  keymaps = (Pvoid_t) NULL;
extern FILE* yyin;

void thinlinc_add_keys(char *keyname, DWORD rdp_scancode, UINT32 modifiers)
{
	int rc_int = 0;
	Word_t index = XStringToKeysym(keyname);
	// TODO IF != NoSymbol
	DEBUG_THINLINC("Add new key : %s", keyname);
	if (keymaps != NULL)
	{
		JLD(rc_int, keymaps, index);
		if (rc_int == JERR)
		{
			free(keyname);
			ERROR_THINLINC("Error while removing index %i from keymaps array", index);
			return;
		}
	}

	tlkeymap *newkey = calloc(1, sizeof(struct list_keymap));
	if (!newkey)
	{
		ERROR_THINLINC("Error while allocating newkey");
		free(keyname);
		return;
	}
	newkey->keysym = index;
	newkey->modifiers = modifiers;
	newkey->rdpscancode = rdp_scancode;
	JLI(newkey, keymaps, index);
	if (newkey == PJERR)
	{
		ERROR_THINLINC("Error while inserting index %i from keymaps array", index);
	}
	free(keyname);
}

int freerdp_keyboard_init_thinlinc(DWORD *keyboardLayoutId)
{
	char *locale = NULL;
	tlkeymap *iter = NULL;
	Word_t index = -1;

	freerdp_detect_keyboard_layout_from_system_locale_thinlinc(keyboardLayoutId, &locale);
	if (locale != NULL) {
		DEBUG_THINLINC("Locale found : %s", locale);

		yyin = fopen("/opt/thinlinc/share/rdesktop/keymaps/fr-be", "r");
		if (!yyin)
		{
			free(locale);
			ERROR_THINLINC("Error while opening keymaps file");
			return -1;
		}
		else
		{
			yyparse();
			JLF(iter, keymaps, index);
			while (iter != NULL)
			{
				printf("KeySym : 0x%08x - Modifiers : 0x%08x - RDP ScanCode : 0x%08x\n", iter->keysym, iter->modifiers, iter->rdpscancode);
				JLN(iter, keymaps, index);
			}
			free(locale);
		}
	}
	else
		return -1;

	return 1;
}

tlkeymap *freerdp_keyboard_get_rdp_scancode_from_thinlinc(KeySym keysym)
{
	return NULL;
}


