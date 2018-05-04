#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>


sem_t rw_mutex;
sem_t mutex;
int read_count =0;
int shared_var=0;

void reader(void* ptr)
{
	int x = (int ) ptr;
	
	printf("%d reader is waiting\n", x);
	sem_wait(&mutex);
	printf("%d reader incrementing the reader count\n",x);
	read_count++;
	if(read_count == 1){
		printf("Making writer to wait\n");
		sem_wait(&rw_mutex);
	}
	sem_post(&mutex);

	printf("%d is reading\n", x);
	printf("value of shared variable is %d\n", shared_var);

	sem_wait(&mutex);
	printf("%d reader is leaving\n",x);
	read_count--;
	if(read_count == 0){
		printf("%d reader signal writer to perform writing\n",x);
		sem_post(&rw_mutex);
	}
	sem_post(&mutex);
}

void writer(void *ptr){

	int x = (int ) ptr;
	
	printf("%d Writer is waiting\n",x); 
	sem_wait(&rw_mutex);
	printf("%d is writring\n",x);
	shared_var++;
	sleep(1);
	sem_post(&rw_mutex);
	sleep(1);
}



int main(){


	sem_init(&rw_mutex, 0, 1);
	sem_init(&mutex, 0, 1);

	int i, w,r;
	printf("Enter number of writers: ");
	scanf("%d",&w);

	printf("Enter number of readers: ");
	scanf("%d",&r);
	
	pthread_t thread[7];


	for(i=0; i<r; i++)
	{
		pthread_create(&thread[i], NULL, (void*) &reader, (void*) i);


	}

	for(i=r; i<w+r; i++)
	{
		pthread_create(&thread[i], NULL, (void*) &writer, (void*) i);

	}	
	for(i=0; i<w+r; i++)
	{
		pthread_join(thread[i], NULL);

	}

	sem_destroy(&rw_mutex);
	sem_destroy(&mutex);
	
}
