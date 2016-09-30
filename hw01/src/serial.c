#include <serial.h>

void send_char(uint8_t c) {
	while (!(in8(SERIAL_ADDR + 5) & bit(5)));
	uint8_t settings = in8(SERIAL_ADDR + 3);
	settings &= ~((uint8_t)bit(7));
	out8(SERIAL_ADDR + 3, settings);
	out8(SERIAL_ADDR, c);
}

void send_string(char *s) {
	while (*s) {
		send_char(*s);
		s++;
	}
}

void setup_serial() {
	out8(SERIAL_ADDR + 1, 0);
	out8(SERIAL_ADDR + 3, bit(7));
	out8(SERIAL_ADDR, bit(2));
	out8(SERIAL_ADDR + 1, 0);
	out8(SERIAL_ADDR + 3, bit(0) | bit(1));
}
