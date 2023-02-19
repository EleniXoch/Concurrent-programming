#ifndef __PIPE_H
#define __PIPE_H

#define _GNU_SOURCE

#include <stdlib.h>
//#include <pthread.h>
#include <ucontext.h>
#include <stdio.h>
#include <signal.h>

typedef struct co{
	ucontext_t current_co;
	char* stack;
}co_t;

void pipe_init(int size);
//void pipe_write(char c);
//int pipe_read(char* c);
void pipe_write_full(char* c, int size);
int pipe_read_full(char* c, int* size);
void pipe_destroy();
void pipe_writeDone();
int mycoroutines_init(co_t *main);
int mycoroutines_create(co_t *co, void* (body)(void *),void *arg);
int mycoroutines_switchto(co_t *co);
int mycoroutines_destroy(co_t *co);

struct FIFO{

	int writeIndex;
	int readIndex;
	int size;
	int used;
	int writeFlag;
	int readFlag;
	char* matrixPtr;

};

//ucontext_t return_co;

//volatile struct FIFO fifoPipe;

#endif
