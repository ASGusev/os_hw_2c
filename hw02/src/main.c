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

extern const uint32_t multiboot_info_addr;

struct map_item_conv_format memory_map[MAX_MEMMAP_LENGTH];

void main()
{
	struct multiboot_info *info = 
		(struct multiboot_info *)(uint64_t)multiboot_info_addr;
	
	struct map_item *map_pos = 
		(struct map_item*)(uint64_t)(info->mmap_addr);
	
	int map_length = convert_memmap(map_pos, info->mmap_length, 
		memory_map);
	
	print_memmap(memory_map, map_length);
	
	buddy_init(memory_map, map_length);
	
	//qemu_gdb_hang();

	//struct desc_table_ptr ptr = {0, 0};

	//write_idtr(&ptr);

	while (1);
}
