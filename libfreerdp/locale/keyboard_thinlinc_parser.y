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
 * %output  "keyboard_thinlinc_parser.c"
 * %defines "keyboard_thinlinc_parser.h"
 */
 
#include "keyboard_thinlinc.h"
#include "keyboard_thinlinc_lexer.h"
#include "keyboard_thinlinc_parser.h"
#include <stdio.h>

extern int yylex();

int yyerror(const char *s);

%}

%define parse.error verbose
 
%union {
	int value;
	char *keysym;
}
 
%token KEYS_KEYSYM
%token KEYS_SCANCODE
%token KEYS_MODIFIERS

%type <keysym> KEYS_KEYSYM 
%type <value> KEYS_SCANCODE KEYS_MODIFIERS

%start keymaps
%%

keymaps: keys;

keys : key
	| keys key;
	
key: KEYS_KEYSYM KEYS_SCANCODE { printf("KeySym : %s - ScanCode : 0x%08x\n", $1, $2); }
	| KEYS_KEYSYM KEYS_SCANCODE KEYS_MODIFIERS { printf("KeySym : %s - ScanCode : 0x%08x - Modifiers : 0x%08x\n", $1, $2, $3); }
	;
	

%%

int yyerror(const char *msg)
{
	fprintf(stderr, "Error:%s\n", msg);
	return 0;
}