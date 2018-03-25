/*
	interrupts.cc
	-------------
*/

#include "interrupts.hh"

// ams-core
#include "splode.hh"


void* event_poll_interrupt_vector : 0x0100;  // Vector 64 (user interrupt 0)

static
asm void event_poll_interrupt_handler()
{
	MOVEM.L  D0-D2/A0-A1,-(SP)
	
	ANDI     #0xDFFF,SR  // leave supervisor mode
	
	JSR      poll_user_input
	
	JSR      0xFFFFFFFA  // enter_supervisor_mode()
	
	MOVEM.L  (SP)+,D0-D2/A0-A1
	RTE
}

asm int set_event_poll_interrupt_handler()
{
	JSR      0xFFFFFFFA  // enter_supervisor_mode()
	BMI.S    bail        // D0 is -1 if branch taken
	
	LEA      event_poll_interrupt_handler,A0
	
	MOVE.L   A0,event_poll_interrupt_vector
	
	MOVE     D0,SR
	MOVEQ.L  #0,D0
	
bail:
	RTS
}
