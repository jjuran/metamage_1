/*
	Mac/AppleEvents/Utilities/sizeof_OffsetArray.hh
	-----------------------------------------------
*/

#ifndef MAC_APPLEEVENTS_UTILITIES_SIZEOFOFFSETARRAY_HH
#define MAC_APPLEEVENTS_UTILITIES_SIZEOFOFFSETARRAY_HH

// Standard C/C++
#include <cstddef>

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif


namespace Mac
{
	
	inline std::size_t sizeof_OffsetArray( const OffsetArray& array )
	{
		return sizeof array - sizeof array.fOffset + array.fNumOfOffsets * sizeof (SInt32);
	}
	
}

#endif

