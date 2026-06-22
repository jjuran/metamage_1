/*
	PRAM.cc
	-------
*/

#include "PRAM.hh"

// ams-common
#include "callouts.hh"


enum
{
	location_begin = 0xE4,
	location_end   = 0xE4 + 12,  // $F0
};

static unsigned char xpram[ 256 ];

pascal void ReadXPRam_patch( void* buffer : __A0, long length_offset : __D0 )
{
	short offset = length_offset;
	short length = length_offset >> 16;
	
	fast_memset( buffer, '\0', length );
	
	if ( offset >= 256 )
	{
		return;
	}
	
	if ( length > 256 - offset )
	{
		length = 256 - offset;
	}
	
	if ( offset < location_end  &&  offset + length > location_begin )
	{
		ReadLocation( (MachineLocation*) &xpram[ location_begin ] );
		
		fast_memcpy( buffer, xpram + offset, length );
	}
}
