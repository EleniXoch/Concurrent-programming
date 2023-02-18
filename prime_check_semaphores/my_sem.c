#include "my_sem.h"

int mysem_create(int initValue){
	int semid;

	if((initValue != 0) && (initValue != 1)){
		printf("mysem_create: Wrong initialization value. Must be 0 or 1!\n");
		return -1;
	}

	semid = semget(IPC_PRIVATE, 1, S_IRWXU);
	if (semid == -1){
		printf("mysem_create: Semget error!\n");
		return -1;
	}
	semctl(semid,0,SETVAL,initValue);
	return semid;
}

void mysem_down(int semid){
	struct sembuf op;
	int result;

	op.sem_num= 0; 
	op.sem_op= -1; 
	op.sem_flg= 0;
	result = semop(semid,&op,1);

	if(result == -1){
		printf("mysem_down: Semop error!\n");
	}
}

int mysem_up(int semid){
	struct sembuf op;
	int result;
	int value;

	value = semctl(semid,0,GETVAL);
	if(value == 1){
		return -1;//missed call
	}

	op.sem_num= 0; 
	op.sem_op= 1; 
	op.sem_flg= 0; 
	result = semop(semid,&op,1);

	if(result == -1){
		printf("mysem_down: Semop error!\n");
		return -2;
	}
	return result;
}

void mysem_destroy(int semid){
	int result;

	result = semctl(semid,0,IPC_RMID);

	if (result == -1){
		printf("mysem_cdestroy: Semctl error!\n");
	}
}

int mysem_value(int semid){
	return semctl(semid,0,GETVAL);
}
