/*
	Printing.cc
	-----------
*/

#include "Printing.hh"

#ifndef __TRAPS__
#include <Traps.h>
#endif

/*
	Stack on entry to PrGlue_patch():
	
	:                :
	+----------------+
	| arguments ...  |
	+----------------+
	| selector       |  4 bytes
	+----------------+
	| return address |  4 bytes
	+----------------+
	
	PC = PrGlue_patch
*/

/*
	Stack before calling select_handler():
	
	:                :
	+----------------+
	| arguments ...  |
	+----------------+
	| return address |  4 bytes
	+----------------+
	
	PC = PrGlue_patch + 6
*/

static
pascal void PrOpen_handler()
{
}

static
pascal void PrClose_handler()
{
}

static
pascal short PrError_handler()
{
	return 0;
}

static
pascal void PrintDefault_handler( char** )
{
}

static
void* select_handler( long selector : __D0 )
{
	switch ( selector )
	{
		case 0xC8000000:  return &PrOpen_handler;
		case 0xBA000000:  return &PrError_handler;
		case 0x20040480:  return &PrintDefault_handler;
		case 0xD0000000:  return &PrClose_handler;
		
		default:
			return 0;
	}
}

asm void PrGlue_patch()
{
	MOVE.L   4(SP),D0
	MOVE.L   (SP)+,(SP)
	
	JSR      select_handler
	
	JMP      (A0)
}
