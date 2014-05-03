#include <stdio.h>
#include <stdlib.h>
#include "types.h"
//#include "pid.h"
#include "mem_man.h"
#include "proc.h"

static u64	time = 0;
static u64 time_blocked = 0;

// Create all of the queues
static blocked _blocked;
static ready _high;
static ready _medium;
static ready _low;

static int counter = 0;

// initial address, process time, time prediction, priority, and something else

void blocked_enq(proc p, u64 estimated_time)
{
	p->_time = time + estimated_time;
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
	proc current = malloc(sizeof(*current));
	proc previous = malloc(sizeof(*previous)); 
	proc next = malloc(sizeof(*next));

	if (_blocked._head != NULL)
	{
		if (current->_time <= time)
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

			if (current->_time <= time)
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
	proc p;

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

	return p;
}

// void process_execute(process * p, u64 x)
// {
// 	while (time < x) {
// 		time += (*profiles[1]) (code_size, *code_pointer);

// 		if (!mem_read(p, code_pointer)) {
// 			blocked_enq(p);
// 			return;
// 		}

// 		time += (*profiles[2]) (data_size, *data_pointer);

// 		if (!mem_read(p, data_pointer)) {
// 			blocked_enq(p);
// 			return;
// 		}

// 		time += (*profiles[1]) (code_size, *data_pointer);

// 		if (!mem_write(p, code_pointer)) {
// 			blocked_enq(p);
// 			return;
// 		}

// 		time += (*profiles[2]) (data_size, *data_pointer);

// 		if (!mem_read(p, data_pointer)) {
// 			blocked_enq(p);
// 			return;
// 		}
// 	}

// 	ready_enq(2 p);

// 	cbar.comtime += (*profiles[2]) (data_size, *data_pointer);

// 	if (!mem_read(p, data_pointer)) {
// 		blocked_enq(p);
// 		return;
// 	}
// }

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
