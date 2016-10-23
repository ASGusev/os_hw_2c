#ifndef __SLAB_H__
#define __SLAB_H__

#include <buddy.h>

struct slab_allocator {
	struct slab_allocator *prev;
	struct slab_allocator *next;
	uint16_t counter;
	uint16_t first;
	uint16_t object_size;
} __attribute__((packed));

struct slab_allocator* slab_create(uint16_t size);

void *slab_get(struct slab_allocator* allocator);

void slab_return(struct slab_allocator *allocator, void *object);

#endif /* __SLAB_H__ */
