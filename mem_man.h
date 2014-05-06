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

void read_page(u16 y);
void write_page(u16 y);
page get_page(u32 addr);

void set_used(u16 y);
void set_pinned(u16 y);
void clear_pinned(u16 y);

void insert_address(u16 y, int index, u16 addr);
u16 get_address(u16 y, int index);

u16 page_alloc();
void page_free(u16 x);
void emancipation_proclamation();

int vas_alloc(u16 v[], u32 size);
void vas_free(u16 v[], u32 size);

u32 virt_to_phys(u32 addr, proc p);
void page_fault(u32 addr, proc p);

u16 walk_page_ring();

#endif
