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

int mycoroutines_init(co_t *main);
int mycoroutines_create(co_t *co, void* (body)(void *),void *arg);
int mycoroutines_switchto(co_t *co);
int mycoroutines_destroy(co_t *co);




ucontext_t return_co;


#endif
