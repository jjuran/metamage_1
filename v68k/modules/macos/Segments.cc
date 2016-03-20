/*
	Segments.cc
	-----------
*/

#include "Segments.hh"

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>
#include <string.h>

// macos
#include "options.hh"


Str31 CurApName   : 0x0910;
short CurJTOffset : 0x0934;


struct jump_table_header
{
	uint32_t above_a5_size;
	uint32_t below_a5_size;
	uint32_t jmptable_size;
	uint32_t jmptable_offset;
};

struct LaunchParamBlockRec
{
	uint32_t reserved1;
	// ...
};

pascal short Launch_patch( LaunchParamBlockRec* pb : __A0 ) : __D0
{
	const unsigned char* appName = (StringPtr) pb->reserved1;
	
	if ( appName == NULL )
	{
		DebugStr( "\p" "LaunchApplication() not supported" );
		
		ExitToShell_patch();
	}
	
	const uint8_t len = appName[ 0 ];
	
	if ( len > 31 )
	{
		DebugStr( "\p" "Application name longer than 31 bytes" );
		
		ExitToShell_patch();
	}
	
	memcpy( CurApName, appName, 1 + len );
	
	Handle code0 = GetResource( 'CODE', 0 );
	
	const jump_table_header header = *(jump_table_header*) *code0;
	
	CurJTOffset = header.jmptable_offset;
	
	const uint32_t total_a5_size = header.above_a5_size
	                             + header.below_a5_size;
	
	void* alloc = malloc( total_a5_size );
	
	void* new_a5 = (char*) alloc + header.below_a5_size;
	
	void* jump_table = (char*) new_a5 + CurJTOffset;
	
	memcpy( jump_table, (jump_table_header*) *code0 + 1, header.jmptable_size );
	
	ReleaseResource( code0 );
	
	/*
		This is a temporary hack until we implement _LoadSeg, and it only
		works on (a) single-segment applications that (b) have their startup
		code linked as the first jump table entry.  This works out fine for
		applications built with CodeWarrior's single-segment option, and for
		TestApp built with SC (as long as main() is defined first).
	*/
	
	Handle code1 = GetResource( 'CODE', 1 );
	
	void* start = *code1 + sizeof (uint32_t);
	
	asm
	{
		MOVEA.L  new_a5,A5
		MOVEA.L  start,A0
		JMP      (A0)
	}
	
	return 0;  // not reached
}

pascal void ExitToShell_patch()
{
	if ( linger_on_exit )
	{
		pause();
	}
	
	_exit( 0 );
}
