/*Phase 2: Basic shell
Adaptation of runCommand that operates in REPL mode*/

#include <sys/syscall.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>
#include <iostream>

//helper function to convert timeval structs to numbers of microseconds
unsigned long timeval2long(struct timeval* timeVal){
	return timeVal->tv_sec*1000 + timeVal->tv_usec/1000;
}

int main(int argc, const char* argv[]){


	char* strCommand;
	char* strArgument;
	
	char* buffer = NULL; 	// buffer to hold input lines
	int read;		//status of line reading
	unsigned int len; 	//length of the line read
	
	//Read, Evaluate, Print, Loop.
	while(1){
		read = getline(buffer, &len, stdin);
		strArgument = strtok(*buffer); 
		while(strArgument != NULL){
			argumentsVect.push_back(strArgument);
			strArgument = strtok(NULL, " ");
		}

		int n_args = argumentsVect.size();
		char* arguments[n_args];
		for (int i=0;i<n_args;i++){
			arguments[n] = argumentsVect[i];	
			printf(arguments[i]);
		}

		//debug - print the command and its argument
		//printf("%s\n%s\n",strCommand, strArgument);
		//char* const arguments[] = {strCommand, strArgument, NULL};	// create an array of string arguments, terminated by a NULL pointer.	

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
	}
}
