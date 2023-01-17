/*
	refnum_file.cc
	--------------
*/

#include "mac_file/refnum_file.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif


#pragma exceptions off


namespace mac  {
namespace file {
	
#if ! __LP64__
	
	types::FSSpec refnum_file( short refNum )
	{
		types::FSSpec result;
		
		FCBPBRec pb;
		
		pb.ioVRefNum = 0;
		pb.ioFCBIndx = 0;
		pb.ioRefNum  = refNum;
		pb.ioNamePtr = result.name;
		
		if ( OSErr err = ::PBGetFCBInfoSync( &pb ) )
		{
			result.vRefNum = 0;
			result.parID   = OSStatus( err );
		}
		else
		{
			result.vRefNum = pb.ioFCBVRefNum;
			result.parID   = pb.ioFCBParID;
		}
		
		return result;
	}
	
#else
	
	void dummy()
	{
	}
	
#endif  // #if ! __LP64__
	
}
}
