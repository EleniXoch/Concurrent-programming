#ifndef MYTHREADS_H_
#define MYTHREADS_H_
#include <stdio.h>
#include <ucontext.h>
//#include "co.h"


// extern struct semaphore ;
// typedef struct semaphore sem_t;

//arxh listas
typedef struct thread_list_node{
	struct thread* data;
	struct thread_list_node* next;
}list_node_t;

typedef struct thread_list{
	struct thread_list_node* head;
	struct thread_list_node* tail;
}list_t;
//telos listas

typedef struct semaphore {
	int semid;
	volatile int val;
	list_t* semlist;
}sem_t;

typedef struct co{
	ucontext_t current_co;
	char* stack;
}co_t;

typedef struct thread{
	co_t context;
	int id;
	int state; //-1 dead, 0 running, 1.2.3 waiting for that thread 
	int sem_block;
}thr_t;

//ucontext_t return_co;

int mythreads_sem_destroy(sem_t *s);
int mythreads_sem_up(sem_t *s);
int mythreads_sem_down(sem_t *s);
int mythreads_sem_init(sem_t *s, int val);
int mythreads_yield();
int mythreads_join(thr_t *thr);
int mythreads_init();
int mythreads_self();
int mythreads_create(thr_t *thr, void *(*body)(void *),void *arg);
extern int mycoroutines_init(co_t *main);
extern int mycoroutines_create(co_t *co, void* (body)(void *),void *arg);
extern int mycoroutines_switchto(co_t *co);
extern int mycoroutines_destroy(co_t *co);

#endif
