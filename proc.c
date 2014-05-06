#include <stdio.h>
#include <stdlib.h>
#include "types.h"
//#include "pid.h"
#include "mem_man.h"
#include "proc.h"

// Create all of the queues
static blocked _blocked;
static ready _high;
static ready _medium;
static ready _low;

static int counter = 0;
static u64	time = 0;
static u64 time_blocked = 0;

// initial address, process time, time prediction, priority, and something else

void blocked_enq(proc p, u64 estimated_time)
{
	printf("Placing process %d in the blocked queue\n", p->_pid);

	p->_blocked_timer = time_get() + estimated_time;
	if (_blocked._head != NULL)
	{
		_blocked._head = p;
	}
	else
	{
		p->_next = _blocked._head;
		_blocked._head = p;
	}
}

void blocked_deq()
{
	printf("Removing process %d from the blocked queue\n", p->_pid);

	proc current = malloc(sizeof(*current));
	proc previous = malloc(sizeof(*previous)); 
	proc next = malloc(sizeof(*next));

	if (_blocked._head != NULL)
	{
		if (current->_blocked_timer <= time_get())
		{
			ready_enq(_blocked._head);
			_blocked._head = NULL;
		}
	}

	else
	{
		current = _blocked._head;
		do
		{
			next = current->_next;

			if (current->_blocked_timer <= time_get())
			{
				if (_blocked._head == current)
				{
					_blocked._head = next;
				}

				previous->_next = next;
				current->_next = NULL;
				ready_enq(current);
			}

			else
			{
				previous = current;
			}

			current = next;
		} while(current != NULL);
	}
}

void ready_enq(proc p)
{
	printf("Placing process %d in the ready queue\n", p->_pid);

	switch (p->_priority)
	{
		case 1 :
			if (_high._head == NULL && _high._tail == NULL)
			{
				_high._head = p;
				_high._tail = p;
			}

			else
			{
				_high._tail->_next = p;
				_high._tail = p;
			}
			break;
		
		case 2 :
			if (_medium._head == NULL && _medium._tail == NULL)
			{
				_medium._head = p;
				_medium._tail = p;
			}

			else
			{
				_medium._tail->_next = p;
				_medium._tail = p;
			}
			break;

		case 3 :
			if (_low._head == NULL && _low._tail == NULL)
			{
				_low._head = p;
				_low._tail = p;
			}

			else
			{
				_low._tail->_next = p;
				_low._tail = p;
			}
			break;
	}
}

proc ready_deq(u8 priority)
{
	proc p = malloc(sizeof(*p));

	switch (priority)
	{
		case 1:
			p = _high._head;
			_high._head = p->_next;
			p->_next = NULL;
			break;

		case 2:
			p = _medium._head;
			_medium._head = p->_next;
			p->_next = NULL;
			break;

		case 3:
			p = _low._head;
			_low._head = p->_next;
			p->_next = NULL;
			break;
	}

	printf("Removing process %d from the ready queue\n", p->_pid);

	return p;
}

//
u32 new_code_addr (u32		addr, u32		limit)
{
	static u32	x[32]	= {	 1,	 1,	 1,	 1,	 1,	 1,	 1,	 1, 1,	 1,	 1,	 1,	 1,	 1,	 1,	 1, 
							 2,	 2,	 2,	 2,	 2,	 2,	 2,	 2, 3,	 3,	 3,	 3,	 4,	 4,	 8,	16};

	u32	r = rand();
	addr	=  (r & 64) ? addr + x[r & 31] : addr - x[r & 31];
	return (addr > limit) ? addr = r % limit : addr;
}

//
u64 new_code_time ()
{
	return	50 + (rand() & 0xfff);
}

//
u32 new_data_addr (u32	addr, u32		base, u32	limit)
{
	static u32	x[32]	= {	 1,	 1,	 1,	 1,	 2,	 2,	 2,	 2,	 3,	 3,	 3,	 3,	 4,	 4,	 4,	 4, 
							 5,	 5,	 6,	 6,	 7,	 7,	 8,	 8,	 9,	10,	11,	12,	16,	20,	28,	40};

	u32	r = rand();
	addr	=  (r & 64) ? addr + x[r & 31] : addr - x[r & 31];
	return ((base < addr) || (addr > limit)) ? addr = base + (r % (limit - base)) : addr;
}

//
u64 new_data_time ()
{
	return	100 + (rand() & 0x1fff);
}

//
u64 get_time()
{
	return time;
}

void set_time(u64 t)
{
	time =  t;
}

//
void process_exec (u64	t, proc p, u32 code_limit, u32 data_limit)
{
	u64	t	= get_time();

	u32	code_trans = virt_to_phys(p->_code_addr, p);
	u32	data_trans = virt_to_phys(p->_data_addr, p);

	u64 timer = p->_time;

	if (!code_trans) 
	{
		page_fault(p->_code_addr, p);
		return;
	}

	if (!data_trans) 
	{
		page_fault(p->_data_addr, p);
		return;
	}

	while (timer) 
	{
		if (p->_code_time < p->_data_time) 
		{
			if (p->_code_time > timer) 
			{
				printf("Process %d ran out of time\n", p->_pid);

				p->_code_time -= timer;
				set_time(time_get() + timer);
				timer -= timer;

				ready_enq(p);
				return;
			}

			else
			{
				set_time(time_get() + p->_code_time);
				timer -= p->_code_time;

				p->_code_addr	= new_code_addr(p->_code_addr, code_limit);
				p->_code_time	= new_code_time();
				code_trans	= virt_to_phys(p->_code_addr, p);
			}

			if (!code_trans) 
			{
				page_fault(p->_code_addr, p);
				return;
			}
		}

		else 
		{
			if (p->_data_time > timer) 
			{
				printf("Process %d ran out of time\n", p->_pid);

				p->_data_time -= timer;
				set_time(time_get() + timer);
				timer -= timer;

				ready_enq(p);
				return;
			}

			else
			{
				set_time (time_get() + p->_data_time);
				timer -= p->_data_time;

				p->_data_addr	= new_data_addr(p->_data_addr, code_limit, data_limit);
				p->_data_time	= new_data_time();
				data_trans	= virt_to_phys(p->_data_addr, p);
			}

			if (!data_trans) 
			{
				page_fault(p->_data_addr, p);
				return;
			}
		}
	}
}

// Initialize values in the queues
void init_queues()
{
	_blocked._head = NULL;

	_high._head = NULL;
	_high._tail = NULL;

	_medium._head = NULL;
	_medium._tail = NULL;

	_low._head = NULL;
	_low._tail = NULL;
}

// Initializes a process
void init_process(u8 priority, u32 csize, u32 dsize, u64 t)
{
	proc new_process = malloc(sizeof(*new_process));

	new_process->_vas = csize + dsize;

	int enough_space = vas_alloc(new_process->_sbt, new_process->_vas);

	if (enough_space)
	{
		new_process->_priority = priority;
		new_process->_time = t;

		new_process->_code_addr = 0;
	  new_process->_code_time = new_code_time();

		new_process->_data_addr = NULL;
		new_process->_data_time = new_data_time();

		u16 alloc = page_alloc();

		if (!alloc)
		{
			u16 swap_page = walk_page_ring();
			page_free(swap_page);
			alloc = page_alloc();
		}

		new_process->_pid

		ready_enq(new_process);
	}
	else
	{
		free(new_process);
	}
}

// Checks ready queues first, 4 from high, 2 from medium, 1 from low
// Then looks through blocked to see if anything is finished
void scheduler()
{
	if (counter < 4)
	{
		ready_deq(1);
		counter++;
	}
	else if (counter >= 4 && counter < 7)
	{
		ready_deq(2);
		counter++;
	}
	else
	{
		ready_deq(3);
		counter = 0;
	}
}
