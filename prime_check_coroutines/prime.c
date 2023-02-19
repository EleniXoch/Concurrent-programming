#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "mythreads.h"

struct thread_input{
	volatile int numberToBeChecked; //number to check if it is prime
	volatile int opStatus; // init to -1(available), 0 is result(NO), 1 is result(YES), 2 is working, 3 is for terminate
	volatile int terminate; // init to 0, 1 is terminate
	sem_t semid;
	sem_t semid2;
};

	int misses= 0;

sem_t available;

// assuming val is not 1
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
	int self = mythreads_self();
	int temp;
	
	while(1){
		//wait for wake up by main
		mythreads_sem_down(&input_ptr->semid);
		//printf("Worker  %d: ksekolaw apo thn main\n", self);
		//main has signaled me to terminate, my previous job has finished
		//main woke me up without a new job so i will terminate
		if(input_ptr->opStatus != 2){
			input_ptr->opStatus = 3;
			//printf("Worker  %d: kanw up thn main\n", self);
			mythreads_sem_up(&input_ptr->semid2);
			return NULL;
		}
		//notify main that i am available/process the job/done the job
		temp = isPrime(input_ptr->numberToBeChecked);
		
		if(temp){
			printf("----------------------%d is a prime!\n", input_ptr->numberToBeChecked);
		}else{
			printf("----------------------%d is NOT a prime!\n", input_ptr->numberToBeChecked);
		}
		//main has signaled me to terminate
		//the signal came while i was working
		if(input_ptr->terminate == 1){
			input_ptr->opStatus = 3;
			//printf("Worker %d: kanw up thn main\n", self);
			mythreads_sem_up(&input_ptr->semid2);
			//printf("Worker %d: terminate\n", self);
			return NULL;
		}
		
		input_ptr->opStatus = temp;
		mythreads_sem_up(&available);
	}
}


int main(int argc, char const *argv[]){
	thr_t* arrayOfThreads ;
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
	
	arrayOfThreads = (thr_t *) malloc(threads*(sizeof(thr_t)));
	threadData = (struct thread_input *) malloc(threads*(sizeof(struct thread_input)));
	//arxikopoihsh
	mythreads_init();
	mythreads_sem_init(&available, threads);
	for (i = 0; i < threads; i++){
		threadData[i].numberToBeChecked = -1;
		threadData[i].opStatus = -1;
		threadData[i].terminate = 0;
		mythreads_sem_init(&threadData[i].semid ,0);//dhmiourgia shmatoforou gia ka8e worker
		mythreads_sem_init(&threadData[i].semid2 ,0);
	}
	//printf("ekana init sem\n");
	//create workers
	for(i = 0; i < threads; i++){
		mythreads_create(&arrayOfThreads[i], worker_thread, &threadData[i]);
	}


	//jobs
	while(1){
		//read next number to be checked
		scanf(" %d",&i);
		if(i == -1){
			break;
		}
		//wait for a worker to become available
		mythreads_sem_down(&available);
		for(j = 0; j < threads; j++){

			if(threadData[j].opStatus < 2){//me -1 einai init, me 0 den einai prime to proigoumeno job, me 1 einai prime to proigoumeno job
				break;
			}

			//if( j == threads -1){
				//mythreads_yield();
			//	j = -1;
			//}
		}

		//assign next job to an available worker(j)
		threadData[j].opStatus = 2;//worker now has a job
		threadData[j].numberToBeChecked = i;
	
		//wake up worker to process the job
		if (mythreads_sem_up(&threadData[j].semid) == -1){
			misses++;//metrhma xamenwn klhsewn up
		}
	}

	for(j = 0; j < threads; j++){//signal all workers to terminate
		threadData[j].terminate = 1;
		if ((threadData[j].opStatus!=2) && (mythreads_sem_up(&threadData[j].semid) == -1)){
			misses++;//metrhma xamenwn klhsewn up
		}
		//printf("ekana up to %d me opStatus %d\n",j, threadData[j].opStatus );
	}
	//printf("TA TERMATIZW\n");
	for(j = 0; j < threads; j++){//block until all workers terminate
		mythreads_sem_down(&threadData[j].semid2);
	}
	
	for(j = 0; j < threads; j++){//destroy semaphores
		mythreads_sem_destroy(&threadData[j].semid);
		mythreads_sem_destroy(&threadData[j].semid2);
	}
	//printf("missed calls: %d\n",misses);

	//free allocated memory
	free(threadData);
	free(arrayOfThreads);
	//printf("main: %lu\n", pthread_self());

	return 0;
}
