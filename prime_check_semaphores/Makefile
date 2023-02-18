CC = gcc
CFLAGS = -Wall -g 
OBJECTS = my_sem.o


prime:  prime.c lib
	$(CC) $(CFLAGS) $<  -o $@ $(LINKS) -lpthread -lmy_sem -L. -static
	
lib: my_sem.o my_sem.h
	ar rcs libmy_sem.a $<

my_sem.o: my_sem.c my_sem.h
	$(CC) $(CFLAGS)  -c $< -o $@ $(LINKS)

clean:
	rm *.o
	rm main
	rm *.a

new: clean main
