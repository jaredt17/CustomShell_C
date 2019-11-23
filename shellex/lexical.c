#include <stdio.h>
#include "y.tab.h"
#include <stdlib.h>
#include <string.h>

enum state { START, PROGNAME, CLA, 	FILENAME,  INPUT_REDIRECT, OUTPUT_REDIRECT_APPEND, OUTPUT_REDIRECT_WRITE, EXECUTE, STDERR_OVERWRITE, STDERR_APPEND, ERROR, END};
char statestr[12][25] = { "START", "PROGNAME","CLA", 	"FILENAME",  "INPUT_REDIRECT", "OUTPUT_REDIRECT_APPEND", "OUTPUT_REDIRECT_WRITE", "EXECUTE", "STDERR_OVERWRITE", "STDERR_APPEND", "ERROR", "END" };

// Command Data Structure
// Describes a simple command and arguments
typedef struct SimpleCommand {
	// Available space for arguments currently preallocated
	int _numberOfAvailableArguments;
	// Number of arguments
	int _numberOfArguments;
	// Array of arguments
	char *_arguments[];
}SimpleCommand;


void insertArgument(struct SimpleCommand *command, char * argument ); // or global *_currentSimpleCommand instead of parameter

// Describes a complete command with the multiple pipes if any
// and input/output redirection if any.
struct Command {
        int _numberOfAvailableSimpleCommands;
        int _numberOfSimpleCommands;
        SimpleCommand ** _simpleCommands;
        char * _outFile;
        char * _inputFile;
        char * _errFile;
        int _background;
};

static struct Command _currentCommand;

void insertSimpleCommand( SimpleCommand * simpleCommand );
void print();
void execute();
void clear();
					//  0 			1			2		3		4			5			6		7					8					9
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

	//set up simple command struct
	//initialize the global currentsimplecommand struct
	struct SimpleCommand *_currentSimpleCommand = malloc(sizeof(*_currentSimpleCommand) + sizeof(char[20]));
	_currentSimpleCommand->_numberOfArguments = 0;
	_currentSimpleCommand->_numberOfAvailableArguments = 1;
	_currentSimpleCommand->_arguments[_currentSimpleCommand->_numberOfArguments] = (char*)malloc(sizeof(char*) * 50);

    char text[TEXTSIZE];
	enum yytokentype token = NOTOKEN;
	
	enum state curstate = START;
	int pos = 0;
	int startpos = 0;
	int numSimpleCommands = 1;
	//as of now running for only one command and then exiting, need overarching while loop
	int textlength;
	while(token != NEWLINE) {
		getNext(&token, text, TEXTSIZE, &textlength);
		//printToken( token );
		//printf("token %d, text >%s< \n", token, text);
		printf("CURstate %s,  token %d\n", statestr[curstate], token-NOTOKEN);
		curstate = table[curstate][token-NOTOKEN];
		switch (curstate){ //we will have specific actions for each state defined in the table, every time there is a pipe we need to create a simple command
			case START:
				//note we will only get here on starts after the first one
				printf("\nStart of next command\n");
				numSimpleCommands++;
				break;
			case PROGNAME:
				insertArgument(_currentSimpleCommand,text);
				break;
			case CLA:
				break;
			case FILENAME:
				break;
			case INPUT_REDIRECT:
				break;
			case OUTPUT_REDIRECT_APPEND:
				break;
			case OUTPUT_REDIRECT_WRITE:
				break;
			case STDERR_OVERWRITE:
				break;
			case STDERR_APPEND:
				break;
			case EXECUTE:
				break;


			case ERROR:
				printf("Error exiting...");
				exit(0);
			default:
				break;

		}
	}
	print(_currentSimpleCommand);
	//call execute to run the commands we just built
	//execute(numSimpleCommands);
	return 0;
}

//insert argument into simple command
void insertArgument(struct SimpleCommand *command, char * argument ) {
	//allocate space for the new command
	command->_arguments[command->_numberOfArguments] = (char*)malloc(sizeof(char*) * strlen(argument));
	strcpy(command->_arguments[command->_numberOfArguments], argument);

	command->_numberOfArguments++;
}

void print(struct SimpleCommand *command){
	for(int i = 0; i < command->_numberOfArguments; i++){ //for the number of commands
		printf("\nArg %d: %s\n", i, command->_arguments[i]);
	}
}


// void execute(int numsimplecommands){
// 	//save in/out
// 	int tmpin = dup(0);
// 	int tmpout = dup(1);

// 	//set the initial input
// 	int fdin;
// 	if (infile){
// 		fdin = open(infile, O_READ);
// 	}
// 	else{

// 		// Use default input

// 		fdin = dup(tmpin);
// 	}

// 	int ret;
// 	int fdout;
// 	for (int i = 0; i < numsimplecommands; i++){

// 		//redirect input
// 		dup2(fdin, 0);
// 		close(fdin);
// 		//setup output
// 		if (i == numsimplecommands - 1){

// 			// Last simple command
// 			if (outfile){
// 				fdout = open(outfile, â€¦â€¦);
// 			}
// 			else{
// 				// Use default output
// 				fdout = dup(tmpout);
// 			}
// 		}

// 		else{
// 			// Not last
// 			//simple command
// 			//create pipe
// 			int fdpipe[2];
// 			pipe(fdpipe);
// 			fdout = fdpipe[1];
// 			fdin = fdpipe[0];
// 		} // if/else

// 		// Redirect output
// 		dup2(fdout, 1);
// 		close(fdout);

// 		// Create child process
// 		ret = fork();
// 		if (ret == 0){
// 			execvp(scmd[i].args[0], scmd[i].args);
// 			perror("Execvp:");
// 			_exit(1);
// 		}
// 	} //  for

// 	//restore in/out defaults
// 	dup2(tmpin, 0);
// 	dup2(tmpout, 1);
// 	close(tmpin);
// 	close(tmpout);
// 	if (!background){
// 		// Wait for last command
// 		waitpid(ret, NULL);
// 	}
// } // execute
