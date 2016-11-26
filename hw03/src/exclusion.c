#include <exclusion.h>
#include <ints.h>

void lock(struct exclusion *excl) {
	while (1) {
		disable_ints();
		if (excl->locked == UNLOCKED) {
			excl->locked = LOCKED;
			enable_ints();
			return;
		}
		enable_ints();
	}
}

void unlock(struct exclusion *excl) {
	excl->locked = UNLOCKED;
}

void init_exclusion(struct exclusion *excl) {
	excl->locked = UNLOCKED;
}
