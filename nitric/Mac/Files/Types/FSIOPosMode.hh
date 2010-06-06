/*
	Mac/Files/Types/FSIOPosMode.hh
	------------------------------
*/

#ifndef MAC_FILES_TYPES_FSIOPOSMODE_HH
#define MAC_FILES_TYPES_FSIOPOSMODE_HH

#ifndef __FILES__
#include <Files.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/flag_ops.hh"


namespace Mac
{
	
	enum FSIOPosMode
	{
		fsAtMark    = ::fsAtMark,
		fsFromStart = ::fsFromStart,
		fsFromLEOF  = ::fsFromLEOF,
		fsFromMark  = ::fsFromMark,
		
		kFSIOPosMode_Max = nucleus::enumeration_traits< UInt16 >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( FSIOPosMode )
	
	typedef FSIOPosMode FSIOPositioningMode;
	
}

#endif

