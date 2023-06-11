/*
	rw.cc
	-----
*/

#include "mac_file/rw.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __APPLE__
#ifndef __DEVICES__
#include <Devices.h>
#endif
#endif


#pragma exceptions off


namespace mac  {
namespace file {

#if ! __LP64__

long read( short refnum, void* buffer, long n, short mode, long offset )
{
	ParamBlockRec pb;
	
	IOParam& io = pb.ioParam;
	
	io.ioRefNum    = refnum;
	io.ioBuffer    = (Ptr) buffer;
	io.ioReqCount  = n;
	io.ioPosMode   = mode;
	io.ioPosOffset = offset;
	
	OSErr err = PBReadSync( &pb );
	
	if ( err == noErr  ||  err == eofErr )
	{
		return io.ioActCount;
	}
	
	return err;
}

short write( short refnum, const void* buffer, long n, short mode, long offset )
{
	ParamBlockRec pb;
	
	IOParam& io = pb.ioParam;
	
	io.ioRefNum    = refnum;
	io.ioBuffer    = (Ptr) buffer;
	io.ioReqCount  = n;
	io.ioPosMode   = mode;
	io.ioPosOffset = offset;
	
	OSErr err = PBWriteSync( &pb );
	
	return err;
}

#else  // #if ! __LP64__

int dummy;

#endif  // #else  // #if ! __LP64__

}
}
