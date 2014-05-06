#ifndef _PROC__H_
#define _PROC__H_

#include "types.h"

typedef struct process *proc;

struct process {
	u16 _pid;
	u32 _vas;		// Virtual Adddress Size
	u8 _priority;
	u64 _time;

	u32* _code_addr;
	u32* _code_time;

	u32* _data_addr;
	u32* _data_time;
	
	proc _next;
	
	u16 _pti;		// Page Table Index
	u16 _sbt[1024];		// Swap Block Table
	u16 _vas_vector[1024];

} process;

typedef struct blocked {
	proc _head;
} blocked;

typedef struct ready {
	proc _head;
	proc _tail;
} ready;

void blocked_enq(proc p, u64 estimated_time);
void blocked_deq();

void ready_enq(proc p);
proc ready_deq(u8 priority);

u64 time_get();
void time_adv(u32 delta);

void init_queues();
void init_process( u8 p);

u32 new_code_addr (u32		addr, u32		limit);
u64 new_code_time ();
u32 new_data_addr (u32	addr, u32		base, u32	limit);
u64 new_data_time ();

#endif
