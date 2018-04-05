/*
	Pointers.cc
	-----------
*/

#include "Pointers.hh"


short MemErr : 0x0220;


asm
char* NewPtr_patch( long size : __D0, short trap_word : __D1 ) : __A0
{
	JSR      0xFFFFFFE6  // alloc
	MOVE.W   D0,MemErr
	
	RTS
}

asm
short DisposePtr_patch( char* p : __A0 ) : __D0
{
	JSR      0xFFFFFFE4  // dealloc
	MOVE.W   D0,MemErr
	
	RTS
}
