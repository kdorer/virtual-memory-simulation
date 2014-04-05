#include <stdio.h>
#include "types.h"
#include "proc.h"
#include "mem_man.h"
#include "util.h"

#define	PAGE_COUNT 65536
#define	PAGE_MASK	(PAGE_COUNT - 1)
#define	VAS_VEC_SIZE (1 << 6)
#define	VAS_VEC_SIZE_MASK (VAS_VEC_SIZE - 1)

// Array of pages
static page mem[PAGE_COUNT];

// first available page
static u16 page_avail = 0;

// Status control of memory
static mem_manage mem_man[PAGE_COUNT] = { 0 };

static u16 mem_offset = 1;
static u64 vas_vec[VAS_VEC_SIZE] = { 0 };

static u32 vas_offset = 0;
static u32 vas_count = VAS_VEC_SIZE;

// Read the contents of page y in memory into page x.
void read_page(page * x, u16 y)
{
	u32 i;
	for (i = 0; i < 512; ++i) {
		x->_u64[i] = mem[y]._u64[i];
	}
}

// Writing the contents of page x into page y in memory. 
void write_page(page * x, u16 y)
{
	u32 i;
	for (i = 0; i < 512; ++i) {
		mem[y]._u64[i] = x->_u64[i];
		// Should change dirty bit here because content has been changed?
	}
}

// Creates page at page_avail, if page_avail is not 0. 
// Page_avail is set to the allocated pages _u16.
// Return the address of the newly allocated page.
u16 page_alloc()
{
	u16 t = page_avail;
	if (page_avail) {
		page_avail = mem[page_avail]._u16[0];
	}
	return t;
}

// Frees page x.
// Page x's _u16 is set to the current page_avail.
// page_avail is then set to the address of the new page.
void page_free(u16 x)
{
	mem[x]._u16[0] = page_avail;
	page_avail = x;
}

// Size is pages you want, array is pages found?
int vas_alloc(u16 v[], u32 size)
{

}

//
void vas_free(u16 v[], u32 size)
{

}

// 
u16 walk_page_ring()
{

}
