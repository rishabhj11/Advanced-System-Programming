CC      = gcc
RM      = rm -f

default: wordstatistics

wordstatistics: wordstatistics.c
	$(CC) -pthread -o wordstatistics wordstatistics.c
	
clean:
	rm -f core *.o wordstatistics
