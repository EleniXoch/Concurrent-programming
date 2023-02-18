#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

struct thread_input{
	volatile int numberToBeChecked; //number to check if it is prime
	volatile int opStatus; // init to -1(available), 0 is result(NO), 1 is result(YES), 2 is working, 3 is for terminate
	volatile int mainSignals; // init to 0, 1 is start, 2 is terminate
};

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
	sched_yield();

		
	while(1){//wait for notification by main

		if(input_ptr->mainSignals == 1){//start signal by main
			input_ptr->mainSignals = 0;
			//notify main that i am available/process the job/done the job
			input_ptr->opStatus = isPrime(input_ptr->numberToBeChecked);
			
			if(input_ptr->opStatus){
				printf("%d is a prime!\n", input_ptr->numberToBeChecked);
			}else{
				printf("%d is NOT a prime!\n", input_ptr->numberToBeChecked);
			}

		}else if(input_ptr->mainSignals == 2){//terminate signal by main

			input_ptr->opStatus = 3;// tha termatisw
			return 0;

		}
	}
}


int main(int argc, char const *argv[]){
	pthread_t * arrayOfThreads ;
	struct thread_input *threadData ;
	int i, j, threads;
	
	threads = atoi(argv[1]);
	arrayOfThreads = (pthread_t *) malloc(threads*(sizeof(pthread_t)));
	threadData = (struct thread_input *) malloc(threads*(sizeof(struct thread_input)));
	//arxikopoihsh
	for (i = 0; i < threads; i++){
		threadData[i].numberToBeChecked = -1;
		threadData[i].opStatus = -1;
		threadData[i].mainSignals = 0;
	}

	//create workers
	for(i = 0; i < threads; i++){
		pthread_create(&arrayOfThreads[i], NULL, worker_thread, &threadData[i]);
	}


	//jobs
	while(1){
		scanf(" %d",&i);
		if(i == -1){
			break;
		}
		//wait for a worker to become available
		for(j = 0; j < threads; j++){

			if(threadData[j].opStatus < 2){//me -1 einai init, me 0 den einai prime to proigoumeno job, me 1 einai prime to proigoumeno job
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
		threadData[j].mainSignals = 1; // start the job
	}

	//wait for all workers to become available
	sched_yield();
	for(j = 0; j < threads; j++){
		if((threadData[j].opStatus == 2)){//working signal
			//sched_yield();
			j = -1;
		}
	}

	//terminate workers
	for(j = 0; j < threads; j++){
		threadData[j].mainSignals = 2;
	}

	//wait for termination
	sched_yield();
	for(j = 0; j < threads; j++){
		if(threadData[j].opStatus != 3){//termination signal
			sched_yield();
			j = -1;
		}
	}

	return 0;
}
