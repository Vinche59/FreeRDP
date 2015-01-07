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
#include <stdio.h>
#include "keyboard_thinlinc.h"
#include "keyboard_thinlinc_parser.h"
#define YY_DECL int yylex()
unsigned int modifiers;
void setModifiers(unsigned int *modifiers, char *str);
%}

%option noinput
%option nounput
%option yylineno

%x KEYS
%x SEQUENCE
%x INCLUDE
%x MAP
%%
^[[:space:]]				{ /* Skip space beginning of line */ }
#.*\n						{ /* Skip comments */ }
include						{ BEGIN(INCLUDE); }
<INCLUDE>[[:space:]]+		{ /* skip spaces */ }
<INCLUDE>[^[:space:]\n]+	{ 
								char *path = thinlinc_get_keymaps_path(yytext);
								if (!path) 
								{
									return -1;
								}
								yyin = fopen(path, "r");
								if (!yyin) 
								{
									ERROR_THINLINC("Unknow keymaps file : %s", path);
									free(path);
									return -1;
								}
								free(path);
								yylineno = 1;
								yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));
								BEGIN(INITIAL);
							}
map		 					{ BEGIN(MAP); }
<MAP>[[:space:]]+			{ /* skip spaces */ }
<MAP>0(x|X)[[:alnum:]]+$	{ BEGIN(INITIAL); sscanf(yytext, "%i", &yylval.uiVal); return LAYOUT; }
sequence.*\n				{ /* Skip sequence for the moment */ }
^[[:alnum:]_]+				{ BEGIN(KEYS); modifiers = 0; yylval.str = strdup(yytext); return KEYS_KEYSYM; }
<KEYS>[[:space:]]+			{ /* skip spaces */ }
<KEYS>0(x|X)[[:alnum:]]+$	{ BEGIN(INITIAL); sscanf(yytext, "%i", &yylval.uiVal); return KEYS_SCANCODE; }
<KEYS>0(x|X)[[:alnum:]]+	{ sscanf(yytext, "%i", &yylval.uiVal); return KEYS_SCANCODE; }
<KEYS>[[:alpha:]]+			{ setModifiers(&modifiers, yytext); }
<KEYS>[[:alpha:]]+$			{ BEGIN(INITIAL); setModifiers(&modifiers, yytext); yylval.uiVal = modifiers; return KEYS_MODIFIERS; }
<KEYS>.						{ /* skip unknown */ }
<*>\n						{ /* skip newline */ }
<*>.						{ printf("Unknown : %c\n", yytext[0]); /* skip unknown */ }
<<EOF>>						{
								yypop_buffer_state();
								yylineno = 1;
								if (!YY_CURRENT_BUFFER)
									yyterminate();
							}
%%

void setModifiers(unsigned int *modifiers, char *str)
{
	if (strcmp(str, "addupper") == 0)
	{
		*modifiers |= ADDUPPER_MODIFIER;
		return;
	}
	if (strcmp(str, "shift") == 0)
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

