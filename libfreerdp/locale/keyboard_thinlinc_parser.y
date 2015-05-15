%{
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
#include "keyboard_thinlinc.h"
#include "keyboard_thinlinc_parser.h"
#include <stdio.h>

extern int yylex();
extern int yylineno;
char *sequence_keysym;
int yyerror(DWORD *keyboardLayoutId, const char *s);

%}

%parse-param {DWORD *keyboardLayoutId}
%define parse.error verbose
%locations

%union {
	unsigned int uiVal;
	char *str;
}
 
%token KEYS_KEYSYM
%token KEYS_SCANCODE
%token KEYS_MODIFIERS
%token LAYOUT
%token SEQUENCE_KEYSYM;
%token SEQUENCE_KEY;

%type <str> KEYS_KEYSYM SEQUENCE_KEYSYM SEQUENCE_KEY
%type <uiVal> KEYS_SCANCODE KEYS_MODIFIERS LAYOUT

%start keymap
%%

keymap : key
	| sequence
	| keymap sequence
	| keymap key
	;

key: LAYOUT { thinlinc_set_keyboard_layout($1, keyboardLayoutId); }
	| KEYS_KEYSYM KEYS_SCANCODE KEYS_MODIFIERS { thinlinc_add_keys($1, $2, $3); }
	| KEYS_KEYSYM KEYS_SCANCODE { thinlinc_add_keys($1, $2, 0); }
	;
	
sequence: SEQUENCE_KEYSYM { thinlinc_create_sequence($1); }
	| SEQUENCE_KEY { thinlinc_add_key_to_sequence(sequence_keysym, $1); }
	;

%%

int yyerror(DWORD *keyboardLayoutId, const char *msg)
{
	ERROR_THINLINC("Error near line #%i : %s", yylineno, msg);
	return -1;
}
