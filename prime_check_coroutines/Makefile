CC = gcc
CFLAGS = -Wall -g 
OBJECTS = mythreads.o co.o


main:  main.c mythreads.o co.o
	$(CC) $(CFLAGS) $< $(OBJECTS) -o $@ $(LINKS)
prime: prime.c mythreads.o co.o 
	$(CC) $(CFLAGS) $< $(OBJECTS) -o $@ $(LINKS)
mythreads.o: mythreads.c mythreads.h co.o
	$(CC) $(CFLAGS)  -c $< -o $@ $(LINKS)

co.o: co.c co.h
	$(CC) $(CFLAGS)  -c $< -o $@ $(LINKS)

clean:
	rm *.o
	rm main

new: clean main