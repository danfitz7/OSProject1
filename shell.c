/*Phase 2: Basic shell
Adaptation of runCommand that operates in REPL mode*/

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

#define BUFSIZE 1000
#define MAXARGS 20
int main(int argc, const char* argv[]){

	char* strCommand;
	char* strArgument;
	
	char buffer[BUFSIZE]; 			// buffer to hold input lines
	char* args_buffer[MAXARGS]; 	//buffer to hold arguments as they are being parsed
	#define delimiters " \n\0"
	printf("Starting shell...");

	//Read, Evaluate, Print, Loop.
	while(1){
		printf("Enter a command and its arguments:");

		fgets(buffer, sizeof(buffer), stdin);
		printf("Received line '%s'\n", buffer);

		char* chompedBuffer = strtok(buffer,"\n");		//chop off trailing newline
		strCommand = strtok(chompedBuffer, delimiters);	//extract the command as the first token (whitespace-seperated word)
		printf("Command is '%s'\n", strCommand);

		int n_args = 0;
		strArgument = strtok(NULL, delimiters); 	//attempt to parse the next token (first argument)
		while (strArgument!=NULL){
			n_args++;

			//make a new string to hold the argument, copy from arguments buffer
			int argument_length = strlen(strArgument);
			printf("\targument '%s' has %d characters\n", strArgument, argument_length);
			args_buffer[n_args] = (char*) malloc(argument_length);
			//memset(args_buffer[n_args], '\0', argument_length);
			strcpy(args_buffer[n_args], strArgument);

			strArgument = strtok(NULL, delimiters);
		}
		printf("Received %d arguments for command %s", n_args, strCommand);

		//make the arguments array
		const char* arguments[n_args+1];
		for (int i=0;i<n_args;i++){
			arguments[i] = args_buffer[i];
		}
		arguments[n_args] = NULL; //last arg is null pointer

		for (int i=0;i<n_args;i++){
			printf("%s\n", arguments[i]);
		}

		/*
		 * for (int i=0;i<n_args;i++){
			//arguments[n] = argumentsVect[i];
			printf(arguments[i]);
		}
		strArgument = strtok(*buffer); 
		while(strArgument != NULL){
			//argumentsVect.push_back(strArgument);
			strArgument = strtok(NULL, " ");
		}

		int n_args = 10;//argumentsVect.size();
		char* arguments[n_args];


		//debug - print the command and its argument
		printf("%s\n",strCommand);
		for (int i=0;i<n_args;i++){
			printf(""\t%s\n",arguments[i]);
		}
		*/


		/*
		struct timeval start;
		struct timeval finish;
		long elapsed;

		int status;
		int childPID = fork();
		if (childPID==0){
			execvp(strCommand, arguments); //we are the child, run the command		
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
		*/
	}
	return 0;
}
