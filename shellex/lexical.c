#include <stdio.h>
#include "y.tab.h"
#include <stdlib.h>

enum state { START, PROGNAME, CLA, 	FILENAME,  INPUT_REDIRECT, OUTPUT_REDIRECT_APPEND, OUTPUT_REDIRECT_WRITE, EXECUTE, STDERR_OVERWRITE, STDERR_APPEND, ERROR, END};
char statestr[12][25] = { "START", "PROGNAME","CLA", 	"FILENAME",  "INPUT_REDIRECT", "OUTPUT_REDIRECT_APPEND", "OUTPUT_REDIRECT_WRITE", "EXECUTE", "STDERR_OVERWRITE", "STDERR_APPEND", "ERROR", "END" };

int table[9][10] = { // NoToken", "GREAT", "NEWLINE", "WORD", "GREATGREAT", "PIPE", "LESS", "GREATAPERSAND", "GREATGREATAPERSAND", "AMPERSAND"
					{  	ERROR   , ERROR  ,ERROR  , PROGNAME, ERROR, ERROR, ERROR, ERROR,ERROR,ERROR }, // start

				  	{  	ERROR, OUTPUT_REDIRECT_WRITE, EXECUTE, CLA, OUTPUT_REDIRECT_APPEND, START, INPUT_REDIRECT, STDERR_OVERWRITE, STDERR_APPEND, EXECUTE}, //PROGNAME

					{	ERROR, OUTPUT_REDIRECT_WRITE, EXECUTE, CLA, OUTPUT_REDIRECT_APPEND, START, INPUT_REDIRECT, STDERR_OVERWRITE, STDERR_APPEND, EXECUTE}, //CLA(WORD)

					{	ERROR, OUTPUT_REDIRECT_WRITE, EXECUTE, ERROR, OUTPUT_REDIRECT_APPEND, START, INPUT_REDIRECT, STDERR_OVERWRITE, STDERR_APPEND, EXECUTE}, //FILENAME

					{	ERROR, ERROR, ERROR, FILENAME,ERROR, ERROR, ERROR, ERROR, ERROR, ERROR}, //INPUT_REDIRECT

					{	ERROR, ERROR, ERROR, FILENAME,ERROR, ERROR, ERROR, ERROR, ERROR, ERROR}, //OUTPUT_REDIRECT_APPEND 

					{	ERROR, ERROR, ERROR, FILENAME,ERROR, ERROR, ERROR, ERROR, ERROR, ERROR},//OUTPUT_REDIRECT_WRITE
					
					{	ERROR, ERROR, ERROR, FILENAME,ERROR, ERROR, ERROR, ERROR, ERROR, ERROR},//STDERR_OVERWRITE

					{	ERROR, ERROR, ERROR, FILENAME,ERROR, ERROR, ERROR, ERROR, ERROR, ERROR}};//STDERR_APPEND


extern void getNext(enum yytokentype *, char *, int, int *);
#define TEXTSIZE 250

void printToken(enum yytokentype token) {
	static char * tokens[] = {"NoToken", "GREAT", "NEWLINE", "WORD", "GREATGREAT",
	"PIPE", "LESS", "GREATAPERSAND", "GREATGREATAPERSAND", "AMPERSAND" };
	printf("Token %s ", tokens[token-NOTOKEN]);
}


int main(int argc, char * argv[]) { 
    char text[TEXTSIZE];
	enum yytokentype token = NOTOKEN;
	
	enum state curstate = START;
	int pos = 0;
	int startpos = 0;

	//as of now running for only one command and then exiting, need overarching while loop
	int textlength;
	while(token != NEWLINE) {
		getNext(&token, text, TEXTSIZE, &textlength);
		//printToken( token );
		//printf("token %d, text >%s< \n", token, text);
		printf("CURstate %s,  token %d\n", statestr[curstate], token-NOTOKEN);
		curstate = table[curstate][token-NOTOKEN];
		switch (curstate){
			case PROGNAME:
				break;
			case CLA:
				break;
			//add all cases here for each step in the state machine
			//every time there is a pipe we need to create a simple command
			case ERROR:
				printf("Error exiting...");
				exit(0);
			default:
				break;

		}
		pos++;
	}
	return 0;
}
