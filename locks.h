#pragma once

#include "threads.h"
#include "interrupt.h"

typedef struct lock_descriptor {
	int is_locked;
} lock_descriptor;

void lock(lock_descriptor*);
void unlock(lock_descriptor*);
