/*
	dcmp.cc
	-------
*/

#include "dcmp.hh"

// Mac OS
#ifndef __MACERRORS__
#include <MacErrors.h>
#endif
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "callouts.hh"


enum
{
	kCompressedResourceSignature = 0xA89F6572,
};


short ResErr : 0x0A60;


struct compressed_rsrc_header
{
	UInt32 signature;
	UInt16 length;
	UInt8  format;
	UInt8  unknown;
};

Handle PtrToHand_dcmp( const void* p : __A0, UInt32 size : __D0 )
{
	compressed_rsrc_header header;
	
	ResErr = CantDecompress;
	
	if ( size < sizeof header )
	{
		ERROR = "Compressed resource lacks a metadata header";
		return NULL;
	}
	
	fast_memcpy( &header, p, sizeof header );  // possibly misaligned
	
	if ( header.signature != kCompressedResourceSignature )
	{
		ERROR = "Compressed resource has unrecognized signature";
		return NULL;
	}
	
	if ( header.length < sizeof header )
	{
		ERROR = "Compressed resource has corrupted header";
		return NULL;
	}
	
	if ( size < header.length )
	{
		ERROR = "Compressed resource has truncated header";
		return NULL;
	}
	
	if ( header.format - 8 > 1 )
	{
		ERROR = "Compressed resource has unknown format ", header.format;
		return NULL;
	}
	
	ERROR = "Resource compression is unimplemented";
	
	return NULL;
}
