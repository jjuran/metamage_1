/*
	decode.cc
	---------
*/

#include "decode.hh"

// mac-sys-utils
#include "mac_sys/beep.hh"

// Arcana
#include "MacBinary.hh"


using mac::types::VRefNum_DirID;


static
long read_FSRead( FSIORefNum refnum, void* buffer, SInt32 n_bytes )
{
	OSErr err = FSRead( refnum, &n_bytes, buffer );
	
	if ( err  &&  err != eofErr )
	{
		return err;
	}
	
	return n_bytes;
}

OSErr Decode( FSIORefNum input, const VRefNum_DirID& destDir )
{
	MacBinary::Decoder decoder( destDir );
	
	const std::size_t blockSize = 4096;
	
	char data[ blockSize ];
	
	std::size_t totalBytes = 0;
	
	try
	{
		long bytes;
		
		while ( (bytes = read_FSRead( input, data, blockSize )) > 0 )
		{
			decoder.Write( data, bytes );
			
			totalBytes += bytes;
		}
		
		if ( bytes < 0 )
		{
			return bytes;
		}
	}
	catch ( const MacBinary::InvalidMacBinaryHeader& )
	{
		//std::fprintf( stderr, "Invalid MacBinary header somewhere past offset %x\n", totalBytes );
		mac::sys::beep();
	}
	catch ( ... )
	{
		// FIXME
	}
	
	return noErr;
}
