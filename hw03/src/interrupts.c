#include <interrupts.h>

struct idt_rec idt[IDT_SIZE];

//extern void exception_handler();

void print_exception_message() {
	send_string("Software interrupt happened.\n");
}

void init_record(struct idt_rec *record) {
	record->reserved0 = 0;
	record->offset0 = 0;
	record->offset1 = 0;
	record->attributes = 0;
	record->segment_selector = 0;
	record->offset2 = 0;
}

void set_interrupt(int int_number, uint64_t handler_addr) {
	struct idt_rec *record = idt + int_number;
	record->offset0 = handler_addr >> 32;
	record->offset1 = handler_addr >> 16;
	record->attributes = bit(15) | ((uint16_t)0x0F << 8);
	record->segment_selector = KERNEL_CS;
	record->offset2 = handler_addr;
}

void setup_idt() {
	for (int i = 0; i < IDT_SIZE; i++) {
		init_record(idt + i);
	}
	
	struct desc_table_ptr descriptor;
	descriptor.size = IDT_SIZE * sizeof(struct idt_rec) - 1;
	descriptor.addr = (uint64_t)idt;
	write_idtr(&descriptor);
}

void setup_pic() {
	out8(MAIN_PIC_CMD_ADDR, bit(4) | bit(0));
	out8(MAIN_PIC_DATA_ADDR, MAIN_PIC_ZERO_INTERRUPT);
	out8(MAIN_PIC_DATA_ADDR, bit(2));
	out8(MAIN_PIC_DATA_ADDR, bit(0));
	
	out8(SLAVE_PIC_CMD_ADDR, bit(4) | bit(0));
	out8(SLAVE_PIC_DATA_ADDR, MAIN_PIC_ZERO_INTERRUPT + 8);
	out8(SLAVE_PIC_DATA_ADDR, 2);
	out8(SLAVE_PIC_DATA_ADDR, bit(0));
	
	out8(SLAVE_PIC_DATA_ADDR, 0xFF);
	out8(MAIN_PIC_DATA_ADDR, 0xFF);
	
	enable_ints();
}

void generate_interrupt() {
	__asm__ ("int $0x32");
}
