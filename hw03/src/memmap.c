#include <memmap.h>

void print_memmap(struct map_item_conv_format *map, uint32_t map_length) {
	for (uint32_t i = 0; i < map_length; i++) {
		printf("Memory from %lx to %lx type %u\n", map[i].begin, 
			map[i].end, map[i].type);
	}
}

void set_record(struct map_item_conv_format *record, uint32_t new_type,
		uint64_t new_begin, uint64_t new_end) {
	record->type = new_type;
	record->begin = new_begin;
	record->end = new_end;
}

int convert_memmap(struct map_item *map, uint32_t map_size, 
		struct map_item_conv_format *new_map) {
	extern char text_phys_begin[8];
	extern char bss_phys_end[8];
	uint64_t taken_begin = (uint64_t)text_phys_begin;
	uint64_t taken_end = (uint64_t)bss_phys_end;
	
	uint32_t passed = 0;
	struct map_item_conv_format *new_record = new_map;
	struct map_item *old_record = map;
	while (passed < map_size) {
		if (taken_begin <= old_record->addr) {
			if (taken_end >= old_record->addr + old_record->length) {
				set_record(new_record, 2, old_record->addr, 
					old_record->addr + old_record->length);
				new_record++;
			}
			else if (taken_end > old_record->addr) {
				set_record(new_record, 2, old_record->addr, taken_end);
				new_record++;
				
				set_record(new_record, old_record->type, taken_end, 
					old_record->addr + old_record->length);
				new_record++;
			}
			else {
				set_record(new_record, old_record->type, old_record->
					addr, old_record->addr + old_record->length);
				new_record++;
			}
		}
		else {
			if (taken_begin >= old_record->addr + old_record->length) {
				set_record(new_record, old_record->type, old_record->
					addr, old_record->addr + old_record->length);
				new_record++;
			}
			else if (taken_end >= old_record->addr + 
					old_record->length) {
				set_record(new_record, old_record->type, 
					old_record->addr, taken_begin);
				new_record++;
				
				set_record(new_record, 2, taken_begin, 
					old_record->addr + old_record->length);
				new_record++;
			}
			else {
				set_record(new_record, old_record->type, 
					old_record->addr, taken_begin);
				new_record++;
				
				set_record(new_record, 2, taken_begin, taken_end);
				new_record++;
				
				set_record(new_record, old_record->type, taken_end, 
					old_record->addr + old_record->length);
				new_record++;
			}
		}
		passed += old_record->size + 4;
		old_record = (struct map_item *)((char*)old_record + 
			old_record->size + 4);
	}
	
	return new_record - new_map;
}
