#ifndef __MEMMAP_H__
#define __MEMMAP_H__

#include <stdint.h>
#include <print.h>

struct map_item {
	uint32_t size;
	uint64_t addr;
	uint64_t length;
	uint32_t type;
} __attribute__((packed));

struct map_item_conv_format {
	uint32_t type;
	uint64_t begin;
	uint64_t end;
};

struct multiboot_info {
	uint32_t flags;
	
	uint32_t mem_lower;
	uint32_t mem_upper;
	
	uint32_t boot_device;
	
	uint32_t cmdline;
	
	uint32_t mods_count;
	uint32_t mods_addr;
 
	char placeholder[16];
   
	uint32_t mmap_length;
	uint32_t mmap_addr;
	
	uint32_t drives_length;
	uint32_t drives_addr;
	
	uint32_t config_table;
	
	uint32_t boot_loader_name;
	
	uint32_t apm_table;
	
	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;
} __attribute__((packed));

#define MAX_MEMMAP_LENGTH 128

void print_memmap(struct map_item_conv_format *map, uint32_t map_size);

int convert_memmap(struct map_item *map, uint32_t map_size, 
		struct map_item_conv_format *new_map);

#endif /* __MEMMAP_H__ */
