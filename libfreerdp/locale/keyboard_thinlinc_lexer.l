%{
/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * X11 Keyboard Mapping in ThinLinc Environment
 *
<<<<<<< HEAD
 * Copyright 2014-2015 Vincent Sourin <sourin-v@bridgestone-bae.com>
=======
 * Copyright 2014 Vincent Sourin <sourin-v@bridgestone-bae.com>
>>>>>>> 50de0f93ea5a05dc5262053f15988c56875c62a4
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
<<<<<<< HEAD
 * 
=======
>>>>>>> 50de0f93ea5a05dc5262053f15988c56875c62a4
 */
#include <stdio.h>
#include "keyboard_thinlinc.h"
#include "keyboard_thinlinc_parser.h"
#define YY_DECL int yylex()
<<<<<<< HEAD

unsigned int modifiers;
char *sequence_keysym = 0;
=======
unsigned int modifiers;
>>>>>>> 50de0f93ea5a05dc5262053f15988c56875c62a4
void setModifiers(unsigned int *modifiers, char *str);
%}

%option noinput
%option nounput
%option yylineno

%x KEYS
<<<<<<< HEAD
%x SEQ
%x SEQ_KEYS
=======
%x SEQUENCE
>>>>>>> 50de0f93ea5a05dc5262053f15988c56875c62a4
%x INCLUDE
%x MAP
%%
^[[:space:]]						{ /* Skip space beginning of line */ printf("Skip space beginning of line\n"); }
#.*\n								{ /* Skip comments */ printf("Skip comments\n");}
include								{ BEGIN(INCLUDE); printf("INCLUDE\n");}
<INCLUDE>[[:space:]]+				{ /* skip spaces */ printf("Skip space INCLUDE\n"); }
<INCLUDE>[^[:space:]\n]+			{ 
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
										DEBUG_THINLINC("Parsing file : %s", path);
										printf("Parsing file : %s\n", path);
										free(path);
										yylineno = 1;
										yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));
										BEGIN(INITIAL);
									}
<<<<<<< HEAD
map				 					{ BEGIN(MAP); printf("MAP\n"); }
<MAP>\n								{ BEGIN(INITIAL); printf("END OF MAP\n"); }
<MAP>[[:space:]]+					{ /* skip spaces */ printf("Skip space MAP\n"); }
<MAP>0(x|X)[[:alnum:]]+				{ sscanf(yytext, "%i", &yylval.uiVal); printf("MAP LAYOUT : --%s--\n", yytext); return LAYOUT; }
sequence							{ BEGIN(SEQ); printf("SEQ\n"); }
<SEQ,SEQ_KEYS>[[:space:]+$\n]		{ 
										printf("SEQ END\n"); 
										if (sequence_keysym != 0)
										{ 
											free(sequence_keysym);
											sequence_keysym = 0;
										}
										BEGIN(INITIAL);
									}
<SEQ,SEQ_KEYS>[[:space:]]+			{ /* skip spaces */ }
<SEQ>[[:alnum:]_]+					{ BEGIN(SEQ_KEYS); sequence_keysym = strdup(yytext); yylval.str = strdup(yytext); return SEQUENCE_KEYSYM; }
<SEQ_KEYS>[[:alnum:]_]+				{ yylval.str = strdup(yytext); return SEQUENCE_KEY; }
[[:alnum:]_]+						{ BEGIN(KEYS); modifiers = 0; yylval.str = strdup(yytext); printf("KEYSYM : --%s-- ", yytext); return KEYS_KEYSYM; }
<KEYS>[[:space:]+$\n]				{ BEGIN(INITIAL); printf("END OF KEYS\n"); }
=======
map				 					{ BEGIN(MAP); printf("MAP\n");}
<MAP>\n								{ BEGIN(INITIAL); printf("END OF MAP\n");}
<MAP>[[:space:]]+					{ /* skip spaces */ printf("Skip space MAP\n");}
<MAP>0(x|X)[[:alnum:]]+				{ sscanf(yytext, "%i", &yylval.uiVal); printf("MAP LAYOUT : --%s--\n", yytext); return LAYOUT; }
sequence							{ BEGIN(SEQUENCE); printf("SEQUENCE\n"); }
<SEQUENCE>\n						{ BEGIN(INITIAL); printf("END OF SEQUENCE\n"); }
<SEQUENCE>[[:space:]]+$				{ /* skip spaces */ BEGIN(INITIAL); printf("Skip space SEQUENCE\n");}
<SEQUENCE>[[:space:]]+				{ /* skip spaces */ }
<SEQUENCE>[[:alnum:]_]+				
[[:alnum:]_]+						{ BEGIN(KEYS); modifiers = 0; yylval.str = strdup(yytext); printf("KEYSYM : --%s-- ", yytext); return KEYS_KEYSYM; }
<KEYS>\n							{ BEGIN(INITIAL); printf("END OF KEYS\n"); }
<KEYS>[[:space:]]+$					{ /* skip spaces */ BEGIN(INITIAL); printf("Skip space KEYS\n");}
>>>>>>> 50de0f93ea5a05dc5262053f15988c56875c62a4
<KEYS>[[:space:]]+					{ /* skip spaces */ }
<KEYS>0(x|X)[[:alnum:]]+			{ sscanf(yytext, "%i", &yylval.uiVal); printf("SCANCODE : --%s-- ", yytext); return KEYS_SCANCODE; }
<KEYS>[[:alpha:]]+					{ setModifiers(&modifiers, yytext); printf("MODIFIERS 1 : --%s-- ", yytext); }
<KEYS>[[:alpha:]]+$					{ setModifiers(&modifiers, yytext); yylval.uiVal = modifiers; printf("MODIFIERS 2 : --%s--\n", yytext); return KEYS_MODIFIERS; }
<<<<<<< HEAD
<KEYS>.								{ /* skip unknown */ printf("Skip unknown KEYS : --%s--\n", yytext); }
.									{ /* skip unknown */ printf("Skip unknown : --%s--\n", yytext); }
=======
<KEYS>.								{ /* skip unknown */ printf("Skip unknown KEYS : --%s--\n", yytext);}
.									{ /* skip unknown */ printf("Skip unknown : --%s--\n", yytext);}
>>>>>>> 50de0f93ea5a05dc5262053f15988c56875c62a4
<<EOF>>								{
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
	ERROR_THINLINC("Modifier Unknown : --%s--", str);	
}

