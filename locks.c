#include "locks.h"

void lock(lock_descriptor* ld) {
	local_irq_disable();    
	while(ld->is_locked == 1) {
        schedule();
    }
    ld->is_locked = 1;
    local_irq_enable();
}

void unlock(lock_descriptor* ld) {
	local_irq_disable();
	ld->is_locked = 0;
    local_irq_enable();
}
