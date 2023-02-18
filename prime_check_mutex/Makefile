CC = gcc
CFLAGS = -Wall -g 

prime:  prime.c 
	$(CC) $(CFLAGS) $<  -o $@ $(LINKS) -lpthread 

clean:
	rm prime

new: clean prime
