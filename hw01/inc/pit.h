#ifndef __PIT_H__
#define __PIT_H__

#include <interrupts.h>

#define PIT_CMD_PORT 0x43
#define PIT0_DATA_PORT 0x40
#define FREQUENCY_DIVISOR (unsigned short)0xFFFF

void set_pit();

#endif /* __PIT_H__ */
