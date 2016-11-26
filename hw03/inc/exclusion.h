#ifndef __EXCLUSION_H__
#define __EXCLUSION_H__

#define LOCKED 1
#define UNLOCKED 0

struct exclusion {
	volatile int locked;
};

void lock(struct exclusion*);

void unlock(struct exclusion*);

void init_exclusion(struct exclusion*);

#endif /* __EXCLUSION_H__ */
