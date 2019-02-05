/*
	interrupts.hh
	-------------
*/

#ifndef INTERRUPTS_HH
#define INTERRUPTS_HH

inline
asm short disable_interrupts()
{
	JSR      0xFFFFFFFA  // enter_supervisor_mode()
	ORI      #0x0700,SR  // mask all interrupts (except NMI)
	ANDI     #0xDFFF,SR  // exit supervisor mode
	
	// old SR (with old interrupt mask) is in D0
}

inline
asm void reenable_interrupts( short previous_SR : __D1 )
{
	JSR      0xFFFFFFFA  // enter_supervisor_mode()
	MOVE     D1,SR       // restore old SR (with old interrupt mask)
}

#endif
