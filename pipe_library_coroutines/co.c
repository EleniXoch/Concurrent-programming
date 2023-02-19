#include "co.h"

extern ucontext_t return_co;
extern volatile struct FIFO fifoPipe;

ucontext_t running_co;

void pipe_init(int size){

	fifoPipe.writeIndex = 0;
	fifoPipe.readIndex = 0;
	fifoPipe.size = size;
	fifoPipe.used = 0;
	fifoPipe.writeFlag = 1; //1 is for writing, 0 is not (exoun meinei kialla gia na grapsw)
	fifoPipe.readFlag = 1;
	fifoPipe.matrixPtr = (char*)calloc(size,sizeof(char));

}

void print_pipe(int size){
	int i;

	printf("\033[0;31m"); 
	printf("---------------PRINTING PIPE!---------------\n");
	printf("\033[0m");
	printf("USED: %d\n", fifoPipe.used);

	for (int i = 0; i < size; ++i){
		printf("%d\n", fifoPipe.matrixPtr[i]);
	}
}

int check_read(){
	return (fifoPipe.readFlag);
}

void pipe_destroy(){

	free(fifoPipe.matrixPtr);

}


void pipe_writeDone(){

	fifoPipe.writeFlag = 0;	

}

int mycoroutines_init(co_t* main){
	int result;

	result = getcontext(&(main->current_co));
	return_co = main->current_co;
	running_co = main->current_co;

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
	ucontext_t* temp;

	temp = &running_co;
	running_co = co->current_co;
	result = swapcontext(temp, &(co->current_co));

	return result;
}

int mycoroutines_destroy(co_t *co){

	free(co->stack);

	return 1;
}

void pipe_write_full(char* c, int size){

	while(size){
		if(*c == -1){
			pipe_writeDone();
			return;
		}
		fifoPipe.matrixPtr[fifoPipe.writeIndex%fifoPipe.size] = *(c++);
		fifoPipe.writeIndex++;
		fifoPipe.used++; 
		size--;
		//printf("pipe_write %d\n", fifoPipe.writeIndex);
	}
}
int pipe_read_full(char* c, int* size){

	while((fifoPipe.readIndex < fifoPipe.writeIndex)){//o agwgos einai adeios
		*(c++) = fifoPipe.matrixPtr[fifoPipe.readIndex%fifoPipe.size];
		//printf("pipe_read %d\n", fifoPipe.readIndex);
		fifoPipe.readIndex++;
		(*size)++;
	}

	if (fifoPipe.writeFlag == 0){//exei klhthei h writeDone
		//ara termatizei h pipe_read
		return 0;
	}
	return 1;
}

