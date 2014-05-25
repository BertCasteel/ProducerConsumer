/**
Robert Casteel
Assignment 2
CIS 415; Spring 2014; University of Oregon
**/

#include "buffer.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

buffer_item buffer[BUFFER_SIZE]; /* the buffer */
pthread_mutex_t mutex;
sem_t empty, full;
int counter;

int initializeBuffer(){
	/*create mutex lock*/
	pthread_mutex_init(&mutex, NULL);
	/*create semaphores
	Second argument indicates whether this semaphore can be used in other processes
	*/
	sem_init(&empty, 0, BUFFER_SIZE);
	sem_init(&full, 0, 0); 
	counter = 0;
	int i;
	for (i = 0; i < BUFFER_SIZE; i++){
		buffer[i] = -1;
	} 
}



int insert_item(buffer_item item) {
	/*insert item intro buffer
	return 0 if successfull, otherwise
	return -1 indicating an error condition */
	sem_wait(&empty);
	pthread_mutex_lock(&mutex);
	
	/* CS */
	buffer[counter] = item;
	counter++;
	/*End CS*/
	
	pthread_mutex_unlock(&mutex);
	sem_post(&full);
	return 0;
}

int remove_item(buffer_item *item){
	/* remove an object from buffer	placing it in item
	return 0 if successful, otheriwse
	return -1 indicating an error condition */
	sem_wait(&full);
	pthread_mutex_lock(&mutex);

	/* CS */
	*item = buffer[counter-1];
	buffer[counter-1] = -1;
	counter--;
	/*End CS */

	pthread_mutex_unlock(&mutex);	
	sem_post(&empty);
	return 0;
}

void * producer(void *param){
	buffer_item item;
	while(1){
		/*sleep for a random period of time */
		sleep(rand()%100);
		/*generate a random number */
		item = rand();
		if( insert_item(item))
			printf("report error condition\n");
		else
			printf("producer produced %d\n", item);
	}
	pthread_exit(0);
}

void * consumer(void *param){
	buffer_item item;
	while(1){	
		sleep(rand()%100);
		item = rand();
		if(remove_item(&item))
			printf("report error condition\n");
		else
			printf("consumer consumed %d\n", item);
	}
	pthread_exit(0);			
}


int main(int argc, char *argv[]){
	/* 1. Get command line arguments */
	if (argc != 4){
		printf("needs four arguments");
		return -1;
	}
	int timeLimit = atoi(argv[1]);
	int numProducers = atoi(argv[2]);
	int numConsumers = atoi(argv[3]);
	/* 2. Initialize buffer */
	initializeBuffer();
	/* 3. Create producer thread(s) */
	pthread_t prod_tid[numProducers];
	pthread_t con_tid[numConsumers];
	int i =0;
	for(i = 0; i < numProducers; i++){
		pthread_create(&prod_tid[i], NULL, producer, NULL);
	}
	/* 4. Create consumer thread(s) */
	for(i = 0; i < numConsumers; i++){
		pthread_create(&con_tid[i], NULL, consumer, NULL);
	}
	/* 5. Sleep */
	sleep(timeLimit);
	/* 6. Exit */
	return 0;
}
