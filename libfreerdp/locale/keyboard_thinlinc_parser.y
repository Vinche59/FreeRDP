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
 */
#include "keyboard_thinlinc.h"
#include "keyboard_thinlinc_parser.h"
#include <stdio.h>

extern int yylex();

int yyerror(const char *s);

%}

%define parse.error verbose
%locations
%union {
	unsigned int uiVal;
	char *str;
}
 
%token KEYS_KEYSYM
%token KEYS_SCANCODE
%token KEYS_MODIFIERS

%type <str> KEYS_KEYSYM 
%type <uiVal> KEYS_SCANCODE KEYS_MODIFIERS

%start keymaps
%%

keymaps: keys;

keys : key
	| keys key;
	
key: KEYS_KEYSYM KEYS_SCANCODE { thinlinc_add_keys($1, $2, 0); }
	| KEYS_KEYSYM KEYS_SCANCODE KEYS_MODIFIERS { thinlinc_add_keys($1, $2, $3); }
	;
	

%%

int yyerror(const char *msg)
{
	ERROR_THINLINC("ThinLinc Keymaps Parser error : %s", msg);
	return -1;
}
