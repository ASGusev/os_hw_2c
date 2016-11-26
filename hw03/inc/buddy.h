#ifndef __BUDDY_H__
#define __BUDDY_H__

#include <memmap.h>
#include <exclusion.h>

#define MAX_MEMORY_SIZE (16ll << 30)
#define BLOCK_SIZE (4ll << 10)
#define BAD_POINTER (void*)0xF0F0F0F0F0F0F0F0

void buddy_init(struct map_item_conv_format *memmap, int memmap_size);

void* buddy_get_block();

void buddy_free_block(void* block);

#endif /* ___BUDDY_H__ */
