/*
	Pointers.cc
	-----------
*/

#include "Pointers.hh"

// Standard C
#include <stdlib.h>

// ams-common
#include "callouts.hh"


short MemErr : 0x0220;

enum
{
	noErr      = 0,
	memFullErr = -108,
};

bool native_alloc;


static
char* NewPtr_handler( long size : __D0, short trap_word : __D1 )
{
	native_alloc = true;  // Set native_alloc in case malloc() calls NewPtr()
	
	void* alloc = malloc( size );
	
	native_alloc = false;
	
	MemErr = noErr;
	
	if ( alloc == NULL )
	{
		MemErr = memFullErr;
		return NULL;
	}
	
	if ( trap_word & 0x0200 )
	{
		fast_memset( alloc, '\0', size );
	}
	
	return (char*) alloc;
}

static
short DisposePtr_handler( char* alloc : __A0 )
{
	native_alloc = true;  // Set native_alloc in case free() calls DisposePtr()
	
	free( alloc );
	
	native_alloc = false;
	
	return MemErr = noErr;
}

asm
char* NewPtr_patch( long size : __D0, short trap_word : __D1 ) : __A0
{
	TST.B    native_alloc
	BEQ.S    not_native
	
	JMP      0xFFFFFFE6  // alloc
	
not_native:
	JSR      NewPtr_handler
	MOVE.W   MemErr,D0
	RTS
}

asm
short DisposePtr_patch( char* p : __A0 ) : __D0
{
	TST.B    native_alloc
	BEQ.S    not_native
	
	JMP      0xFFFFFFE4  // dealloc
	
not_native:
	JMP      DisposePtr_handler
}
