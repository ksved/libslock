#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 4

#include "../include/atomic_ops.h" //the memory barriers are defined there
#include "../include/ttas.h"

ttas_lock_t* the_lock;

void *do_something(void *id)
{
    int* my_core = (int*) id; 
    /* local data */
    uint32_t  my_data;
    /*initialize this thread's local data*/
    init_ttas_local(*my_core, &my_data);
    
    int count = 0;
    int i;
    while(1){
         
        for(i =0; i<10000; i++){
            count = count*i;
        }
        /*acquire the lock*/
        ttas_lock(the_lock,&my_data);
        
        printf("%d\n", *my_core);
        for(i =0; i<10000; i++){
            count = count*i;
        }
        /*release the lock*/
        ttas_unlock(the_lock);
    }
    
    /*free internal memory structures which may have been allocated for the local data*/
    end_ttas_local(my_data);

    return NULL;

}

int main(int argc, char *argv[])
{
    pthread_t threads[NUM_THREADS];
    long t;
     the_lock = (ttas_lock_t*)malloc(sizeof(ttas_lock_t));
    /*initialize the global data*/
    init_ttas_global(the_lock); 
    int ids[]={0,1,2,3};

    MEM_BARRIER;

    for(t=0;t<NUM_THREADS;t++){
        //printf("In main: creating thread %ld\n", t);
        if (pthread_create(&threads[t], NULL, *do_something, &ids[t])!=0){
            fprintf(stderr,"Error creating thread\n");
            exit(-1);
        }
    }

    for (t = 0; t < NUM_THREADS; t++) {
        if (pthread_join(threads[t], NULL) != 0) {
            fprintf(stderr, "Error waiting for thread completion\n");
            exit(1);
        }
    }

    /*free internal memory strucutres which may have been allocated for this lock */
    end_ttas_global();

    pthread_exit(NULL);
}
