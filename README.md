# Shell in C
Custom Shell created in C by Jared Teller
11/24/2019

All code for this shell was built on top of the lexical analyzer found in lexical.c

## Compiling the Shell
To compile, be in the root directory of the project where the makefile is.
Make command: `make`
If compiling fails, open and save lex.yy.c and try again!

## Running the Shell
To run the shell type the command: `./lexical`

You will see the prompt:
`%` meaning you can enter commands now. 

Command examples:
`%ls -la > out.txt`</br>
`%grep LINE < in.txt | sort`</br>
`%ls -la >> out.txt`</br>

The following are Invalid Command examples and output an error to stderr:</br>
`% > in.txt | ls` - cannot start command with a redirect</br>
`%ls -la | in.txt` - cannot pipe to a file</br>
`%ls < in.txt < in.txt` - more than one input redirect</br>
