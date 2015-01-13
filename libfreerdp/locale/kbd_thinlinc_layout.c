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

#include <winpr/crt.h>

struct _THINLINC_KEYMAP_FILE
{
	UINT32 keyboardLayoutID; /* Keyboard Layout ID */
	const char* keymap_file; /* ThinLinc keymap */
};
typedef struct _THINLINC_KEYMAP_FILE THINLINC_KEYMAP_FILE;

static const THINLINC_KEYMAP_FILE thinlincKeymap[] =
{
		{ 0x00000401, "ar" },
		{ 0x00000405, "cs" },
		{ 0x00000406, "da" },
		{ 0x00000407, "de" },
		{ 0x00000807, "de-ch" },
		{ 0x00010409, "en-dv" },
		{ 0x00000809, "en-gb" },
		{ 0x00000409, "en-us" },
		{ 0x0000040a, "es" },
		{ 0x00000425, "et" },
		{ 0x0000040b, "fi" },
		{ 0x00000438, "fo" },
		{ 0x0000040c, "fr" },
		{ 0x0000080c, "fr-be" },
		{ 0x00000c0c, "fr-ca" },
		{ 0x0000100c, "fr-ch" },
		{ 0x0000040d, "he" },
		{ 0x0000041a, "hr" },
		{ 0x0000040e, "hu" },
		{ 0x0000040f, "is" },
		{ 0x00000410, "it" },
		{ 0x00010411, "ja" },
		{ 0x00010412, "ko" },
		{ 0x00000427, "lt" },
		{ 0x00000426, "lv" },
		{ 0x0000042f, "mk" },
		{ 0x00000413, "nl" },
		{ 0x00000813, "nl-be" },
		{ 0x00000414, "no" },
		{ 0x00000415, "pl" },
		{ 0x00000816, "pt" },
		{ 0x00000416, "pt-br" },
		{ 0x00000419, "ru" },
		{ 0x0000041b, "sk" },
		{ 0x00000424, "sl" },
		{ 0x0000041d, "sv" },
		{ 0x0000041e, "th" },
		{ 0x0000041f, "tr" },
};

char *find_thinlinc_keymap_for_keyboard_layout(DWORD keyboardLayoutId)
{
	int i = 0;

	if (keyboardLayoutId == 0)
		return NULL;

	for (i = 0; i < ARRAYSIZE(thinlincKeymap); i++)
	{
		if (thinlincKeymap[i].keyboardLayoutID == keyboardLayoutId)
			return strdup(thinlincKeymap[i].keymap_file);
	}

	return NULL;
}

