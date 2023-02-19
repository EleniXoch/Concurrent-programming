#include <signal.h>
#include <sys/time.h>
#include <stdlib.h>
#include "mythreads.h"

extern ucontext_t return_co;


list_t list;
thr_t main_thread;
co_t markdead_scheduler_co;
volatile int critical_section_flag = 0;


void list_append_tail(list_t* list, thr_t* thread){

	list_node_t* curr = (list_node_t*)calloc(1,sizeof(list_node_t));
	curr->data = thread;
	curr->next = NULL;

	if(list->head == NULL){
		list->head = curr;
		list->tail = list->head;
		return;
	}

	list->tail->next = curr;
	list->tail = curr;
	//printf("list tail id :%d\n", list->tail->data->id);
}



void node_destroy(list_t* list, thr_t* thread){
	list_node_t* curr, *prev = NULL;


	for (curr = list->head; curr != NULL ; prev = curr, curr = curr->next){
		if(curr->data->id == thread->id){
			break;
		}
	}
	if (curr == NULL){
		return;
	}
	if(curr == list->head){
		list->head = list->head->next;
		free(curr);
		return;
	}
	if(curr == list->tail){
		list->tail = prev;
		list->tail->next = NULL;
		free(curr);
		return;
	}
	prev->next = curr->next;
	
	free(curr);
	return;
}

void init_list(list_t* list){
	list->head = NULL;
	list->tail = NULL;
}

int mythreads_self(){
	return list.head->data->id;
}

int mythreads_sem_init(sem_t *s, int val){
	static int semids = 1;

	critical_section_flag = 1;
	s->semlist = (list_t*)malloc(sizeof(list_t));
	init_list(s->semlist);
	s->val = val;
	s->semid = semids++;
	critical_section_flag = 0;

	return 1;
}


int mythreads_sem_down(sem_t *s){

	critical_section_flag = 1;
	//printf("down timh sem: %d\n", s->val);
	if(s->val > 0){
		s->val--;
		critical_section_flag = 0;
		return 1;
	}
	//vlock the current thread to the s semaphore
	s->val--;
	list.head->data->sem_block = s->semid;
	list_append_tail(s->semlist, list.head->data);
	critical_section_flag = 0;
	mythreads_yield(s->semlist);

	return 1;

}

int mythreads_sem_up(sem_t *s){
	//list_node_t* temp;

	critical_section_flag = 1;
	//printf("up timh sem: %d\n", s->val);
	s->val++;
	if(s->semlist->head != NULL){
		s->semlist->head->data->sem_block = 0;
		node_destroy(s->semlist, s->semlist->head->data);
	}
	critical_section_flag = 0;

	return 1;
}

int mythreads_sem_destroy(sem_t *s){
	//if head is null return 0
	if(s->semlist->head == NULL){
		return 0;
	}
	free(s->semlist);
	return 1;
}


void list_head2tail(list_t* list){
	list_node_t* temp;

	if((list->head == NULL) || (list->head == list->tail)){
		//printf("ben egine peristrofi\n");
		return;
	}

	temp = list->head;
	list->head = list->head->next;
	temp->next = NULL;
	list->tail->next = temp;
	list->tail = temp;
}

void pSigHandler(){
	//printf("HRTE SHMA\n");
	thr_t* temp ;
	temp = (list.head->data);
	//int result;
	list_node_t* curr;

	if(critical_section_flag == 1){
		//printf("kaleitai switchto\n");
		return;
	}

	//printf("head: %d %p\n", list.head->data->id, list.head->data);
	list_head2tail(&list);
	while((list.head->data->state != 0) && (list.head->data->sem_block == 0)){
		//printf("loupa\n");
		list_head2tail(&list);
	}

	//printf("eftasa %d %d tail : %d\n", temp->id, list.head->data->id, list.tail->data->id);
	for (curr = list.head; curr != NULL ;curr = curr->next){
		//printf("ID:%d STATE: %d SEM_BLOCK: %d\n", curr->data->id, curr->data->state, curr->data->sem_block);
	}
	if(temp->id != list.head->data->id){
		//printf("signalhandler swap!\n");
		swapcontext(&(temp->context.current_co), &(list.head->data->context.current_co));
	}
	//mycoroutines_switchto(&(list.head->data->context));
}

void* markdead_scheduler(void* arg){
	list_node_t* curr;

	//printf("mphka sto dead %d---------------\n",list.head->data->id);
	critical_section_flag = 1;
	list.head->data->state = -1;
	for (curr = list.head; curr != NULL ;curr = curr->next){
		if(curr->data->state == list.head->data->id){
			//printf("ksipnaw thn main---------\n");
			curr->data->state = 0;
		}
	}
	critical_section_flag = 0;

	while(1){
		pSigHandler();
	}
	//find if a thread is blocked on that id
	//printf("termatise adoksa\n");
	return NULL;
}

int mythreads_init(){
	struct sigaction psa;
	struct itimerval tv;

	init_list(&list);

	//mythreads_create(main);
	main_thread.id = 1;
	main_thread.sem_block = 0;
	main_thread.state = 0;
	mycoroutines_init(&main_thread.context);
	mycoroutines_create(&markdead_scheduler_co, markdead_scheduler,NULL);
	return_co = markdead_scheduler_co.current_co;
	list_append_tail(&list, &main_thread);
	//printf("head2: %d %p\n", list.head->data->id, &list.head);
	psa.sa_flags = 0;
	sigemptyset(&(psa.sa_mask));
    sigaddset(&(psa.sa_mask), SIGALRM);
	psa.sa_handler = pSigHandler;   // << use the name of your signal hander
	sigaction(SIGALRM, &psa, NULL);

	tv.it_interval.tv_sec = 0;
	tv.it_interval.tv_usec = 1000000;  // when timer expires, reset to 100ms
	tv.it_value.tv_sec = 0;
	tv.it_value.tv_usec = 1000000;   // 100 ms == 100000 us
	setitimer(ITIMER_REAL, &tv, NULL);



	return 1;
}



int mythreads_create(thr_t *thr, void *(*body)(void *),void *arg){
	static int ids = 2; //1 is reserved for main
	int result;

	critical_section_flag = 1;
	//thr = (thr_t*)calloc(1,sizeof(thr_t));
	thr->id = ids++;
	thr->state = 0;
	thr->sem_block = 0;
	//printf("thr->id2 %d\n", thr->id);
	list_append_tail(&list, thr);
	result = mycoroutines_create(&(thr->context), body, arg);
	critical_section_flag = 0;

	return result;
} 

int mythreads_join(thr_t *thr){
	list_node_t* curr;

	critical_section_flag = 1;
	for (curr = list.head; curr != NULL ; curr = curr->next){
		//printf("thr->id: %d\n", thr->id);
		if(curr->data->id == thr->id){
			if(thr->state == -1){
				//printf("den perimenw\n");
				critical_section_flag = 0;
				return 1;
			}else{
				list.head->data->state = thr->id;
				//printf("perimenw\n");
				critical_section_flag = 0;
				pSigHandler();
				//critical_section_flag = 0;
			}
		}
	}
	critical_section_flag = 0;

	return 0;
}

int mythreads_yield(){
	pSigHandler();
	return 1;
}
