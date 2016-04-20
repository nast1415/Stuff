#include "threads.h"
#include "stdio.h"

pid_t current_thread_id, number_of_threads;
thread threads[MAX_CNT];
pid_t joins[MAX_CNT];
lock_descriptor* ld;


void setup_threads() {
	current_thread_id = 0;
	number_of_threads = 1;
	threads[0].state = RUNNING;
}

pid_t create_thread(void (*fptr)(void*), void* arg) {
	local_irq_disable();
	
	number_of_threads++;
	thread* new_thread = &threads[number_of_threads - 1];	

	new_thread->stack = kmem_alloc(2 * PAGE_SIZE);
	new_thread->stack_pointer = (uint8_t*) new_thread->stack + 2 * PAGE_SIZE - sizeof(thread_init);

	thread_init* init_value = new_thread->stack_pointer;
	extern void *start_thread;

	init_value->start_thread_addr = &start_thread;

	init_value->r12 = 0;
    init_value->r13 = 0;
    init_value->r14 = 0;
    init_value->r15 = 0;
    init_value->rbx = 0;
    init_value->rbp = 0;

	init_value->fptr = fptr;
    init_value->arg = arg;

    new_thread->state = RUNNING;

	local_irq_enable();
	return (pid_t)(number_of_threads - 1);
}

void exit_thread() {
	local_irq_disable();	
	threads[current_thread_id].state = TERMINATED;

	if (threads[joins[current_thread_id]].state == JOINING) {
		threads[joins[current_thread_id]].state = RUNNING;
	}
	schedule();
}

void join(pid_t awaited_thread_id) {
	local_irq_disable();	
	threads[current_thread_id].state = JOINING;
	joins[awaited_thread_id] = current_thread_id;
	
	while (threads[awaited_thread_id].state != TERMINATED && threads[awaited_thread_id].state != DELETED) {
        //printf("I'm going to go the schedule from join!\n");
        schedule();
		//printf("I came back from the schedule to join!\n");
        
    }
	local_irq_enable();
}
void switch_threads(void **old_sp, void *new_sp);

void schedule() {
	//printf("I'm in schedule! I'm very glad!\n");
	pid_t next_thread_id = (current_thread_id + 1) % number_of_threads;

	while(1) {
		if (next_thread_id != current_thread_id) {
			if (threads[next_thread_id].state == RUNNING) {
				pid_t previous_thread_id = current_thread_id;
				current_thread_id = next_thread_id;

				switch_threads(&threads[previous_thread_id].stack_pointer, threads[current_thread_id].stack_pointer);

				local_irq_disable();			
				break;
			}
			if (threads[next_thread_id].state == TERMINATED) {
				kmem_free(threads[next_thread_id].stack);
				threads[next_thread_id].state = DELETED;
			}
		} else {
			if (threads[next_thread_id].state == RUNNING) {
				break;			
			}
		}
		next_thread_id = (next_thread_id + 1) % number_of_threads;
	}
}


