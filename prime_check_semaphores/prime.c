#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "my_sem.h"

struct thread_input{
	volatile int numberToBeChecked; //number to check if it is prime
	volatile int opStatus; // init to -1(available), 0 is result(NO), 1 is result(YES), 2 is working, 3 is for terminate
	volatile int terminate; // init to 0, 1 is terminate
	volatile int semid;
	volatile int semid2;
};

	int misses= 0;

//assuming val is not 1
//returns 1 is val is prime
int isPrime(int val){
	int i, flag = 0;

	for( i = 2; i <= val/2; ++i){
		if(val % i == 0){//not a prime
			flag = 1;
			break;
		}
	}

	//flag is 0 if its a prime
	return(!flag);
}

void* worker_thread(void* args){
	struct thread_input *input_ptr = (struct thread_input*) args;
	unsigned long int self = pthread_self();

	
	while(1){//wait for notification by main
		
		mysem_down(input_ptr->semid);
		//printf("ksekolaw thn main %lu\n", self);
		if(input_ptr->opStatus != 2){
			input_ptr->opStatus = 3;
			//up2
			//printf("kanw up thn main %lu\n", self);
			mysem_up(input_ptr->semid2);
			return NULL;
		}
		//start signal by main
		//notify main that i am available/process the job/done the job
		input_ptr->opStatus = isPrime(input_ptr->numberToBeChecked);
		
		if(input_ptr->opStatus){
			printf("%d is a prime!\n", input_ptr->numberToBeChecked);
		}else{
			printf("%d is NOT a prime!\n", input_ptr->numberToBeChecked);
		}
		if(input_ptr->terminate == 1){
			input_ptr->opStatus = 3;
			//up2
			//printf("kanw up thn main %lu\n", self);
			mysem_up(input_ptr->semid2);
			return NULL;
		}
		//printf("terminate: %d\n", input_ptr->terminate);
	}
}


int main(int argc, char const *argv[]){
	pthread_t * arrayOfThreads ;
	struct thread_input *threadData ;
	int i, j, threads;

	
	if(argc != 2){
		printf("Wrong arguments read README.txt\n");
		exit(-1);
	}

	threads = atoi(argv[1]);

	if (threads <= 0){
		printf("Wrong arguments read README.txt\n");
		exit(-1);
	}
	
	arrayOfThreads = (pthread_t *) malloc(threads*(sizeof(pthread_t)));
	threadData = (struct thread_input *) malloc(threads*(sizeof(struct thread_input)));
	//arxikopoihsh
	for (i = 0; i < threads; i++){
		threadData[i].numberToBeChecked = -1;
		threadData[i].opStatus = -1;
		threadData[i].terminate = 0;
		threadData[i].semid = mysem_create(0);//dhmiourgia shmatoforou gia ka8e worker
		threadData[i].semid2 = mysem_create(0);
	}

	//create workers
	for(i = 0; i < threads; i++){
		pthread_create(&arrayOfThreads[i], NULL, worker_thread, &threadData[i]);
	}


	//jobs
	while(1){
		//read next number to be checked
		scanf(" %d",&i);
		if(i == -1){
			break;
		}
		//wait for a worker to become available
		for(j = 0; j < threads; j++){

			if(threadData[j].opStatus < 2){// -1 initialization,  0: last job is not prime, 1: last job is prime
				break;
			}

			if( j == threads -1){
				j = -1;
			}
		}

		//assign next job to an available worker(j)
		threadData[j].opStatus = 2;//worker is now working
		threadData[j].numberToBeChecked = i;
	
		//notify worker to process the job
		if (mysem_up(threadData[j].semid) == -1){
			misses++;//metrhma xamenwn klhsewn up
		}
	
		//threadData[j].mainSignals = 1; // start the job
	}

	for(j = 0; j < threads; j++){//wait for all to terminate/terminate all workers
		threadData[j].terminate = 1;
		mysem_up(threadData[j].semid);
		//down2
	}
	//printf("TA TERMATIZW\n");
	for(j = 0; j < threads; j++){//wait for all to terminate/terminate all workers
		mysem_down(threadData[j].semid2);
		//down2
	}

	for(j = 0; j < threads; j++){//destroy semaphores
		mysem_destroy(threadData[j].semid);
		mysem_destroy(threadData[j].semid2);
	}

	return 0;
}
