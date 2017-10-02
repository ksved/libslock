#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 4

#include "../include/atomic_ops.h" //the memory barriers are defined there
#include "../include/mcs.h"

/* global data */
mcs_global_params the_lock;

void *do_something(void *id)
{
    int* my_core = (int*) id; 
    /* local data */
    mcs_local_params my_data;
    /*initialize this thread's local data*/
    init_mcs_local(*my_core, &my_data);
    int count = 0;
    
    while(1){
         
        /*acquire the lock*/
        mcs_acquire(the_lock.the_lock,my_data);
        printf("%d\n", *my_core);
        for(int i =0; i<10000; i++){
            count = count*i;
        }
        /*release the lock*/
        mcs_release(the_lock.the_lock,my_data);
    }

    /*free internal memory structures which may have been allocated for the local data*/
    end_mcs_local(my_data);

    return NULL;

}

int main(int argc, char *argv[])
{
    pthread_t threads[NUM_THREADS];
    long t;

    /*initialize the global data*/
    init_mcs_global(&the_lock); 
    int ids[]={0,1,2,3};

    MEM_BARRIER;

    for(t=0;t<NUM_THREADS;t++){
        printf("In main: creating thread %ld\n", t);
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
    end_mcs_global(the_lock);

    pthread_exit(NULL);
}
