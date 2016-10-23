#include <buddy.h>

#define MAX_BLOCKS (MAX_MEMORY_SIZE / BLOCK_SIZE)

uint8_t blocks_tree[MAX_BLOCKS / 2];

#define set_bit(var, bit, pos) \
	var = (var & ~(1 << pos)) | (bit << pos)
	
#define get_bit(var, pos) \
	(var >> pos) & 1

void set_full(uint64_t node_id, uint8_t full) {
	uint64_t cell_id = node_id >> 2;
	uint64_t shift = (node_id & 3ll) << 1;
	set_bit(blocks_tree[cell_id], full, shift);
}

void set_empty(uint64_t node_id, uint8_t empty) {
	uint64_t cell_id = node_id >> 2;
	uint64_t shift = ((node_id & 3ll) << 1) + 1;
	set_bit(blocks_tree[cell_id], empty, shift);
}

uint8_t is_full(uint64_t node_id) {
	uint64_t cell_id = node_id >> 2;
	uint64_t shift = (node_id & 3ll) << 1;
	return get_bit(blocks_tree[cell_id], shift);
}

uint8_t is_empty(uint64_t node_id) {
	uint64_t cell_id = node_id >> 2;
	uint64_t shift = ((node_id & 3ll) << 1) + 1;
	return get_bit(blocks_tree[cell_id], shift);
}

void mark_group(uint64_t pos, uint64_t cur_lb, uint64_t cur_rb, 
		uint64_t group_lb, uint64_t group_rb) {
	if (cur_lb >= group_rb || cur_rb <= group_lb) {
		return;
	}
	if (cur_lb >= group_lb && cur_rb <= group_rb) {
		set_full(pos, 1);
		set_empty(pos, 0);
		return;
	}
	int mid = (cur_lb + cur_rb) / 2;
	
	if (is_empty(pos)) {
		set_empty(pos * 2, 1);
		set_empty(pos * 2 + 1, 1);
	}
	
	if (is_full(pos)) {
		set_full(pos * 2, 1);
		set_full(pos * 2 + 1, 1);
	}
	
	mark_group(pos * 2, cur_lb, mid, group_lb, group_rb);
	mark_group(pos * 2 + 1, mid, cur_rb, group_lb, group_rb);
	set_full(pos, is_full(pos * 2) & is_full(pos * 2 + 1));
	set_empty(pos, is_empty(pos * 2) & is_empty(pos * 2 + 1));
}

void buddy_init(struct map_item_conv_format *memmap, int memmap_size) {
	set_full(1, 0);
	set_empty(1, 1);
	for (int i = 0; i < memmap_size; i++) {
		uint64_t cut_beginning = (memmap[i].begin + BLOCK_SIZE - 1) / 
			BLOCK_SIZE;
		uint64_t cut_end = memmap[i].end / BLOCK_SIZE;
		if (memmap[i].type == 1 && cut_end > cut_beginning) {
			mark_group(1, 0, MAX_BLOCKS, cut_beginning, cut_end);
		}
	}
}

uint64_t get_block(uint64_t node_id, uint64_t cur_lb, uint64_t cur_rb) {
	if (cur_rb - cur_lb == 1) {
		set_full(node_id, 0);
		set_empty(node_id, 1);
		return cur_lb;
	}
	
	if (is_full(node_id)) {
		set_full(node_id * 2, 1);
		set_full(node_id * 2 + 1, 1);
		set_empty(node_id * 2, 0);
		set_empty(node_id * 2 + 1, 0);
	}
	
	uint64_t block = 0;
	uint64_t mid = (cur_lb + cur_rb) / 2;
	
	if (!is_empty(node_id * 2)) {
		block = get_block(node_id * 2, cur_lb, mid);
	}
	else {
		block = get_block(node_id * 2 + 1, mid, cur_rb);
	}
	
	set_empty(node_id, is_empty(node_id * 2) & 
		is_empty(node_id * 2 + 1));
	set_full(node_id, 0);
	
	return block;
}

void* buddy_get_block() {
	if (is_empty(1)) {
		return BAD_POINTER;
	}
	return (void*)(get_block(1, 0, MAX_BLOCKS) * BLOCK_SIZE);
}

void buddy_free_block(void* pointer) {
	uint64_t block = (uint64_t)pointer / BLOCK_SIZE;
	uint64_t node_id = block + MAX_BLOCKS;
	set_empty(node_id, 0);
	while (node_id > 1) {
		node_id /= 2;
		set_empty(node_id, 0);
		set_full(node_id, is_full(node_id * 2) & 
			is_full(node_id * 2 + 1));
	}
}
