#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include <ints.h>
#include <desc.h>
#include <memory.h>
#include <serial.h>

#define IDT_SIZE 256
#define MAIN_PIC_CMD_ADDR 0x20
#define MAIN_PIC_DATA_ADDR 0x21
#define SLAVE_PIC_CMD_ADDR 0xA0
#define SLAVE_PIC_DATA_ADDR 0xA1
#define MAIN_PIC_ZERO_INTERRUPT 32

struct idt_rec {
	uint16_t offset2;
	uint16_t segment_selector;
	uint16_t attributes;
	uint16_t offset1;
	uint32_t offset0;
	uint32_t reserved0;
} __attribute__((packed));

void setup_idt();

void setup_pic();

void generate_interrupt();

void set_interrupt(int, uint64_t);

#endif /* __INTERRUPTS_H__ */
