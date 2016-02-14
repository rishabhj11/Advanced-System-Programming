EEL- 5934 ADVANCED SYSTEM PROGRAMMING
ASSIGNMENT-2
NAME: RISHABH JAIN

Setup:
The zip contains the code file- wordstatistics.c and the make file-Makefile, in order to compile the
code. A text file wordCount.txt is created on successful run of the program.

Steps to follow:
> To compile: $ make
> To run: $ ./wordstatistics <name of input file> <number of mappers> <number of reducers>
For Example: ./wordstatistics input.txt 10 20
> To clean: $ make clean

Details of implementation:
The code creates the following threads:
> map_pool_updater - 1
> mapper - multiple
> reducer - multiple
> word_count_writer -1

To achieve synchronization, semaphores and mutexes are used between producers and consumers.

For example:
>At Producer's End:
sem_wait(&empty);
pthread_mutex_lock(&mutex);
//CRITICAL SECTION
pthread_mutex_unlock(&mutex);
sem_post(&full);

>At Consumer's End:
sem_wait(&full);
pthread_mutex_lock(&mutex);
//CRITICAL SECTION
pthread_mutex_unlock(&mutex);
sem_post(&empty);

> At the end of the program, all the threads exit themselves after all the operations are complete.