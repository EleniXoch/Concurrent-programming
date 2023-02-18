CC = gcc
CFLAGS = -Wall -g 
OBJECTS = pipe.o
LINKS = -pthread


main:  main.c lib
	$(CC) $(CFLAGS) $<  -o $@ $(LINKS) -lpipe -L.
	
lib: pipe.o
	ar rcs libpipe.a pipe.o

pipe.o: pipe.c pipe.h
	$(CC) $(CFLAGS)  -c $< -o $@ $(LINKS)

clean:
	rm *.o
	rm main
	rm *.a

new: clean main