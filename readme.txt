Daniel Fitzgerald
A.J. Tamburri
Project 1 Checkpoint

Phase 1:
We have tested our runCommand executable by passing it a call to ls /home.
It prints out the command it recognizes, the output of the command, and the 
required usage data for the command.

Phase 2:
Shell is able to accept commands, change the working directory, and exit correctly. 

Phase 3:
Shell2 augments shell with a new function, executeBackgroundCommand().
This is similar to the original executeShellCommand() function, but calls a helper function to wait for background processes to finish in a loop.
The helper function is waitForChildrenToFinish(int hang), where the "hang" flag indicates if the wait() call in the loop should hang or not.
This function is also called before and after executing normal non-background commands, and called when the shell exits (with hanging enabled to wait for all children to finish before exiting the shell).

When a new background process is created, its pid, command, and starttime are recorded in a data structure, and this process record is added to a list of currently running background processes.
This list is currently implemented as a fixed-size array. A linked-list or stack may be better since its size could increase, but there was insufficient time to implement more advanced data structures.
When the "jobs" command is encountered, the information about the currently running background processes is printed. When a background process exits, it should be removed from this list and the memory for it's data structure should be freed.
However, it was difficult to tell in the waitForChildrenToFinish() loop if a process had exited or not, so this was not completed. In addition, it is unclear if the statistics printed for background processes are compute correctly using the cumulative statistics.
The process datastucture could be augmented to store the cumulative statistics at the time the process was greated, then these could be subtracted from subsiquent statistics until the process finishes. Whatever is left over belonged to that process. 

testInput.txt is for shell, demonstrating basic commands with 0, 1 or multiple arguments, "cd", and "exit".
TestInput2 is for shell2 demonstrating background commands ut of order with "jobs"