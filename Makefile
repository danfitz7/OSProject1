#Start of the Makefile
runCommand: shell.o runCommand.o
	gcc -std=c99 -o runCommand runCommand.o -I.
	gcc -std=c99 -o shell shell.o -I.

runCommand.o: runCommand.c
	gcc -std=c99 -Wall -c runCommand.c -I.

shell.o: shell.c
	gcc -std=c99 -Wall -c shell.c -I.

clean:
	rm *.o

##End of the Makefile

