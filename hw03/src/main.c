/*
static void qemu_gdb_hang(void)
{
#ifdef DEBUG
	static volatile int wait = 1;

	while (wait);
#endif
}
*/

#include <memmap.h>
#include <print.h>
#include <buddy.h>
#include <slab.h>
#include <pdt.h>
#include <interrupts.h>

extern const uint32_t multiboot_info_addr;

struct map_item_conv_format memory_map[MAX_MEMMAP_LENGTH];

void test_buddy() {
	int correct = 1;
	
	uint64_t **pointers = buddy_get_block();
	for (uint64_t i = 0; i < 512; i++) {
		pointers[i] = buddy_get_block();
		if ((uint64_t)pointers[i] % BLOCK_SIZE != 0ull) {
			correct = 0;
		}
		for (uint64_t j = 0; j < 512; j++) {
			pointers[i][j] = i;
		}
	}
	
	for (uint64_t i = 0; i < 512; i++) {
		if (pointers[i][i] != i) {
			correct = 0;
		}
		buddy_free_block(pointers[i]);
	}
	buddy_free_block(pointers);
	
	if (correct) {
		printf("Buddy test was passed.\n");
	}
	else {
		printf("Buddy test was not passed.\n");
	}
}

struct simple_list {
	struct simple_list* next;
	uint64_t val;
};

void delete_next(struct simple_list *pos, struct slab_allocator *alloc) {
	struct simple_list *temp = pos->next;
	pos->next = pos->next->next;
	slab_return(alloc, temp);
}

void test_slab() {
	struct slab_allocator *alloc = slab_create(sizeof(struct simple_list));
	struct simple_list head;
	struct simple_list *pos = &head;
	int correct = 1;
	for (uint64_t i = 0; i < 4000; i++) {
		pos->next = slab_get(alloc);
		pos = pos->next;
		pos->val = i;
	}
	pos->next = BAD_POINTER;
	
	pos = &head;
	while (pos->next != BAD_POINTER) {
		delete_next(pos, alloc);
		pos = pos->next;
	}
	
	pos = &head;
	for (uint64_t i = 0; i < 1000; i++) {
		pos = pos->next;
	}
	for (uint64_t i = 0; i < 1000; i++) {
		delete_next(pos, alloc);
	}
	
	for (uint64_t i = 0; i < 1000; i++) {
		struct simple_list *temp = slab_get(alloc);
		temp->val = i;
		temp->next = head.next;
		head.next = temp;
	}
	
	pos = &head;
	for (uint64_t i = 999; i < 1000; i--) {
		pos = pos->next;
		if (pos->val != i) {
			correct = 0;
		}
	}
	for (uint64_t i = 1; i < 2000; i += 2) {
		pos = pos->next;
		if (pos->val != i) {
			correct = 0;
		}
	}
	
	while (head.next != BAD_POINTER) {
		delete_next(&head, alloc);
	}
	
	if (correct) {
		printf("Slab test was passed.\n");
	}
	else {
		printf("Slab test was not passed.\n");
	}
}

void main()
{
	
	setup_idt();
	
	setup_pic();
	
	struct multiboot_info *info = 
		(struct multiboot_info *)(uint64_t)multiboot_info_addr;
	
	struct map_item *map_pos = 
		(struct map_item*)(uint64_t)(info->mmap_addr);
	
	int map_length = convert_memmap(map_pos, info->mmap_length, 
		memory_map);
	
	print_memmap(memory_map, map_length);
	
	buddy_init(memory_map, map_length);
	
	make_pdt();
	
	test_buddy();
	
	test_slab();
	
	//qemu_gdb_hang();

	//struct desc_table_ptr ptr = {0, 0};

	//write_idtr(&ptr);

	while (1);
}
