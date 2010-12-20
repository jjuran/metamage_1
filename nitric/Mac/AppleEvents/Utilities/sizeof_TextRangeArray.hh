/*
	Mac/AppleEvents/Utilities/sizeof_TextRangeArray.hh
	--------------------------------------------------
*/

#ifndef MAC_APPLEEVENTS_UTILITIES_SIZEOFTEXTRANGEARRAY_HH
#define MAC_APPLEEVENTS_UTILITIES_SIZEOFTEXTRANGEARRAY_HH

// Standard C/C++
#include <cstddef>

// Mac OS
#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif


namespace Mac
{
	
	inline std::size_t sizeof_TextRangeArray( const TextRangeArray& array )
	{
		return sizeof array - sizeof array.fRange + array.fNumOfRanges * sizeof (TextRange);
	}
	
}

#endif

