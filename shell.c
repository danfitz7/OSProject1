/*Phase 2: Basic shell
Adaptation of runCommand that operates in REPL mode
*/

#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>
#include <string.h>

//helper function to convert timeval structs to numbers of microseconds
unsigned long timeval2long(struct timeval* timeVal){
	return timeVal->tv_sec*1000 + timeVal->tv_usec/1000;
}

// execute the given command with the given arguments
void executeShellCommand(const char* strCommand, const char*   arguments[]){
	//store child process statistics
	struct timeval start;
	struct timeval finish;
	long elapsed;

	int status;
	int childPID = fork();
	if (childPID==0){
		execvp(strCommand, (char**) arguments); //we are the child, run the command
	}else{

		gettimeofday(&start, NULL);
		int pid = wait(&status);//we are the parent, monitor the child
		gettimeofday(&finish, NULL);
		elapsed = (finish.tv_sec-start.tv_sec)*1000 + (finish.tv_usec-start.tv_usec)/1000;

		struct rusage rusageStruct;
		int usage = getrusage(RUSAGE_CHILDREN, &rusageStruct);
		long uTimeMs = timeval2long(&(rusageStruct.ru_utime));
		long sTimeMs = timeval2long(&(rusageStruct.ru_stime));
		printf("Usage:\nElapsed Wall Clock Time: %lu millisecond(s)\n", elapsed);
		printf("User CPU Time: %lu, System CPU Time: %lu\n", uTimeMs, sTimeMs);
		printf("Involuntary Preemptions: %lu\n", rusageStruct.ru_nivcsw);
		printf("Voluntary CPU Switches: %lu\n", rusageStruct.ru_nvcsw);
		printf("Soft Page Faults: %lu\n", rusageStruct.ru_minflt);
		printf("Hard Page Faults: %lu\n", rusageStruct.ru_majflt);
	}
}

#define MAX_LINE_LENGTH 128
#define MAX_ARGS 32
int main(int argc, const char* argv[]){

	#define delimiters " \n\0"	// Separate arguments with these delimiters
	char buffer[MAX_LINE_LENGTH+2]; 	// Buffer to hold input line characters, plus an extra, plus a NULL termination
	char* strCommand;			// The command string is extracted from stdin every loop
	char* strArgument;			// Each argument to the command is extracted here before being copied to the arguments array
	char* arguments[MAX_ARGS+1]; // Buffer to hold arguments as they are being parsed

	printf("Starting shell...\n");

	//Read, Evaluate, Print, Loop.
	while(1){
		printf("Enter a command and its arguments:");

		// read the next line from standard input, store in the input buffer.
		fgets(buffer, sizeof(buffer)+1, stdin);
		if (strlen(buffer)>MAX_LINE_LENGTH+1){
			printf("ERROR: maximum input length exceeded (%d chars).\n", strlen(buffer)-1);

			//clear the rest of the input buffer
			//fflush(stdin);
			int c;
			while ((c = fgetc(stdin)) != EOF && c != '\n'); //http://stackoverflow.com/questions/7898215/how-to-clear-input-buffer-in-c
		}else{
	//		printf("Received line '%s'\n", buffer);

			// extract the command
			char* chompedBuffer = strtok(buffer,"\n");		//chop off trailing newline
			strCommand = strtok(chompedBuffer, delimiters);	//extract the command as the first token (whitespace-seperated word)
			printf("Command is '%s' (%d characters)\n", strCommand, MAX_LINE_LENGTH);

			// loop through input line, extracting and counting arguments
			int max_arg_i = 0;	//number of arguments parsed so far
			strArgument = strtok(NULL, delimiters); 	//attempt to parse the next token (first argument)
			int MAX_ARGS_Error = 0;
			while (strArgument!=NULL){
	//			printf("Next token...\n");

				if (max_arg_i+1 > MAX_ARGS){
					printf("ERROR: Maximum number of arguments exceeded.\n");
					MAX_ARGS_Error = 1;
					break;
				}

				//make a new string to hold the argument, copy from arguments buffer
				size_t argument_length = strlen(strArgument); //chompedBuffer-strArgument
				printf("\tArgument %d:'%s' has %d characters\n", max_arg_i, strArgument, argument_length);
				arguments[max_arg_i] = (char*) malloc(argument_length+1);
				strcpy(arguments[max_arg_i], strArgument);
				strArgument = strtok(NULL, delimiters);
				max_arg_i++;
			}
			if (!MAX_ARGS_Error){
				printf("Received %d arguments for command %s\n", max_arg_i, strCommand);

				//debug print
				printf("Arguments buffer is...\n");
				for (int i=0;i<max_arg_i;i++){
					printf("%s\n", arguments[i]);
				}

				arguments[max_arg_i+1] = NULL; //last arg is null pointer
				//executeShellCommand(strCommand, arguments);

				//free memory from the arguments
				for (int i=0;i<max_arg_i;i++){
					free(arguments[i]);
				}
			}
		}
	}
	return 0;
}
