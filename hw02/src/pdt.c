#include <pdt.h>

#define TABLE_LENGTH 512

void make_pdt() {
	uint64_t* pdpt = buddy_get_block();
	for (uint64_t i = 0; i < TABLE_LENGTH; i++) {
		pdpt[i] = (i << 30) | 7ull | 128ull;
	}
	
	uint64_t* pml4 = buddy_get_block();
	for (uint64_t i = 0; i < TABLE_LENGTH; i++) {
		pdpt[i] = 0;
	}
	pml4[0] = (unsigned long long)pdpt | 7ull;
	pml4[TABLE_LENGTH - 1] = pml4[0];
	__asm__ volatile("mov %0, %%cr3" : "=r"(pml4) : );
}
