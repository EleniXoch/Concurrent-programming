#define _GNU_SOURCE

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "co.h"
#define SIZE 64

extern void pipe_init(int size);
extern void pipe_write(char c);
extern int pipe_read(char* c);
extern void pipe_destroy();
extern void pipe_writeDone();
extern void print_pipe(int size);
extern int check_read();

int readDone = 0;
co_t writer;
co_t reader;
co_t main_co;

ucontext_t return_co;
volatile struct FIFO fifoPipe;


struct input_read{
	char inputFile[10];
	int size;
};

volatile int i;

void* read_thread(void* args){
	pthread_t self = pthread_self();
	char* outputFile;
	FILE** fd = (FILE**) args;
	char bytesRead[SIZE + 1] = {'\0'};
	int check;
	int size = 0;

	while(1){
		size = 0;
		check = pipe_read_full(bytesRead, &size);
		if (check == 0){
			//printf("diavasa %s me size %d\n", bytesRead, size);
			fwrite(bytesRead, sizeof(char), size, *fd);
			fflush(*fd);
			readDone = 1;
			break;
		}
		//printf("diavasa %s\n", bytesRead);
		fwrite(bytesRead, sizeof(char), size, *fd);
		mycoroutines_switchto(&writer);
	}
	mycoroutines_switchto(&main_co);
	return NULL;
}

void* write_thread(void* args){
	pthread_t self = pthread_self();
	FILE** fd = (FILE**) args;
	int i = 0;
	int count = 0;
	char bytesToBeWritten[SIZE + 1] = {'\0'};

	while(1){
		while(count < SIZE){
			if(feof(*fd)){
				//printf("bghka\n");
				break;
			}
			bytesToBeWritten[i++] = fgetc(*fd);
			//printf("%d\n", bytesToBeWritten[i-1]);

			if (bytesToBeWritten[i-1] == -1){
				break;
			}
			//printf("%d\n", byteToBeWritten);
			count++;
		}
		//printf("writer: phra apo to arxeio %s me count %d\n",bytesToBeWritten, count);
		pipe_write_full(bytesToBeWritten, count);

		if(count < SIZE){
			//printf("writer: teleiwsa to diavasma apo to arxeio, writeDone = 1\n");
			pipe_writeDone();
		}

		count = 0;
		mycoroutines_switchto(&reader);
	}
	
	return NULL;
}



int main(int argc, char const *argv[]){
	struct input_read input;
	pthread_t t1, t2;
	char outputFile[10];
	int res1, res2;
	int check;
	FILE* fdw, *fdr;
	

	strcpy(input.inputFile, "abc.txt");
	input.size = SIZE;
	strcpy(outputFile, "out.txt");


	pipe_init(input.size);

	fdw = fopen(input.inputFile,"r+");
	rewind(fdw);
	//printf("%c\n", fgetc(fdw));
	rewind(fdw);
	mycoroutines_create(&writer, write_thread, &fdw);
	fdr = fopen(outputFile,"w+");
	mycoroutines_create(&reader, read_thread, &fdr);
	
	mycoroutines_init(&main_co);

	if(readDone == 0){
		mycoroutines_switchto(&writer);
	}
	printf("final\n");
	//print_pipe(28);
	pipe_destroy();
	return 0;
}
