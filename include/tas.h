/*
 * File: ttas.h
 * Author: Tudor David <tudor.david@epfl.ch>, Vasileios Trigonakis <vasileios.trigonakis@epfl.ch>
 *
 * Description: 
 *      Implementation of a test-and-test-and-set lock with back-off
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Tudor David, Vasileios Trigonakis
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


#ifndef _tas_H_
#define _tas_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#ifndef __sparc__
#include <numa.h>
#endif
#include <pthread.h>
#include "atomic_ops.h"
#include "utils.h"


#define MIN_DELAY 100
#define MAX_DELAY 1000

typedef volatile uint32_t tas_index_t;
#ifdef __tile__
typedef uint32_t tas_lock_data_t;
#else
typedef uint8_t tas_lock_data_t;
#endif

typedef struct tas_lock_t {
    union {
        tas_lock_data_t lock;
#ifdef ADD_PADDING
        uint8_t padding[CACHE_LINE_SIZE];
#else
        uint8_t padding;
#endif
    };
}tas_lock_t;


static inline uint32_t backoff(uint32_t limit) {
    uint32_t delay = rand()%limit;
    limit = MAX_DELAY > 2*limit ? 2*limit : MAX_DELAY;
    cdelay(delay);
    return limit;

}

/*
 *  Lock acquire and release methods
 */

void tas_lock(tas_lock_t* the_lock, uint32_t* limit);

int tas_trylock(tas_lock_t* the_lock, uint32_t* limit);

void tas_unlock(tas_lock_t* the_lock);

int is_free_tas(tas_lock_t * the_lock);
/*
   Some methods for easy lock array manipluation
   */

tas_lock_t* init_tas_array_global(uint32_t num_locks);


uint32_t* init_tas_array_local(uint32_t thread_num, uint32_t size);


void end_tas_array_local(uint32_t* limits);


void end_tas_array_global(tas_lock_t* the_locks);

/*
 *  Single lock initialization and destruction
 */

int init_tas_global(tas_lock_t* the_lock);

int  init_tas_local(uint32_t thread_num, uint32_t* limit);

void end_tas_local();

void end_tas_global();


#endif


