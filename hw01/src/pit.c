#include <pit.h>

extern void timer_interrupt_handler();

void print_pit_message() {
	send_string("Timer interrupt happened.\n");
}

void set_pit() {
	set_interrupt(MAIN_PIC_ZERO_INTERRUPT, (uint64_t)timer_interrupt_handler);
	
	out8(PIT_CMD_PORT, bit(4) | bit(5) | bit(2));
	out8(PIT0_DATA_PORT, FREQUENCY_DIVISOR && 0x00FF);
	out8(PIT0_DATA_PORT, FREQUENCY_DIVISOR >> 8);
	out8(MAIN_PIC_DATA_ADDR, 0xFE);
}
