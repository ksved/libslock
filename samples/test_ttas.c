#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 100

#include "../include/atomic_ops.h" //the memory barriers are defined there
#include "../include/ttas.h"
#include "time.h"

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
    int j = 10000;
    clock_t begin, end;
    double time_spent = 0;
    
    MEM_BARRIER;
    
    while(j > 0){
         

        begin = clock();
        /*acquire the lock*/
        ttas_lock(the_lock,&my_data);
        end = clock();

        /*release the lock*/
        ttas_unlock(the_lock);
        
        time_spent = time_spent + (double)(end - begin);
        j--;
    }
    
    time_spent = (double) time_spent /  (CLOCKS_PER_SEC * 10000);
    printf("%d\t%lf\t%d\n", *my_core, time_spent, count);
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
    int ids[]= { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99 };


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

    pthread_exit(NULL);
}
