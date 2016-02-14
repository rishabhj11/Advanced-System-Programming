Assignment-1 (Course EEL5934)
Rishabh Jain

Language used: C
OS used: Ubuntu (linux)

Files:
1. input.txt - It contains the sample input.
2. mapper.c - Code to map the input strings from input.txt to key 1.
3. reducer.c - Code to reduce the output of mapper by counting the mapped strings, and displays with new keys as total count of each string.
4. combiner.c - Code to fork the mapper and reducer processes.
5. Makefile - Compiles the above files and runs the combiner.

How to run:
1. Compile the file by using the command: make all
2. Run the combiner by using the command: make run

Explaination:
The combiner process(parent) first forks the reducer(child1), which waits for the input, and then forks the mapper(child2). The parent waits for both the children to complete and then exits.

References:
1. Linked lists in C: http://www.cprogramming.com/tutorial/c/lesson15.html

