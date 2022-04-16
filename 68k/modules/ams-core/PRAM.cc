/*
	PRAM.cc
	-------
*/

#include "PRAM.hh"

// ams-common
#include "callouts.hh"


pascal void ReadXPRam_patch( void* buffer : __A0, long length_offset : __D0 )
{
	short length = length_offset >> 16;
	
	fast_memset( buffer, '\0', length );
}
