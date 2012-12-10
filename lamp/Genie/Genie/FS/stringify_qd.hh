/*
	stringify_qd.hh
	---------------
*/

#ifndef GENIE_FS_STRINGIFYQD_HH
#define GENIE_FS_STRINGIFYQD_HH

// plus
#include "plus/var_string_fwd.hh"

// #include <MacTypes.h>
struct Point;
struct Rect;


namespace Genie
{
	
	struct stringify_Point
	{
		static void apply( plus::var_string& out, const Point& pt );
	};
	
	struct stringify_Rect
	{
		static void apply( plus::var_string& out, const Rect& rect );
	};
	
}

#endif

