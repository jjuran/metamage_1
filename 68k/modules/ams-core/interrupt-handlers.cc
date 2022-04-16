/*
	interrupt-handlers.cc
	---------------------
*/

#include "interrupt-handlers.hh"

// ams-core
#include "splode.hh"


void* event_poll_interrupt_vector    : 0x0100;  // Vector 64 (user interrupt 0)
void* sploop_detect_interrupt_vector : 0x0160;  // Vector 88 (user interrupt 24)

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

/*
	Spinloop structure:
	
	4A6D 1234 | TST.W    (0x1234,A5)
	66FA      | BNE.S    *-4
	
	4A00: TST
	0040: .W
	0028: addressing mode 5
	0005: register 5
	
	Mitigation:
	
	486D 1234 | PEA      (0x1234,A5)
	A2F6      | _WaitUntilWordIsZero
*/

static
asm void spinloop_detect_interrupt_handler()
{
	MOVE.L   D0,-(SP)
	MOVE.L   A0,-(SP)
	
//	LEA      8(SP),A0   // exception stack frame
	MOVEA.L  10(SP),A0  // exception stack frame's stacked PC
	
	MOVE.W   (A0),D0        // copy the TST opcode
	ANDI.W   #0x003F,(A0)   // keep only the effective address mode
	ORI.W    #0x4840,(A0)   // replace prior TST opcode with PEA
	ANDI.W   #0x00C0,D0     // isolate size code
	LSL.W    #3,D0          // Shift size code from $00C0 to $0600
	ORI.W    #0xA0F6,D0     // compose trap word
	MOVE.W   D0,4(A0)       // replace BNE opcode with trap
	
	MOVEA.L  (SP)+,A0
	MOVE.L   (SP)+,D0
	RTE
}

asm int install_interrupt_handlers()
{
	JSR      0xFFFFFFFA  // enter_supervisor_mode()
	BMI.S    bail        // D0 is -1 if branch taken
	
	LEA      event_poll_interrupt_handler,A0
	MOVE.L   A0,event_poll_interrupt_vector
	
	LEA      spinloop_detect_interrupt_handler,A0
	MOVE.L   A0,sploop_detect_interrupt_vector
	
	MOVE     D0,SR
	MOVEQ.L  #0,D0
	
bail:
	RTS
}
