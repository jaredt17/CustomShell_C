#include <stdio.h>
#include "y.tab.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>


enum state { START, PROGNAME, CLA, 	FILENAME,  INPUT_REDIRECT, OUTPUT_REDIRECT_APPEND, OUTPUT_REDIRECT_WRITE, BACKGROUND, STDERR_OVERWRITE, STDERR_APPEND, ERROR, END};
char statestr[12][25] = { "START", "PROGNAME","CLA", 	"FILENAME",  "INPUT_REDIRECT", "OUTPUT_REDIRECT_APPEND", "OUTPUT_REDIRECT_WRITE", "BACKGROUND", "STDERR_OVERWRITE", "STDERR_APPEND", "ERROR", "END" };

// Command Data Structure
// Describes a simple command and arguments
typedef struct SimpleCommand {
	// Available space for arguments currently preallocated
	int _numberOfAvailableArguments;
	// Number of arguments
	int _numberOfArguments;
	// Array of arguments
	char **_arguments;
}SimpleCommand;

// Describes a complete command with the multiple pipes if any
// and input/output redirection if any.
typedef struct Command {
        int _numberOfAvailableSimpleCommands;
        int _numberOfSimpleCommands;
        SimpleCommand ** _simpleCommands;
        char * _outFile;
        char * _inputFile;
        char * _errFile;
        int _background;
		int outputAppend;
		int stdErrAppend;
}Command;

//FUNCTION DEFINITIONS
void insertSimpleCommand( SimpleCommand * simpleCommand, Command * command );
void print();
void execute();
void clear(Command *command);
void insertArgument(SimpleCommand *command, char * argument ); // or global *_currentSimpleCommand instead of parameter
void printAllCommands(Command * command);

Command* initCommand();
SimpleCommand* initSimpleCommand();


//THE TABLE BELOW IS FOR A FINITE STATE MACHINE AS DESCRIBED IN THE FILE: DRAWING
					//  0 			1			2		3		4			5			6		7					8					9
int table[11][10] = { // NoToken", "GREAT", "NEWLINE", "WORD", "GREATGREAT", "PIPE", "LESS", "GREATAPERSAND", "GREATGREATAPERSAND", "AMPERSAND"
					{  	ERROR   , ERROR  ,START  , PROGNAME, ERROR, ERROR, ERROR, ERROR,ERROR,ERROR }, // start

				  	{  	ERROR, OUTPUT_REDIRECT_WRITE, END, CLA, OUTPUT_REDIRECT_APPEND, START, INPUT_REDIRECT, STDERR_OVERWRITE, STDERR_APPEND, BACKGROUND}, //PROGNAME

					{	ERROR, OUTPUT_REDIRECT_WRITE, END, CLA, OUTPUT_REDIRECT_APPEND, START, INPUT_REDIRECT, STDERR_OVERWRITE, STDERR_APPEND, BACKGROUND}, //CLA(WORD)

					{	ERROR, OUTPUT_REDIRECT_WRITE, END, ERROR, OUTPUT_REDIRECT_APPEND, START, INPUT_REDIRECT, STDERR_OVERWRITE, STDERR_APPEND, BACKGROUND}, //FILENAME

					{	ERROR, ERROR, ERROR, FILENAME,ERROR, ERROR, ERROR, ERROR, ERROR, ERROR}, //INPUT_REDIRECT

					{	ERROR, ERROR, ERROR, FILENAME,ERROR, ERROR, ERROR, ERROR, ERROR, ERROR}, //OUTPUT_REDIRECT_APPEND 

					{	ERROR, ERROR, ERROR, FILENAME,ERROR, ERROR, ERROR, ERROR, ERROR, ERROR},//OUTPUT_REDIRECT_WRITE

					{	ERROR, ERROR, END, ERROR,ERROR, ERROR, ERROR, ERROR, ERROR, ERROR},//BACKGROUND
					
					{	ERROR, ERROR, ERROR, FILENAME,ERROR, ERROR, ERROR, ERROR, ERROR, ERROR},//STDERR_OVERWRITE

					{	ERROR, ERROR, ERROR, FILENAME,ERROR, ERROR, ERROR, ERROR, ERROR, ERROR},//STDERR_APPEND

					{  	ERROR   , ERROR  ,PROGNAME  , PROGNAME, ERROR, ERROR, ERROR, ERROR,ERROR,ERROR }}; //ERROR


extern void getNext(enum yytokentype *, char *, int, int *);
#define TEXTSIZE 250

void printToken(enum yytokentype token) {
	static char * tokens[] = {"NoToken", "GREAT", "NEWLINE", "WORD", "GREATGREAT",
	"PIPE", "LESS", "GREATAPERSAND", "GREATGREATAPERSAND", "AMPERSAND" };
	printf("Token %s ", tokens[token-NOTOKEN]);
}

int main(int argc, char * argv[]) { 

	//initialize the global currentsimplecommand struct
    SimpleCommand* curSimpleCommand;
    
    //SET UP COMMAND struct
   	Command* _currentCommand;

	while(1){ //keep running until ctrl C -- need to add exit command
	clear(_currentCommand);
	_currentCommand = initCommand();

	int outputRed = 0;
	int inRed = 0;

	printf("%%");
	
	//set up simple command struct
	//initialize the global currentsimplecommand struct
	
 	curSimpleCommand = initSimpleCommand();
    char text[TEXTSIZE];
	enum yytokentype token = NOTOKEN;
	
	enum state curstate = START;
	enum state prevstate = START;

	char fileText[TEXTSIZE];

	int pos = 0;
	int startpos = 0;
	//as of now running for only one command and then exiting, need overarching while loop
	int textlength;
	while(curstate != END && curstate != ERROR) {
		getNext(&token, text, TEXTSIZE, &textlength);
		//printToken( token );
		//printf("token %d, text >%s< \n", token, text);
		//printf("CURstate %s,  token %d\n", statestr[curstate], token-NOTOKEN);
		curstate = table[curstate][token-NOTOKEN];
		switch (curstate){ //we will have specific actions for each state defined in the table, every time there is a pipe we need to create a simple command
			case START:
				if(token == NEWLINE){
					printf("%%");
				}
				if(prevstate != ERROR){
					//print(_currentSimpleCommand);
					insertSimpleCommand(curSimpleCommand, _currentCommand);
					curSimpleCommand = initSimpleCommand();
				}
				//note we will only get here on starts after the first one
				//print(_currentSimpleCommand);
				//need to clear the simple command for the next one
				break;
			case PROGNAME:
				if(strcmp(text, "quit") == 0 || strcmp(text, "exit") == 0){
					exit(0);
				}
				insertArgument(curSimpleCommand,text);
				break;
			case CLA:
				insertArgument(curSimpleCommand,text);
				break;
			case FILENAME:
				if(prevstate == INPUT_REDIRECT){
					strcpy(fileText, text);
					_currentCommand->_inputFile = fileText;
				}else if(prevstate == OUTPUT_REDIRECT_APPEND){
					strcpy(fileText, text);
					_currentCommand->_outFile = fileText;
				}else if(prevstate == OUTPUT_REDIRECT_WRITE){
					strcpy(fileText, text);
					//printf("%s", fileText);
					_currentCommand->_outFile = fileText;
					//printf("\nFILENAME: %s\n", _currentCommand->_outFile);
				}else if(prevstate == STDERR_APPEND){
					strcpy(fileText, text);
					_currentCommand->_errFile = fileText;
				}else if(prevstate == STDERR_OVERWRITE){
					strcpy(fileText, text);
					_currentCommand->_errFile = fileText;
				}
				break;
			case INPUT_REDIRECT:
				inRed++;
				if(inRed != 1){
					fprintf(stderr, "You can only have one input redirect per command... Try again\n");
					prevstate = ERROR;
					curstate = ERROR;
					break;
				}
				prevstate = INPUT_REDIRECT;
				break;
			case OUTPUT_REDIRECT_APPEND:
				outputRed++;
				if(outputRed != 1){
					fprintf(stderr, "You can only have one output redirect per command... Try again\n");
					prevstate = ERROR;
					curstate = ERROR;
					break;
				}
				prevstate = OUTPUT_REDIRECT_APPEND;
				_currentCommand->outputAppend = 1;
				break;
			case OUTPUT_REDIRECT_WRITE:
				outputRed++;
					if(outputRed != 1){
						fprintf(stderr, "You can only have one output redirect per command... Try again\n");
						prevstate = ERROR;
						curstate = ERROR;
						break;
					}
				prevstate = OUTPUT_REDIRECT_WRITE;
				break;
			case STDERR_OVERWRITE:
				prevstate = STDERR_OVERWRITE;
				break;
			case STDERR_APPEND:
				prevstate = STDERR_APPEND;
				_currentCommand->stdErrAppend = 1;
				break;
			case BACKGROUND:
				_currentCommand->_background = 1;
				break;
			case END:
				if(prevstate != ERROR){
					insertSimpleCommand(curSimpleCommand, _currentCommand);
					curSimpleCommand = initSimpleCommand();

					execute(_currentCommand);
					initCommand(_currentCommand);
				}
				
				break;

			case ERROR:
				prevstate = ERROR;
				fprintf(stderr, "Bad command: Press Enter to continue ...\n");
				break;
			default:
				break;

		}
	}
	//print(curSimpleCommand);
	//printf("\n\n%s\n\n", _currentCommand->_simpleCommands[1]->_arguments[1]);
	//clear(curSimpleCommand); //reset the simple command
	}

	

	//printAllCommands(_currentCommand);
	//execute(_currentCommand);

	//call execute to run the commands we just built
	//execute(numSimpleCommands);
	// TESTING execvp(_currentSimpleCommand->_arguments[0], _currentSimpleCommand->_arguments);
	return 0;
}


Command* initCommand(){
    Command* _currentCommand = malloc(sizeof(SimpleCommand) * 2);
    _currentCommand->_simpleCommands = malloc(sizeof(SimpleCommand) * 50);
    _currentCommand->_numberOfSimpleCommands = 0;
    _currentCommand->_numberOfAvailableSimpleCommands = 50;
    _currentCommand->_inputFile = NULL;
    _currentCommand->_outFile = NULL;
    _currentCommand->_errFile = NULL;
    _currentCommand->outputAppend = 0;
    _currentCommand->_background = 0;
	_currentCommand->stdErrAppend = 0;
    return _currentCommand;
}

SimpleCommand* initSimpleCommand(){
    SimpleCommand* curSimpleCommand = malloc(sizeof(SimpleCommand)*50);
    curSimpleCommand->_arguments = malloc(sizeof(char*)*50);
    curSimpleCommand->_numberOfArguments = 0;
    curSimpleCommand->_numberOfAvailableArguments = 50;
    return curSimpleCommand;
}


//INSERTS A SIMPLE COMMAND INTO THE ARRAY INSIDE COMMANDS
void insertSimpleCommand(SimpleCommand* curSimpleCommand, Command* _currentCommand ){
    if(_currentCommand->_numberOfSimpleCommands == _currentCommand->_numberOfAvailableSimpleCommands){
        _currentCommand->_numberOfAvailableSimpleCommands *= 2;
         _currentCommand->_simpleCommands = realloc(_currentCommand->_simpleCommands, sizeof(*curSimpleCommand)* _currentCommand->_numberOfAvailableSimpleCommands);
    }
   
    _currentCommand->_simpleCommands[_currentCommand->_numberOfSimpleCommands] = curSimpleCommand;
    _currentCommand->_numberOfSimpleCommands++; //increase the number of simple commands in the commands
}

void printAllCommands(Command * command){ //dont run this it was just used for testing a specific element in commands
	//for(int i = 0; i < command->_numberOfSimpleCommands; i++){
		printf("\n\n%s\n\n", command->_simpleCommands[1]->_arguments[2]);
		
	//}
}

//insert argument into simple command
void insertArgument(SimpleCommand* curSimpleCommand, char * argument ) {
   //allocate space for the new command
	curSimpleCommand->_arguments[curSimpleCommand->_numberOfArguments] = (char*)realloc(curSimpleCommand->_arguments[curSimpleCommand->_numberOfArguments], sizeof(char*) * strlen(argument));
	strcpy(curSimpleCommand->_arguments[curSimpleCommand->_numberOfArguments], argument);
	curSimpleCommand->_arguments[curSimpleCommand->_numberOfArguments+1] = NULL;
	curSimpleCommand->_numberOfArguments++;
}

void print(SimpleCommand *command){
	printf("\n___SIMPLE COMMAND____\n");
	for(int i = 0; i < command->_numberOfArguments; i++){ //for the number of commands
		printf("Arg %d: %s\n", i, command->_arguments[i]);
	}
	printf("___END____\n");
}

void clear(Command *command){
	free(command);
}

void execute(Command * command){
	//printf("\n__AFTER EXECUTING__\n");
	//printf("\n\nNUMBER OF SIMPLE COMMANDS IN COMMAND %d\n\n", command->_numberOfSimpleCommands);
	//save in/out
	int tmpin = dup(0);
	int tmpout = dup(1);
	int tmperr = dup(2);

	//set the initial input
	int fdin;

	//set up initial error
	int fderr;
	if(command->_errFile != NULL){
		if(command->stdErrAppend == 1){
			fderr = open(command->_errFile, O_RDWR | O_APPEND);
		}else{
			fderr = open(command->_errFile, O_RDWR | O_CREAT | O_TRUNC, 0666);
		}
	}
	else{
		//use default error
		fderr = dup(tmperr);
	} //err file now set up for either append or create mode

	if (command->_inputFile != NULL){
		fdin = open(command->_inputFile, O_RDWR);
	}
	else{

		// Use default input
		fdin = dup(tmpin);
	}

	pid_t ret;
	int fdout;
	for (int i = 0; i < command->_numberOfSimpleCommands; i++){

		//printf("\n\nTEST: %d	%s\n\n", i, command->_simpleCommands[i]->_arguments[0]);

		//redirect input
		dup2(fdin, 0);
		close(fdin);

		//redirect to err file
		dup2(fderr, 2);
		close(fderr);

		//setup output
		if (i == command->_numberOfSimpleCommands - 1){

			// Last simple command
			if (command->_outFile){
				if(command->outputAppend == 1){
					fdout = open(command->_outFile,  O_RDWR | O_APPEND | O_CREAT, 0666);
					//printf("\nOUTFILE NAME APPEND: %s\n", command->_outFile);
				}else{ //overwrite mode
					fdout = open(command->_outFile,  O_RDWR | O_CREAT | O_TRUNC, 0666);
					//printf("\nOUTFILE NAME: %s\n", command->_outFile);
				}
				
				
			}
			else{
				// Use default output
				fdout = dup(tmpout);
			}
		}

		else{
			// Not last
			//simple command
			//create pipe
			int fdpipe[2];
			pipe(fdpipe);
			fdout = fdpipe[1];
			fdin = fdpipe[0];
		} // if/else

		// Redirect output
		dup2(fdout, 1);
		close(fdout);

		// Create child process
		ret = fork();
		if (ret == 0){
			execvp(command->_simpleCommands[i]->_arguments[0], command->_simpleCommands[i]->_arguments);
			perror("Execvp:");
			_exit(1);
		}
	} //  for

	//restore in/out defaults
	dup2(tmpin, 0);
	dup2(tmpout, 1);
	dup2(tmperr, 2);
	close(tmperr);
	close(tmpin);
	close(tmpout);
	if (!command->_background){
		// Wait for last command
		waitpid(ret, NULL, 0);
	}
} // execute
