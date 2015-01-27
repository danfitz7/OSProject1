#Start of the Makefile
all: runCommand.o  shell.o shell2.o
	gcc -std=c99 -o runCommand runCommand.o -I.
	gcc -std=c99 -o shell shell.o -I.
	gcc -std=c99 -o shell2 shell2.o -I.
	

runCommand.o: runCommand.c
	gcc -std=c99 -Wall -c runCommand.c -I.

shell.o: shell.c
	gcc -std=c99 -Wall -c shell.c -I.

shell2.o: shell2.c
	gcc -std=c99 -Wall -c shell2.c -I.

clean:
	rm *.o

##End of the Makefile

