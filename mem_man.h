#ifndef	_MEM_MAN__H_
#define	_MEM_MAN__H_

#include "types.h"
#include "proc.h"

typedef struct {
	u64 _used:1;      // Page is being used
	u64 _dirty:1;     // Data has been changed
	u64 _pinned:1;    // Can not be swapped out
	u64:13;           // Padding
	u64 _l1i:10;      // Level 1 address
	u64:6;            // Padding
	u64 _l2i:10;      // Level 2 address
	u64:6;            // Padding
	u64 _proc:16;     // Process id?????!?
} mem_manage;

void read_page(page * x, u16 y);
void write_page(page * x, u16 y);

void build_mem_avail();

u16 page_alloc();
void page_free(u16 x);

int vas_alloc(u16 v[], u32 size);
void vas_free(u16 v[], u32 size);

#endif
