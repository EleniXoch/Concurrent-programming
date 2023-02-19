CC = gcc
CFLAGS = -Wall -g 
OBJECTS = co.o


main:  main.c co.o
	$(CC) $(CFLAGS) $< $(OBJECTS) -o $@ $(LINKS)

co.o: co.c co.h
	$(CC) $(CFLAGS)  -c $< -o $@ $(LINKS)

clean:
	rm *.o
	rm main

new: clean main