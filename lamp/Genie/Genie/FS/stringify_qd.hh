/*
	stringify_qd.hh
	---------------
*/

#ifndef GENIE_FS_STRINGIFYQD_HH
#define GENIE_FS_STRINGIFYQD_HH

// plus
#include "plus/string.hh"

// #include <MacTypes.h>
struct Point;
struct Rect;


namespace Genie
{
	
	struct stringify_Point
	{
		static plus::string apply( const Point& pt, bool binary );
	};
	
	struct stringify_Rect
	{
		static plus::string apply( const Rect& rect, bool binary );
	};
	
}

#endif

