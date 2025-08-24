/*
	Shutdown.cc
	-----------
*/

#include "Shutdown.hh"

// POSIX
#include <unistd.h>

// log-of-war
#include "logofwar/report.hh"

// ams-seg
#include "options.hh"


#pragma exceptions off


#define COUNT(array)  (sizeof (array) / sizeof *(array))


static
void unimplemented_call( short selector : __D0 )
{
	FATAL = "unimplemented ShutDown call ", selector;
	
	asm { ILLEGAL }
}

static
void ShutDown_call( short selector : __D0 )
{
	if ( linger_on_exit )
	{
		pause();
	}
	
	_exit( 0 );
}

static void* handlers[] =
{
	&unimplemented_call,
	&ShutDown_call,  // ShutDwnPower
	&ShutDown_call,  // ShutDwnStart
};

enum
{
	n_handlers = COUNT( handlers ),
};

asm void ShutDown_patch( short selector )
{
	MOVEA.L  (SP)+,A0        // pop return address
	MOVE.W   (SP)+,D0        // pop selector
	MOVE.L   A0,-(SP)        // push return address
	
	LEA      handlers,A0
	
	CMPI.W   #n_handlers,D0  // selector <=> n_handlers
	BPL.S    just_go         // if selector >= n_handlers, use 0 offset
	
	MOVE.W   D0,D1           // copy selector as array index
	
	LSL.W    #2,D1           // scale index to pointer width
	ADDA.W   D1,A0           // apply byte offset
	
just_go:
	
	MOVEA.L  (A0),A0         // load the handler address
	
	JMP      (A0)
}
