/*
	vivify_qd.hh
	------------
*/

#ifndef MACTYPESSCRIBE_VIVIFYQD_HH
#define MACTYPESSCRIBE_VIVIFYQD_HH

// mac-types
#include "mac_types/Point.hh"
#include "mac_types/Rect.hh"


namespace mac    {
namespace types  {
namespace scribe {
	
	class invalid_decode_input_error {};
	
	
	struct vivify_Point
	{
		static mac::types::Point apply( const char* begin, const char* end );
	};
	
	struct vivify_Rect
	{
		static mac::types::Rect apply( const char* begin, const char* end );
	};
	
}  // namespace scribe
}  // namespace types
}  // namespace mac

#endif
