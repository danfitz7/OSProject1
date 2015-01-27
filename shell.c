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

struct rusage prev_cumulative_rusage = {0}; //calloc(sizeof (struct rusage), 0); the usage cumulative statistics for all previous child processes (init to all 0s)
struct rusage rusage_sub(struct rusage current_usage){
	// subtract the (relivent) previous cumulative usages from the current cumulative usages to get the usage attributed to the mst recent child process
	current_usage.ru_utime.tv_sec -= prev_cumulative_rusage.ru_utime.tv_sec;
	current_usage.ru_utime.tv_usec -= prev_cumulative_rusage.ru_utime.tv_usec;
	current_usage.ru_stime.tv_sec -= prev_cumulative_rusage.ru_stime.tv_sec;
	current_usage.ru_stime.tv_usec -= prev_cumulative_rusage.ru_stime.tv_usec;
	current_usage.ru_nivcsw -= prev_cumulative_rusage.ru_nivcsw;
	current_usage.ru_nvcsw -= prev_cumulative_rusage.ru_nvcsw;
	current_usage.ru_minflt -= prev_cumulative_rusage.ru_minflt;
	current_usage.ru_majflt -= prev_cumulative_rusage.ru_majflt;

	//reset the cumulative usages
	getrusage(RUSAGE_CHILDREN, &prev_cumulative_rusage);

	//return the usage for the most recent child process
	return current_usage;
}

// Execute the given command with the given arguments and print its statistics.
void executeShellCommand(const char* strCommand, char* const arguments[]){
	//store child process statistics
	struct timeval start;
	struct timeval finish;
	long elapsed;

	// keep track of our children
	int status;
	int childExecutionFailed = 0;
	int* p_childExecutionFailed = &childExecutionFailed;

	int childPID = fork();
	if (childPID==0){
		printf("Executing '%s'\n",strCommand);
		int error = execvp(strCommand, arguments); //we are the child, run the command
		if (error == -1){
			printf("execution failed\n");
			*p_childExecutionFailed = 1;
		}
	}else{
		if (!childExecutionFailed){
			gettimeofday(&start, NULL);
			int pid = wait(&status);//we are the parent, monitor the child
			if (pid != -1){
				gettimeofday(&finish, NULL);
				elapsed = (finish.tv_sec-start.tv_sec)*1000 + (finish.tv_usec-start.tv_usec)/1000;

				struct rusage current_usage;
				/*int usage = */getrusage(RUSAGE_CHILDREN, &current_usage);
				current_usage = rusage_sub(current_usage);

				long uTimeMs = timeval2long(&(current_usage.ru_utime));
				long sTimeMs = timeval2long(&(current_usage.ru_stime));

				printf("\nUsage:\nElapsed Wall Clock Time: %lu millisecond(s)\n", elapsed);
				printf("User CPU Time: %lu, System CPU Time: %lu\n", uTimeMs, sTimeMs);
				printf("Involuntary Preemptions: %lu\n", current_usage.ru_nivcsw);
				printf("Voluntary CPU Switches: %lu\n", current_usage.ru_nvcsw);
				printf("Soft Page Faults: %lu\n", current_usage.ru_minflt);
				printf("Hard Page Faults: %lu\n", current_usage.ru_majflt);
				return;
			}
		}
	}
	printf("Not printing statistics for failed execution.\n");
}

// Change the working directory of the shell to the given path.
void changeDirectory(char* strPath){
	if (strPath!=NULL){
		printf("Changing working directory to '%s'\n", strPath);
		if (chdir(strPath) !=0){
			printf("ERROR: bad path: '%s'.", strPath);
		}
	}else{
		printf("ERROR: no new directory given.\n");
	}
}

int exitShell(){
	printf("\nExiting shell...\n");
	exit(0);
	return 0;
}

#define MAX_LINE_LENGTH 128
char buffer[MAX_LINE_LENGTH+2]; 	// Buffer to hold input line characters, plus an extra, plus a NULL termination
char* promptForInput(){
	printf("\nEnter a command and its arguments:");

	// read the next line from standard input, store in the input buffer.
	return fgets(buffer, sizeof(buffer)+1, stdin);
}

#define MAX_ARGS 32
int main(int argc, const char* argv[]){

	#define delimiters " \n\0"	// Separate arguments with these delimiters
	char* strCommand;			// The command string is extracted from stdin every loop
	char* strArgument;			// Each argument to the command is extracted here before being copied to the arguments array
	char* arguments[MAX_ARGS+1]; // Buffer to hold arguments as they are being parsed

	printf("Starting shell...\n");

	//Read, Evaluate, Print, Loop.
	while((promptForInput())){

		if (strlen(buffer)>MAX_LINE_LENGTH+1){
			printf("ERROR: maximum input length exceeded (%d chars).\n", strlen(buffer)-1);

			//clear the rest of the input buffer
			int c;
			while ((c = fgetc(stdin)) != EOF && c != '\n'); //http://stackoverflow.com/questions/7898215/how-to-clear-input-buffer-in-c
		}else{
	//		printf("Received line '%s'\n", buffer);

			// extract the command
			char* chompedBuffer = strtok(buffer,"\n");		//chop off trailing newline
			strCommand = strtok(chompedBuffer, delimiters);	//extract the command as the first token (whitespace-seperated word)
//			printf("Command is '%s' (%d characters)\n", strCommand, MAX_LINE_LENGTH);

			// detect shell exit
			if (strcmp(strCommand, "exit") == 0){
				exitShell();
			}

			// get the first argument (if there is one)
			strArgument = strtok(NULL, delimiters); 	//attempt to parse the next token (first argument)

			//catch shell change directory, give it the path
			if (strcmp(strCommand, "cd") == 0){
				changeDirectory(strArgument);
			}else{

				// loop through input line, extracting and counting arguments
				int max_arg_i = 0;	//number of arguments parsed so far
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
//					printf("\tArgument %d:'%s' has %d characters\n", max_arg_i, strArgument, argument_length);
					arguments[max_arg_i] = (char*) malloc(argument_length+1);
					strcpy(arguments[max_arg_i], strArgument);
					strArgument = strtok(NULL, delimiters);
					max_arg_i++;
				}
				if (!MAX_ARGS_Error){
//					printf("Received %d arguments for command %s\n", max_arg_i, strCommand);

					//debug print
					/*printf("Arguments buffer is...\n");
					for (int i=0;i<max_arg_i;i++){
						printf("\t%s\n", arguments[i]);
					}*/

					arguments[max_arg_i] = NULL; //last arg is null pointer
					executeShellCommand(strCommand, arguments);

					//free memory from the arguments
					for (int i=0;i<max_arg_i;i++){
						free(arguments[i]);
					}
				}
			}
		}
	}
	exitShell();
}
