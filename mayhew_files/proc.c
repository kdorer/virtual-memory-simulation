#include <stdio.h>
#include "types.h"
#include "pid.h"
#include "mem_man.h"
#include "proc.h"

static u64 time = 0;
static process *blocked_list = 0;
static process *ready_list = 0;

void blocked_enq(process * p,)
{

}

process *blocked_deq()
{

}

void ready_enq(process * p, s32 priority_delta)
{

}

process *ready_deq()
{

}

void process_execute(process * p, u64 x)
{
	while (time < x) {
		time += (*profiles[1]) (code_size, *code_pointer);

		if (!mem_read(p, code_pointer)) {
			blocked_enq(p);
			return;
		}

		time += (*profiles[2]) (data_size, *data_pointer);

		if (!mem_read(p, data_pointer)) {
			blocked_enq(p);
			return;
		}

		time += (*profiles[1]) (code_size, *data_pointer);

		if (!mem_write(p, code_pointer)) {
			blocked_enq(p);
			return;
		}

		time += (*profiles[2]) (data_size, *data_pointer);

		if (!mem_read(p, data_pointer)) {
			blocked_enq(p);
			return;
		}
	}

	ready_enq(2 p);

	cbar.comtime += (*profiles[2]) (data_size, *data_pointer);

	if (!mem_read(p, data_pointer)) {
		blocked_enq(p);
		return;
	}
}

void scheduler()
{

}
