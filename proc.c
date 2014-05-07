#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "types.h"
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

	p->_blocked_timer = estimated_time;
	if (_blocked._head == NULL)
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
	if (_blocked._head == NULL)
	{
		return;
	}

	proc cp = _blocked._head;
    proc pp;

	if (_blocked._head->_next == NULL)
	{
		if (cp->_blocked_timer <= time_get())
		{
			printf("Removing process %d from the blocked queue\n", _blocked._head->_pid);
			ready_enq(_blocked._head);
			_blocked._head = NULL;
		}
	}

	else
	{
		do
		{
			if (cp->_blocked_timer <= time_get())
			{
				if (_blocked._head == cp)
				{
					_blocked._head = cp->_next;
				}

				pp->_next = cp->_next;
				cp->_next = NULL;
				ready_enq(cp);
				printf("Removing process %d from the blocked queue\n", cp->_pid);
			}

			else
			{
				pp = cp;
			}

			cp = cp->_next;
		} while(cp != NULL);
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
	//proc p = malloc(sizeof(*p));

    proc p;

	switch (priority)
	{
		case 1:

			if (_high._head == NULL)
			{
				return NULL;
			}

			p = _high._head;
			_high._head = p->_next;
			p->_next = NULL;
			printf("Scheduling a process from high priority queue\n");
			break;

		case 2:

			if (_medium._head == NULL)
			{
				return NULL;
			}

			p = _medium._head;
			_medium._head = p->_next;
			p->_next = NULL;
			printf("Scheduling a process from medium priority queue\n");
			break;

		case 3:

			if (_low._head == NULL)
			{
				return NULL;
			}

			p = _low._head;
			_low._head = p->_next;
			p->_next = NULL;
			printf("Scheduling a process from low priority queue\n");
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
u64 time_get()
{
	return time;
}

void set_time(u64 t)
{
	time =  t;
}

//
void process_exec (proc p)
{

	printf("Executing process %d\n", p->_pid);

	u32	code_trans = virt_to_phys(p->_code_addr, p);
	u32	data_trans = virt_to_phys(p->_data_addr, p);

	u64 timer = p->_time;

	if (!code_trans) 
	{
		page_fault(p->_code_addr, p);
		return;
	}

	set_used(code_trans);

	if (!data_trans) 
	{
		page_fault(p->_data_addr, p);
		return;
	}

	set_used(data_trans);

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

				p->_code_addr	= new_code_addr(p->_code_addr, p->_code_size);
				p->_code_time	= new_code_time();
				code_trans	= virt_to_phys(p->_code_addr, p);
			}

			if (!code_trans) 
			{
				page_fault(p->_code_addr, p);
				return;
			}

			set_used(code_trans);
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

				p->_data_addr	= new_data_addr(p->_data_addr, p->_code_size, p->_data_size);
				p->_data_time	= new_data_time();
				data_trans	= virt_to_phys(p->_data_addr, p);
			}

			if (!data_trans) 
			{
				page_fault(p->_data_addr, p);
				return;
			}

			set_used(data_trans);
		}
	}

	p->_run_counter--;

	if (!p->_run_counter)
	{
		vas_free(p->_sbt, p->_vas);

		int i;
		for (i = 0; i < p->_vas; i++)
		{
			u16 l2 = get_address(p->_pid, i);
			clear_pinned(l2);
		}
		clear_pinned(p->_pid);
		printf("Process %d has finished executing", p->_pid);

		free(p);
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
int init_process(u8 priority, u32 csize, u32 dsize, u64 t)
{
	proc np = malloc(sizeof(*np));

	np->_vas = ( ( ( (csize + dsize) / 1000) / 1000) / 4);

	int enough_space = vas_alloc(np->_sbt, np->_vas);

	if (enough_space)
	{
		np->_priority = priority;
		np->_time = t;

		np->_code_addr = 0;
	    np->_code_time = new_code_time();
	    np->_code_size = csize;

		np->_data_addr = csize + 1;
		np->_data_time = new_data_time();
		np->_data_size = dsize;	

		np->_run_counter = 5;
		np->_next = NULL;

		u16 alloc = page_alloc();

		if (!alloc)
		{
			u16 swap_page = walk_page_ring();
			page_free(swap_page);
			alloc = page_alloc();
		}

		np->_pid = alloc;
		set_pinned(alloc);

		int i;
		for(i = 0; i < np->_vas; i++)
		{
			u16 alloc = page_alloc();

			if (!alloc)
			{
				u16 swap_page = walk_page_ring();
				page_free(swap_page);
				alloc = page_alloc();
			}

			insert_address(np->_pid, i, alloc);
			set_pinned(alloc);
		}

		ready_enq(np);
		return 1;
	}
	else
	{
		return 0;
	}
}

// Checks ready queues first, 4 from high, 2 from medium, 1 from low
// Then looks through blocked to see if anything is finished
void scheduler()
{
    //printf( "we be schedualing dis shit yo");
	proc gp;
	set_time(time_get() + 100000000);
	blocked_deq();


    switch ( counter )
    {
        case 0:
        case 1:
        case 2:
        case 3:
        
            if ( (gp = ready_deq( 1 )) != NULL )
            {
                counter++;
                break;
            }

        case 4:
        case 5:
        case 6:

            if( (gp = ready_deq( 2 )) != NULL )
            {
                counter++;
                break;
            }
        
        case 7:

            if( (gp = ready_deq( 3 )) != NULL )
            {
                counter = 0;
            }
        default:
            break;
    }

	if (gp != NULL)
	{
		process_exec(gp);
	}
}
