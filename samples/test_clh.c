#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 28

#include "../include/atomic_ops.h" //the memory barriers are defined there
#include "../include/clh.h"

/* global data */
clh_global_params global_d;

void *do_something(void *id)
{
    int* my_core = (int*) id; 
    /* local data */
    clh_local_params *local_d;
    
    /*initialize this thread's local data*/
    init_clh_local(*my_core, local_d);
    
    int count = 0;
    int i;
    int j = 10000;
    clock_t begin, end;
    double time_spent = 0;
    
    MEM_BARRIER;
    
    while(j > 0){
        for(i =0; i<10000; i++){
            count = count + i;
        }
        
        begin = clock();
        /*acquire the lock*/
        local_d->my_pred= (clh_qnode*) clh_acquire(&global_d->the_lock, local_d->my_qnode);
        end = clock();
        
        for(i =0; i<10000; i++){
            count = count - i;
        }
		
		/*release the lock*/
        local_d->my_qnode=clh_release(local_d->my_qnode, local_d->my_pred);
        time_spent = time_spent + (double)(end - begin);
        j--;        
    }
     
    time_spent = (double) time_spent /  (CLOCKS_PER_SEC * 10000);
    printf("%d\t%lf\t%d\n", *my_core, time_spent, count);
    /*free internal memory structures which may have been allocated for the local data*/
    end_clh_local(*local_d);

    return NULL;

}

int main(int argc, char *argv[])
{
    pthread_t threads[NUM_THREADS];
    long t;
    
    
    /*initialize the global data*/
    init_clh_global(&global_d);
    int ids[]= { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27 };

    MEM_BARRIER;

    for(t=0;t<NUM_THREADS;t++){
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
    end_clh_global(global_d);

    pthread_exit(NULL);
}
