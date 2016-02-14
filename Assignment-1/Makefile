CC      = gcc
RM      = rm -f

run:   
	./combiner

all: mapper reducer combiner

mapper: mapper.c
	$(CC) -o mapper mapper.c
	
reducer: reducer.c
	$(CC) -o reducer reducer.c
	
combiner: combiner.c
	$(CC) -o combiner combiner.c

clean:
	rm -f core *.o mapper reducer combiner
