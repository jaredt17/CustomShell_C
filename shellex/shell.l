/*
* shell.l: simple lexical analyzer for the shell.
*/
%{
#include <string.h>
#include "y.tab.h"
%}

%%
\n {
	return NEWLINE;
}

[ \t] {
	/* Discard spaces and tabs */
}


"<" {
	return LESS;
}

">" {
	return GREAT;
}

">>" {
	return GREATGREAT;
}

">&" {
	return GREATAMPERSAND;
}

">>&" {
	return GREATGREATAMPERSAND;
}

"|" {
	return PIPE;
}

"&" {
	return AMPERSAND;
}

[^ \t\n][^ \t\n]* {
	/* Assume that file names have only alpha chars */
	return WORD;
}

"." {
	/* Invalid character in input */
	return NOTOKEN;
}
%%
int yywrap( void ) { return 1; }
void getNext(enum yytokentype *  token, char text[], int size, int * length) {
	*token = yylex();
	*length = yyleng;
	strcpy(text, yytext);
}
