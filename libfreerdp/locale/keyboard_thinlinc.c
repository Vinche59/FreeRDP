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
#include <ctype.h>
#include "keyboard_thinlinc_parser.h"
#include "keyboard_thinlinc.h"
#include "liblocale.h"

BOOL thinlincKeyboardReady = FALSE;
UINT32 remote_modifiers_state = 0;
UINT32 saved_remote_modifiers_state = 0;
Pvoid_t  keymaps = (Pvoid_t) NULL;
extern FILE* yyin;

char *toUpper(char *s)
{
	int l = strlen(s);
	char *str = calloc(l + 1, sizeof(char));
	int i = 0;

	if (!str)
		return NULL;

	for (i = 0; i < l; i++)
		str[i] = toupper(s[i]);
	str[l] = '\0';

	return str;
}

int thinlinc_add_keys(char *keyname, DWORD rdp_scancode, UINT32 modifiers)
{
	int rc_int = 0;
	KeySym index = XStringToKeysym(keyname);;
	Word_t *PValue = NULL;
	tlkeymap *newKey = NULL;

	DEBUG_THINLINC("Add keys : %s - 0x%08x - 0x%08x", keyname, rdp_scancode, modifiers);

	if (index == NoSymbol)
	{
		ERROR_THINLINC("No Symbol for keysym : %s", keyname);
		free(keyname);
		return -1;
	}
	if (keymaps != NULL)
	{
		JLD(rc_int, keymaps, index);
		if (rc_int == JERR)
		{
			free(keyname);
			ERROR_THINLINC("Error while removing index %i from keymaps array", index);
			return -1;
		}
	}

	JLI(PValue, keymaps, index);
	if (PValue == PJERR)
	{
		ERROR_THINLINC("Error while inserting index %i in keymaps array", index);
	}
	else
	{
		newKey = calloc(1, sizeof(struct list_keymap));
		newKey->keysym = index;
		if (modifiers & ADDUPPER_MODIFIER)
			newKey->modifiers = modifiers & ~ADDUPPER_MODIFIER;
		else
			newKey->modifiers = modifiers;
		newKey->rdpscancode = rdp_scancode;
#ifdef WITH_DEBUG_THINLINC
		newKey->keyname = strdup(keyname);
#endif
		*PValue = (Word_t) newKey;
	}

	if (modifiers & ADDUPPER_MODIFIER)
	{
		modifiers &= ~ADDUPPER_MODIFIER;
		thinlinc_add_keys(toUpper(keyname), rdp_scancode, modifiers | SHIFT_MODIFIER);
	}

	free(keyname);
	return 0;
}

char *thinlinc_get_keymaps_path(char *p)
{
	const char *keymap_path = "share/rdesktop/keymaps/";
	char *path = NULL;
	char *prefix = NULL;
	DWORD nSize = GetEnvironmentVariableA("TLPREFIX", NULL, 0);

	if (nSize < 1)
		return NULL;

	prefix = calloc(nSize + strlen(keymap_path) + 1, sizeof(char));

	if (!prefix)
		return NULL;

	nSize = GetEnvironmentVariableA("TLPREFIX", prefix, nSize);
	strcat(prefix, "/");
	strcat(prefix, keymap_path);

	path = calloc(strlen(prefix) + strlen(p) + 1, sizeof(char));
	if (!path)
	{
		ERROR_THINLINC("Error while allocating thinlinc_get_keymaps_path");
		free(prefix);
		return NULL;
	}
	strcpy(path, prefix);
	strcat(path, p);

	free(prefix);
	return path;
}

int freerdp_keyboard_init_thinlinc(DWORD *keyboardLayoutId)
{
	char *locale = NULL;
	char *path = NULL;
#ifdef WITH_DEBUG_THINLINC
	Word_t *PValue = NULL;
	Word_t index = -1;
	tlkeymap *iter = NULL;
#endif

	freerdp_detect_keyboard_layout_from_system_locale_thinlinc(keyboardLayoutId, &locale);
	if (locale != NULL) {
		DEBUG_THINLINC("Locale found : %s", locale);
		path = thinlinc_get_keymaps_path(locale);
		if (!path)
		{
			free(locale);
			return -1;
		}
		yyin = fopen(path, "r");
		if (!yyin)
		{
			ERROR_THINLINC("Error while opening keymaps file : %s", path);
			free(locale);
			free(path);
			return -1;
		}
		else
		{
			yyparse();
#ifdef WITH_DEBUG_THINLINC
			JLL(PValue, keymaps, index);
			while (PValue != NULL)
			{
				iter = ((tlkeymap *) (*PValue));
				DEBUG_THINLINC("KeySym : 0x%08x - Modifiers : 0x%08x - RDP ScanCode : 0x%08x", iter->keysym, iter->modifiers, iter->rdpscancode);
				JLP(PValue, keymaps, index);
			}
#endif
			free(locale);
			free(path);
		}
	}
	else
		return -1;

	thinlincKeyboardReady = TRUE;
	return 1;
}

tlkeymap *freerdp_keyboard_get_rdp_scancode_from_thinlinc(KeySym keysym)
{
	Word_t *PValue;

	JLG(PValue, keymaps, keysym);
	if (PValue == PJERR)
	{
		ERROR_THINLINC("Internal Judy Library Error");
		return NULL;
	}
	if (PValue == NULL)
	{
		ERROR_THINLINC("Unknown keysym : 0x%08x", keysym);
		return NULL;
	}
	return ((tlkeymap *)(*PValue));
}

UINT32 thinlinc_get_remote_modifiers_state()
{
	return remote_modifiers_state;
}

void thinlinc_set_remote_modifiers_state(UINT32 remote_state)
{
	remote_modifiers_state |= remote_state;
}

void thinlinc_remove_remote_modifiers_state(UINT32 remote_state)
{
	remote_modifiers_state &= ~remote_state;
}

void thinlinc_set_saved_remote_modifiers_state()
{
	saved_remote_modifiers_state = remote_modifiers_state;
}

UINT32 thinlinc_get_saved_remote_modifiers_state()
{
	return saved_remote_modifiers_state;
}
