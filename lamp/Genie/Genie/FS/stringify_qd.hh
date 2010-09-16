/*
	stringify_qd.hh
	---------------
*/

#ifndef GENIE_FS_STRINGIFYQD_HH
#define GENIE_FS_STRINGIFYQD_HH

// #include <MacTypes.h>
struct Point;
struct Rect;


namespace plus
{
	
	class var_string;
	
}

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

