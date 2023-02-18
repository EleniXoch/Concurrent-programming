#define _GNU_SOURCE

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

extern void pipe_init(int size);
extern void pipe_write(char c);
extern int pipe_read(char* c);
extern void pipe_destroy();
extern void pipe_writeDone();

int readDone = 0;

struct input_read{
	char inputFile[30];
	int size;
};


void* read_thread(void* args){
	char* outputFile;
	FILE* fd;
	char byteRead;
	int check;

	outputFile = (char *)args;

	fd = fopen(outputFile,"w+");
	if (fd == NULL){
		printf("ERROR: No output file!\n");
		exit(-1);
	}

	while(1){

		//kathe fora diavazei ena byte apo ton agwgo
		//kai to grafei ston fd (dhladh to output file mas)

		check = pipe_read(&byteRead);
		if (check == 0){
			//an to diavasma teleiwse epituxws kanouem to readDone = 1 gia an termatisei h main
			readDone = 1;
			break;
		}
		fputc(byteRead,fd);
		//printf("egrapsa sto arxeio %d\n",byteRead);
	}

	return 0;
}

void* write_thread(void* args){
	struct input_read input;
	FILE* fd;
	char byteToBeWritten;

	input = *(struct input_read *) args;

	fd = fopen(input.inputFile,"r");
	if (fd == NULL){
		printf("ERROR: No input file!\n");
		exit(-1);
	}

	while(1){
		
		//kathe fora pairnei ena byte apo to input file kai to grafei ston agwgo
		//mexri na synathsei to EOF
		byteToBeWritten = fgetc(fd);
		/*if (byteToBeWritten == -1){
			break;
		}*/
		//printf("diavasa apo arxeio %d\n",byteToBeWritten);
		if(feof(fd)){
			pipe_writeDone();
			break;
		}
		pipe_write(byteToBeWritten);
		
		
	}

	return 0;
}


/******main*****/
//thewroume san paradeigma inoutfile to diagram_LEDdisplay.pdf kai san output out.pdf
//epishs dhlwnoume rhta to size tou agwgou mesa sthn main

int main(int argc, char const *argv[]){
	struct input_read input;
	pthread_t t1, t2;
	char outputFile[10];
	int check1, check2;

	strcpy(input.inputFile, "diagram_LEDdisplay.pdf");
	strcpy(outputFile, "out.pdf");
	
	input.size = 64;
	pipe_init(input.size);

	check2 = pthread_create(&t2, NULL, write_thread, &input);
	if(check2 != 0){
		printf("ERROR: pthread_create failed!\n");
		exit(-1);
	}

	check1 = pthread_create(&t1, NULL, read_thread, outputFile);
	if(check1 != 0){
		printf("ERROR: pthread_create failed!\n");
		exit(-1);
	}

	

	while(readDone == 0){
		//perimenoume na teleiwsei to diavasma apo ton agwgo
		sched_yield();
	}

	pipe_destroy();
	
	return 0;
}
