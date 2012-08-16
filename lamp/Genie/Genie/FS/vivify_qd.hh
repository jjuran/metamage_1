/*
	vivify_qd.hh
	------------
*/

#ifndef GENIE_FS_VIVIFYQD_HH
#define GENIE_FS_VIVIFYQD_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif


namespace Genie
{
	
	class invalid_decode_input_error {};
	
	
	struct vivify_Point
	{
		static Point apply( const char* begin, const char* end );
	};
	
	struct vivify_Rect
	{
		static Rect apply( const char* begin, const char* end );
	};
	
}

#endif

