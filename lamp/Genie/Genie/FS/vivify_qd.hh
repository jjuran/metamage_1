/*
	vivify_qd.hh
	------------
*/

#ifndef GENIE_FS_VIVIFYQD_HH
#define GENIE_FS_VIVIFYQD_HH

// mac-types
#include "mac_types/Point.hh"
#include "mac_types/Rect.hh"


namespace Genie
{
	
	class invalid_decode_input_error {};
	
	
	struct vivify_Point
	{
		static mac::types::Point apply( const char* begin, const char* end );
	};
	
	struct vivify_Rect
	{
		static mac::types::Rect apply( const char* begin, const char* end );
	};
	
}

#endif

