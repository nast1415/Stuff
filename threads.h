#pragma once

#include <sys/types.h>
#include "locks.h"
#include "interrupt.h"
#include "memory.h"
#include "kmem_cache.h"
#include "assert.h"

#define MAX_CNT 100000

typedef enum {RUNNING, JOINING, TERMINATED, DELETED} thread_state;

typedef struct thread {
	void* stack; //A pointer to the top of our stack
	void* stack_pointer; //A pointer to the top of next thread's stack
	thread_state state;
} thread;

typedef struct thread_initialization {
	uint64_t r15, r14, r13, r12, rbx, rbp;
	void* start_thread_addr;

	void (*fptr)(void*);
	void* arg;
} thread_init;


extern pid_t current_thread_id, number_of_threads;
extern thread threads[MAX_CNT];
extern pid_t joins[MAX_CNT];

void setup_threads(); 
pid_t create_thread(void (*fptr)(void*), void* arg);

void join(pid_t awaited_thread_id);

void schedule();
