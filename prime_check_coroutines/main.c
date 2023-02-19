#include "mythreads.h"
#include <stdlib.h>
#include <unistd.h>


sem_t sem;

void* foo(void* arg){
	printf("eimai to2 prospathw na kanw down\n");
	mythreads_sem_down(&sem);
	printf("eimai to 2. ekana down\n");
	sleep(5);
	printf("termatizw2\n");
}

void* foo2(void* arg){
	printf("eimai to 3. kanw up ton sem\n");
	mythreads_sem_up(&sem);
	printf("eimai to 3\n");
	sleep(5);
	printf("termatizw3\n");
}

int main(int argc, char const *argv[]){
	int i,j= 0 ;
	thr_t t1, t2;

	mythreads_init();

	mythreads_sem_init(&sem, 0);
	mythreads_create(&t1, foo, NULL);
	mythreads_create(&t2, foo2, NULL);
	// sleep(5);
	// printf("eimai edw\n");
	// // for (i = 0; i < 1000000; i++){
	// // 	j++;
	// // 	printf(".\n");
	// // }
	// // printf("%d\n", j);
	// sleep(5);
	// printf("eimai ekei\n");
	// sleep(5);
	printf("eftasa st0 join 1 ----------------\n");
	//printf("id2 :%d\n", t1.id);
	mythreads_join(&t1);
	printf("eftasa st0 join 2 ----------------\n");
	mythreads_join(&t2);
	printf("termatizw\n");
	return 0;
}
