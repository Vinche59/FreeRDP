%{
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
 * 
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include "keyboard_thinlinc.h"
#include "keyboard_thinlinc_parser.h"
#define YY_DECL int yylex()
unsigned int modifiers;
void setModifiers(unsigned int *modifiers, char *str);
%}

%option outfile="keyboard_thinlinc_lexer.c" header-file="keyboard_thinlinc_lexer.h"

%x KEYS
%x SEQUENCE
%x INCLUDE
%%
^#.*\n					{ /* Skip comments */ }
^include					{ BEGIN(INCLUDE); }
<INCLUDE>[[:space:]]+		{ /* skip spaces */ }
<INCLUDE>[^[:space:]\n]+	{ 
								char buf[1024];
								strcat(buf, THINLINC_KEYMAPS_DIRS);
								yyin = fopen(strcat(buf, yytext), "r");
								if (!yyin) {
									ERROR_THINLINC("Unknow keymaps file : %s", buf);
								}
								buf[0] = '\0';
								yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));
								BEGIN(INITIAL);
							}
^map.*\n	 			{ /* Skip map for the moment */ }
^sequence.*\n			{ /* Skip sequence for the moment */ }
^[[:alnum:]_]+			{ BEGIN(KEYS); modifiers = 0; yylval.keysym = strdup(yytext); return KEYS_KEYSYM; }
<KEYS>[[:space:]]+		{ /* skip spaces */ }
<KEYS>0x[[:alnum:]]+$	{ BEGIN(INITIAL); sscanf(yytext, "%i", &yylval.value); return KEYS_SCANCODE; }
<KEYS>0x[[:alnum:]]+	{ sscanf(yytext, "%i", &yylval.value); return KEYS_SCANCODE; }
<KEYS>[[:alpha:]]+		{ setModifiers(&modifiers, yytext); }
<KEYS>[[:alpha:]]+$		{ BEGIN(INITIAL); setModifiers(&modifiers, yytext); yylval.value = modifiers; return KEYS_MODIFIERS; }
.						{ /* ignore erreur */ }
<<EOF>>					{ 
							yypop_buffer_state();
							if (!YY_CURRENT_BUFFER)
								yyterminate(); 
						}
%%

void setModifiers(unsigned int *modifiers, char *str)
{
	if (strcmp(str, "shift") == 0 || strcmp(str, "addupper") == 0)
	{
		*modifiers |= SHIFT_MODIFIER;
		return;
	}
	if (strcmp(str, "altgr") == 0)
	{
		*modifiers |= ALTGR_MODIFIER;
		return;
	}
	if (strcmp(str, "localstate") == 0)
	{
		*modifiers |= LOCALSTATE_MODIFIER;
		return;
	}
	if (strcmp(str, "numlock") == 0)
	{
		*modifiers |= NUMLOCK_MODIFIER;
		return;
	}
	if (strcmp(str, "inhibit") == 0)
	{
		*modifiers |= INHIBIT_MODIFIER;
		return;
	}
	ERROR_THINLINC("Modifier Unknown : %s", str);	
}

