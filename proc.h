#ifndef _PROC__H_
#define _PROC__H_

#include "types.h"

typedef struct process{
	u16 _pid;
	u32 _vas;		// Virtual Adddress Size

	u64 _time;
	u8 _priority;
	
	struct process *_next;
	
	u16 _pti;		// Page Table Index
	u16 _sbt[1024];		// Swap Block Table

} process;

typedef struct blocked {
	struct process *_head;
} blocked;

typedef struct ready {
	struct process *_head;
	struct process *_tail;
} ready;

void blocked_enq(process * p, u64 time);
void blocked_deq();

void ready_enq(process * p);
process *ready_deq(u8 priority);

u64 time_get();
void time_adv(u32 delta);

void init_queues();

#endif
