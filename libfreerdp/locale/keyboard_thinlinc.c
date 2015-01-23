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
#define JUDYERROR_NOTEST 1
#include <Judy.h>
#include <freerdp/locale/locale.h>
#include <freerdp/scancode.h>
#include <ctype.h>
#include <dirent.h>
#include "kbd_thinlinc_layout.h"
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

void thinlinc_set_keyboard_layout(DWORD layout, DWORD *keyboardLayoutId)
{
	*keyboardLayoutId = layout;
}

void thinlinc_create_sequence(char *keyname)
{
	int rc_int = 0;
	KeySym index = NoSymbol;
	Word_t *PValue = NULL;
	tlkeymap *newKey = NULL;

	DEBUG_THINLINC("Create Sequence for : %s", keyname);

	index = XStringToKeysym(keyname);
	if (index == NoSymbol)
	{
		ERROR_THINLINC("No Symbol for keysym : %s", keyname);
		free(keyname);
		return;
	}
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

	JLI(PValue, keymaps, index);
	if (PValue == PJERR)
	{
		ERROR_THINLINC("Error while inserting index %i in keymaps array", index);
	}
	else
	{
		newKey = calloc(1, sizeof(tlkeymap));
		newKey->key = NULL;
		dsa
		*PValue = (Word_t) newKey;
	}

	free(keyname);
}

void thinlinc_add_key_to_sequence(char *index, char *keyname)
{

}

void thinlinc_add_keys(char *keyname, BYTE rdp_scancode, UINT32 modifiers)
{
	int rc_int = 0;
	KeySym index = NoSymbol;
	Word_t *PValue = NULL;
	tlkeymap *newKey = NULL;

	DEBUG_THINLINC("Add keys : %s - 0x%x - 0x%x", keyname, rdp_scancode, modifiers);

	index = XStringToKeysym(keyname);
	if (index == NoSymbol)
	{
		ERROR_THINLINC("No Symbol for keysym : %s", keyname);
		free(keyname);
		return;
	}
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

	JLI(PValue, keymaps, index);
	if (PValue == PJERR)
	{
		ERROR_THINLINC("Error while inserting index %i in keymaps array", index);
	}
	else
	{
		newKey = calloc(1, sizeof(tlkeymap));
		newKey->sequence = NULL;
		newKey->key = calloc(1, sizeof(thinlinc_key));
		newKey->key->keysym = index;
		if (modifiers & ADDUPPER_MODIFIER)
			newKey->key->modifiers = modifiers & ~ADDUPPER_MODIFIER;
		else
			newKey->key->modifiers = modifiers;
		newKey->key->rdpscancode = rdp_scancode;
#ifdef WITH_DEBUG_THINLINC
		newKey->key->keyname = strdup(keyname);
#endif
		*PValue = (Word_t) newKey;
	}

	if (modifiers & ADDUPPER_MODIFIER)
	{
		modifiers &= ~ADDUPPER_MODIFIER;
		thinlinc_add_keys(toUpper(keyname), rdp_scancode, modifiers | SHIFT_MODIFIER);
	}

	free(keyname);
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
	int i = 0;
#ifdef WITH_DEBUG_THINLINC
	Word_t *PValue = NULL;
	Word_t index = -1;
	tlkeymap *iter = NULL;
#endif

	if (*keyboardLayoutId != 0) {
		locale = find_thinlinc_keymap_for_keyboard_layout(*keyboardLayoutId);
	}

	if (!locale) {
		if(*keyboardLayoutId != 0)
			ERROR_THINLINC("Cannot find keymap for layout Id : 0x%x. Trying with system locale.", *keyboardLayoutId);
		locale = freerdp_get_system_locale_thinlinc();
	}

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
			ERROR_THINLINC("Error while opening keymaps file : %s.", path);
			free(path);
			for (i = 0; i < strlen(locale); i++)
			{
				if (locale[i] == '-')
				{
					locale[i] = '\0';
					break;
				}
			}
			path = thinlinc_get_keymaps_path(locale);
			ERROR_THINLINC("Trying with keymaps file : %s", path);
			yyin = fopen(path, "r");
			if (!yyin)
			{
				ERROR_THINLINC("Error while opening keymaps file : %s.", path);
				free(locale);
				free(path);
				return -1;
			}
		}
		if (yyparse(keyboardLayoutId))
		{
			free(locale);
			free(path);
			return -1;
		}
#ifdef WITH_DEBUG_THINLINC
		JLL(PValue, keymaps, index);
		while (PValue != NULL)
		{
			iter = ((tlkeymap *) (*PValue));
			if (iter->key != NULL)
				DEBUG_THINLINC("KeySym : 0x%x - Modifiers : 0x%x - RDP ScanCode : 0x%x", iter->key->keysym, iter->key->modifiers, iter->key->rdpscancode);
			JLP(PValue, keymaps, index);
		}
#endif
		free(locale);
		free(path);
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
