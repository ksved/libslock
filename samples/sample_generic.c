/*
* File: sample_generic.c
* Author: Tudor David <tudor.david@epfl.ch>
*
* Description: 
*      Simple example of how to use the generic lock interface.
*
* The MIT License (MIT)
*
* Copyright (c) 2013 Tudor David
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
* the Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 14

#include "lock_if.h"
#include "atomic_ops.h"

/* global data */
lock_global_data the_lock;

void *do_something(void *id)
{
    int* my_core = (int*) id; 
    /* local data */
    lock_local_data my_data;
    /*initialize this thread's local data*/
    init_lock_local(*my_core, &the_lock, &my_data);
    
    int count = 0;
    int i;
    int j = 10000;
    clock_t begin, end;
    double time_spent = 0, time_spent_t;
    
	MEM_BARRIER;

    while(time_spent_t < 1){
            
        begin = clock();
    	/*acquire the lock*/
    	acquire_lock(&my_data,&the_lock);    	
        
        count++;
		
    	/*release the lock*/
    	release_lock(&my_data,&the_lock);
    	
		end = clock();
        j--;
        time_spent = time_spent + (double)(end - begin);
        time_spent_t = (double) time_spent /  (CLOCKS_PER_SEC);
        //for(i =0; i<9998; i++){
//            count = count + i;
//        }
        
    }
    
    
    printf("%d\t%d\n", *my_core, count);
    
    /*free internal memory structures which may have been allocated for the local data*/
    free_lock_local(my_data);

    return NULL;

}

int main(int argc, char *argv[])
{
    pthread_t threads[NUM_THREADS];
    long t;

    /*initialize the global data*/
    init_lock_global(&the_lock); 
    int ids[]= { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };


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
    free_lock_global(the_lock);

    pthread_exit(NULL);
}
