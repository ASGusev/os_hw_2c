#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <ioport.h>

#define SERIAL_ADDR 0x3f8

#define bit(x) 1 << x

void setup_serial();

void send_string(char*);

void send_char(uint8_t);

#endif /* __SERIAL_H__ */
