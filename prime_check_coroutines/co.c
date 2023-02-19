#include "co.h"


int mycoroutines_init(co_t* main_thread){
	int result;

	result = getcontext(&(main_thread->current_co));
	//return_co = main_thread->current_co;

	return result;
}

int mycoroutines_create(co_t *co, void* (body)(void *),void *arg){
	int result;

	result = getcontext(&(co->current_co));

	co->stack = (char*)calloc(SIGSTKSZ, sizeof(char));
	co->current_co.uc_link = &return_co;
	co->current_co.uc_stack.ss_sp = co->stack;
	co->current_co.uc_stack.ss_size = SIGSTKSZ*sizeof(char);

	makecontext(&(co->current_co), (void (*)(void)) body, 1, arg);

	return result;
}

int mycoroutines_switchto(co_t *co){
	int result;

	//printf("eftasa2\n");
	result = setcontext(&(co->current_co));
	//printf("eftasa3\n");

	return result;
}

int mycoroutines_destroy(co_t *co){

	free(co->stack);

	return 1;
}
