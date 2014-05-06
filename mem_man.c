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

// Bitmap. 64 levels of 64 bits. each bit is a chunk of 4mb that represents a page table. 
static u64 vas_vec[VAS_VEC_SIZE] = { 0 };

static u32 vas_offset = 0;
static u32 vas_count = VAS_VEC_SIZE;

// Read the contents of page y in memory into page x.
void write_page(page * x, u16 y)
{
	u32 i;
	for (i = 0; i < 512; ++i) {
		x->_u64[i] = mem[y]._u64[i];
	}
}

// Writing the contents of page x into page y in memory. 
void read_page(page * x, u16 y)
{
	u32 i;
	for (i = 0; i < 512; ++i) {
		mem[y]._u64[i] = x->_u64[i];
		// Should change dirty bit here because content has been changed?
	}
}

page get_page(u32 addr)
{
	return mem[addr];
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

//
u32 virt_to_phys(u32 addr, proc p)
{
	u32 l1_index = addr >> 22;
	u32 l2_index = ((addr >> 12) & 0x3FF);

	u16 l1_addr = p->_pid;
	page l1 = mem[l1_addr];
	u32 l2_addr = l1._u32[l1_index];

	if (!l2_addr)
	{
		return 0;
	}

	page l2 = mem[l2_addr];
	u32 phys_addr = l2._u32[l2_index];

	if (!phys_addr)
	{
		return 0;
	}
	return phys_addr;
}

// Array is the sbt from proc(I believe), size is the number of chunks a process wants.
int vas_alloc(u16 v[], u32 size)
{
	int result = 0;

	if(size <= vas_count)
	{
		for(int i = 0; i < size; i++)
		{
			while(vas_vec[vas_offset] == 0xFFFFFFFFFFFFFFFF)
			{
				vas_offset++;
				if(vas_offset > 63)
				{
					vas_offset = 0;
				}
			}

			// Find a free chunk and record its position
			u16 bit_pos = (u16) lsb64(vas_vec(vas_offset));
			// Create an address of the chunk level index and position
			u16 chunk_address = (vas_offset << 8) | (bit_pos);
			// Store address in the passed in array
			v[i] = chunk_address;

			u64 flipped_bit = 1;
			flipped_bit = flipped_bit << bit_pos;

			// Flip the bit at vas_offset and bit_post to indicate allocated memory
			vas_vec[vas_offset] = vas_vec[vas_offset] | (flipped_bit);

			// If the row is completely allocated
			vas_count--;
		}

		result = 1;
	}

	// result: 0 is failure, 1 is success
	return result;
}

// Array is the sbt from proc(I believe), size is the number of chunks a process wants.
void vas_free(u16 v[], u32 size)
{
	for(int i = 0; i < size; i++)
	{
		// Get an address of a chunk to be free
		u16 chunk_address = v[i];
		u16 bit_pos = chunk_address & 0x00FF;
		u16 vas_offset_temp = chunk_address >> 8;

		u64 flipped_bit = 1;
		flipped_bit = flipped_bit << bit_pos;
		vas_vec[vas_offset_temp] = vas_vec[vas_offset_temp] & ~(flipped_bit);

		vas_count++;
	}
}

// Iterate through memory and do stuff:
// Set used to 0 for each page.
u16 walk_page_ring()
{
	u16 temp = 0;
	for(int i; i < PAGE_COUNT; i++)
	{
		if(!mem_man[i])
		{
			temp = i;
			break;
		}
		mem_man[i] = mem_man[i]._used & 0;
	}
	return temp;
}
