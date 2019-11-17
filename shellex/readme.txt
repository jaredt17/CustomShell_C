
files that parse stdin and return a token and the text.
this is for a shell which has
>& or >>& to redirect stderr
> overwrite
>> append


lexical.c is a demonstration main() that calls the function getNext 
it also has a debug funtion printToken

if you write your own file with main() use at least
#include "y.tab.h"
extern void getNext(enum yytokentype *, char *, int, int *);
the parameters are
void getNext(enum yytokentype *  token, char text[], int size, int * length) {

where you send the char array: text[size]
length returns the length of what was copied into text

the file y.tab.h has 
enum yytokentype
  {
    NOTOKEN = 258,
    GREAT = 259,
    NEWLINE = 260,
    WORD = 261,
    GREATGREAT = 262,
    PIPE = 263,
    LESS = 264,
    GREATAMPERSAND = 265,
    GREATGREATAMPERSAND = 266,
    AMPERSAND = 267
  };

Do not change y.tab.h or yytokentype or all is lost

the files
lex.yy.c, y.tab.h, shell.l you should not need to edit

This code is a state maching generated using flex

