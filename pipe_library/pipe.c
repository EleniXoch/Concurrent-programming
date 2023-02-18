#include "pipe.h"

struct FIFO{

	volatile long int writeIndex;
	volatile long int readIndex;
	volatile int size;
	volatile int used;
	volatile int writeFlag;
	volatile int readFlag;
	char* matrixPtr;

};

volatile struct FIFO fifoPipe;

void pipe_init(int size){

	fifoPipe.writeIndex = 0; // thesi tou pinaka pou tha graftei to epomeno byte
	fifoPipe.readIndex = 0; // thesi tou pinaka pou tha diavastei to epomeno byte
	fifoPipe.size = size; // to megethos tou agwgou
	fifoPipe.used = 0; // poses theseis xrhhsmopoiountai
	fifoPipe.writeFlag = 1; //1 is for writing, 0 is not writing
	fifoPipe.matrixPtr = (char*)calloc(size,sizeof(char));

}

void pipe_destroy(){

	free(fifoPipe.matrixPtr);

}


void pipe_writeDone(){

	fifoPipe.writeFlag = 0;	

}


void pipe_write(char c){

	while(fifoPipe.writeIndex - fifoPipe.readIndex == fifoPipe.size){//einai gematos o agwgos
		sched_yield();
	}

	
	
	//grafei to byte pou phre san orisma sthn epomenh kenh thesi
	fifoPipe.matrixPtr[fifoPipe.writeIndex%fifoPipe.size] = c;
	//fifoPipe.used++;
	fifoPipe.writeIndex++;
	/*if(fifoPipe.writeIndex == fifoPipe.size){//an ftasei sto telos na gyrnaei sthn arxh to index
		fifoPipe.writeIndex = 0;
	}*/
	
	//printf("PIPE_WRITE: read index %d wire index %d used %d\n",fifoPipe.readIndex, fifoPipe.writeIndex, fifoPipe.used );
	 
}

int pipe_read(char* c){

	while((fifoPipe.readIndex == fifoPipe.writeIndex)){//o agwgos einai adeios
		if (fifoPipe.writeFlag == 0){//exei klhthei h writeDone
			//ara termatizei h pipe_read
			return 0;
		}
		sched_yield();
	}

	

	//diavazei to epomeno byte apo ton agwgo
	*c = fifoPipe.matrixPtr[fifoPipe.readIndex%fifoPipe.size];
	//fifoPipe.used--;
	fifoPipe.readIndex++;
	/*if(fifoPipe.readIndex == fifoPipe.size){//an ftasei sto terma na gyrnaei sthn arxh
		fifoPipe.readIndex = 0;
	}
	*/
	//printf("PIPE_READ: read index %d wire index %d used %d\n",fifoPipe.readIndex, fifoPipe.writeIndex, fifoPipe.used );
	

	return 1;
}
