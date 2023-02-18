#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

struct thread_input{
	volatile int numberToBeChecked; //number to check if it is prime
	volatile int opStatus; // init to -1(available), 0 is result(NO), 1 is result(YES), 2 is working, 3 is for terminate
	//volatile int terminate; // init to 0, 1 is terminate
	pthread_cond_t wakeup;
};

pthread_mutex_t mutex;
pthread_cond_t available;
pthread_cond_t termination;
int signal_terminate; //0 is init, 1 is terminate
int avl; // how many are available
int terminated; // how many have terminated

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
	unsigned long int self = pthread_self();
	int result;

	
	while(1){
		//wait for wake up by main
		//mysem_down(input_ptr->semid);
		pthread_mutex_lock(&mutex);
		avl++;
		pthread_cond_signal(&available);
		//lew sthn main oti eimai available
		if((input_ptr->opStatus <2) && (signal_terminate != 1)){//den tou exei anatethei job
			//printf("Worker  %lu: den mou exei dothei job\n", self);
			pthread_cond_wait(&(input_ptr->wakeup), &mutex);
		}

		//check if notfied to terminate in the meantime
		if(signal_terminate == 1){
			//printf("Worker  %lu: kanw signal thn main kai termatizw 1\n", self);
			input_ptr->opStatus = 3;
			terminated++;
			pthread_cond_signal(&termination);
			pthread_mutex_unlock(&mutex);
			return NULL;
		}

		pthread_mutex_unlock(&mutex);
		result = isPrime(input_ptr->numberToBeChecked);
		
		if(result){
			printf("%d is a prime!\n", input_ptr->numberToBeChecked);
		}else{
			printf("%d is NOT a prime!\n", input_ptr->numberToBeChecked);
		}

		pthread_mutex_lock(&mutex);
		input_ptr->opStatus = result;
		pthread_mutex_unlock(&mutex);
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
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&available, NULL);
	pthread_cond_init(&termination, NULL);

	signal_terminate = 0;
	avl = 0;
	terminated = 0;
	//arxikopoihsh
	for (i = 0; i < threads; i++){
		pthread_cond_init(&threadData[i].wakeup, NULL);
		threadData[i].numberToBeChecked = -1;
		threadData[i].opStatus = -1;
		//threadData[i].terminate = 0;
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
		pthread_mutex_lock(&mutex);

		//wait for a worker to become available
		//if no one is available, wait and try again
		if(avl == 0){
			pthread_cond_wait(&available, &mutex);
		}
		//now someone is definetely available

		for(j = 0; j < threads; j++){

			if(threadData[j].opStatus < 2){//me -1 einai init, me 0 den einai prime to proigoumeno job, me 1 einai prime to proigoumeno job
				break;
			}
		}
		//assign next job to an available worker(j)
		threadData[j].opStatus = 2;//worker now has a job
		threadData[j].numberToBeChecked = i;
		avl--;
		//wake up worker to process the job
		//printf("main edwsa job avl: %d\n", avl);
		pthread_cond_signal(&threadData[j].wakeup);
	
		
		pthread_mutex_unlock(&mutex);
	}

	
	//wait for all the workers to become avilable
	pthread_mutex_lock(&mutex);
	//printf("MAIN: CHECK IF EVERYONE IS AVAILABLE\n");
	while(avl != threads){
		pthread_cond_wait(&available, &mutex);
	}
	//printf("TA OPSTATUS:\n");
	//for(j = 0; j < threads; j++){
	//	printf("%d\n", threadData[j].opStatus);
	//}

	signal_terminate = 1; // dwse to shma na termatisoun
	pthread_mutex_unlock(&mutex);



	pthread_mutex_lock(&mutex);
	//printf("MAIN: TA TERMATIZW\n");
	for(j = 0; j < threads; j++){//dwse shma na termatisoun
		pthread_cond_signal(&threadData[j].wakeup);
	}
	pthread_mutex_unlock(&mutex);
	


	//wait for all the workers to terminate
	pthread_mutex_lock(&mutex);
	//printf("MAIN: CHECK IF EVERYONE IS TERMINATED\n");
	while(terminated != threads){
		pthread_cond_wait(&termination, &mutex);
	}
	pthread_mutex_unlock(&mutex);

	//free allocated memory
	free(threadData);
	free(arrayOfThreads);
	//printf("main: %lu\n", pthread_self());
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&available);
	pthread_cond_destroy(&termination);

	return 0;
}
