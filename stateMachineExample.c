#include <stdio.h>
enum token {B, O, OTHER, EOL};
enum state { START, B1, O1, B2, END, ERROR};
// string arrays for debugging statements so we can watch
char tokenstring[4][6] = { "B", "O", "OTHER", "EOL" };
char statestr[6][6] = { "START", "B1", "B2", "O1", "END", "ERROR" };

					// B,   O,     Other, eol 
int table[3][4] = { 
					{  B1, START, START, ERROR }, // start
				  	{ START,  O1,   START, ERROR }, // B1
				  	{ END,   START, START, ERROR }
				};// O1

enum token getTok(char ch) {
	enum token retval = OTHER;
	if      (ch == 'b' || ch == 'B') retval = B;
	else if (ch == 'o' || ch == 'O') retval = O;
	else if (ch == '\n' || ch == 'r' || ch == EOF || ch == '\0') retval = EOL;
	return retval;
}

int main(int argc, char * argv[]) {
	char input[250];
	int pos = 0;
	int startpos = 0;
	enum state curstate = START;
	enum tok token;

	printf("enter string: ");
	fgets(input, 250, stdin);
	printf("read >%s<\n", input);
	// get new state
	while (curstate != ERROR && curstate != END) {
		
		token = getTok(input[pos]);
		printf("state %s,  token %s\n", statestr[curstate],tokenstring[ token]);
		
		curstate = table[curstate][token];
		// curstate = table[curstate][getTok(input[pos])];
		printf("new state %s\n", statestr[curstate]);
		switch (curstate) {
		case ERROR :
			printf("\nbob not found\n"); 
			break;
		case B1: 
			startpos = pos; // pos start of bob 
			break;
		case END: 
			printf("\nbob found at pos %d\n", startpos); 
			break;
		default: 
				break;
		} // end switch
		pos++;
	} // end while	
	
	printf("press key");
	getchar();
	return 0;
}