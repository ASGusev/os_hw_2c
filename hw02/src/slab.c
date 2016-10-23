#include <slab.h>

#define BAD_OBJECT_ID 0xFFFF
#define OBJECT_ID_MASK 0xFFFll
#define GAP_MASK 0xFFFFFF8000000000ll

#define skip_gap(ptr) \
	ptr = (void*)((uint64_t)ptr ^ GAP_MASK)

struct slab_allocator* slab_create(uint16_t size) {
	if (size == 1) {
		size = 2;
	}
	struct slab_allocator* allocator = buddy_get_block();
	if (allocator == BAD_POINTER) {
		return allocator;
	}
	allocator->counter = 0;
	allocator->object_size = size;
	allocator->prev = allocator;
	allocator->next = allocator;
	allocator->first = sizeof(struct slab_allocator);
	char* pos = (char*)((uint64_t)allocator->first | (uint64_t)allocator);
	while (pos - (char*)allocator + 2 * size <= BLOCK_SIZE) {
		*((uint16_t*)((uint64_t)pos)) = (uint16_t)((uint64_t)
			(pos + size) & OBJECT_ID_MASK);
		pos += size;
	}
	*((uint16_t*)pos) = BAD_OBJECT_ID;
	return allocator;
}

void *get_object(struct slab_allocator* allocator) {
	allocator->counter++;
	void *object = (void*)((uint64_t)allocator | 
		(uint64_t)allocator->first);
	allocator->first = *((uint16_t*)((uint64_t)allocator->first | 
		(uint64_t)allocator));
	return object;
}

void *slab_get(struct slab_allocator* allocator) {
	void *object = BAD_POINTER;
	
	if (allocator->object_size + sizeof(struct slab_allocator) > 
		BLOCK_SIZE) {
		object = buddy_get_block();
		skip_gap(object);
		return object;
	}
	if (allocator->first == BAD_OBJECT_ID) {
		if (allocator->next->first == BAD_OBJECT_ID) {
			struct slab_allocator* new_slab = 
				slab_create(allocator->object_size);
			if (new_slab == BAD_POINTER) {
				return BAD_POINTER;
			}
			new_slab->next = allocator->next;
			new_slab->prev = allocator;
			new_slab->next->prev = new_slab;
			new_slab->prev->next = new_slab;
		}
		
		object = get_object(allocator->next);
		skip_gap(object);
		
		if (allocator->next->first == BAD_OBJECT_ID) {
			struct slab_allocator *next_allocator = allocator->next;
			next_allocator->prev->next = next_allocator->next;
			next_allocator->next->prev = next_allocator->prev;
			
			next_allocator->prev = allocator->prev;
			next_allocator->next = allocator;
			next_allocator->prev->next = next_allocator;
			next_allocator->next->prev = next_allocator;
		}
		
		return object;
	}
	
	object = get_object(allocator);
	skip_gap(object);
	
	return object;
}

void slab_return(struct slab_allocator *allocator, void *object) {
	skip_gap(object);
	if (allocator->object_size + sizeof(struct slab_allocator) > 
		BLOCK_SIZE) {
		buddy_free_block(object);
		return;
	}
	struct slab_allocator *allocator_node = (struct slab_allocator*)
		((uint64_t)object & ~OBJECT_ID_MASK);
	uint16_t object_id = ((uint64_t)object & OBJECT_ID_MASK);
	
	*(uint16_t*)object = allocator_node->first;
	allocator_node->first = object_id;
	allocator_node->counter--;
	
	if (allocator_node != allocator && allocator_node->counter == 0) {
		allocator_node->prev->next = allocator_node->next;
		allocator_node->next->prev = allocator_node->prev;
		buddy_free_block(allocator_node);
	}
	else if (allocator_node != allocator && allocator_node != 
		allocator->next) {
		allocator_node->prev->next = allocator_node->next;
		allocator_node->next->prev = allocator_node->prev;
		
		allocator_node->prev = allocator;
		allocator_node->next = allocator->next;
		allocator_node->prev->next = allocator_node;
		allocator_node->next->prev = allocator_node;
	}
}
