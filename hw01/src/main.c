/*
static void qemu_gdb_hang(void)
{
#ifdef DEBUG
	static volatile int wait = 1;

	while (wait);
#endif
}
*/

#include <desc.h>
#include <ints.h>
#include <serial.h>
#include <interrupts.h>
#include <pit.h>

extern void error();
void main(void)
{
	setup_serial();	
	
	send_string("A message through serial port.\n");
	
	setup_idt();
	
	setup_pic();
	
	generate_interrupt();
	
	set_pit();
	
	//qemu_gdb_hang();

	//struct desc_table_ptr ptr = {0, 0};

	//write_idtr(&ptr);

	while (1);
}
