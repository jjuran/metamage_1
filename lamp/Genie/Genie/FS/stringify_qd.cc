/*
	stringify_qd.cc
	---------------
*/

#include "Genie/FS/stringify_qd.hh"

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// iota
#include "iota/decimal.hh"

// plus
#include "plus/var_string.hh"


namespace Genie
{
	
	static void append_Point( plus::var_string& out, const Point& pt )
	{
		out += iota::inscribe_decimal( pt.h );
		
		out += ',';
		
		out += iota::inscribe_decimal( pt.v );
	}
	
	plus::string stringify_Point::apply( const Point& pt, bool binary )
	{
		if ( binary )
		{
			return plus::string( (const char*) &pt, sizeof pt );
		}
		
		plus::var_string result;
		
		append_Point( result, pt );
		
		return result;
	}
	
	plus::string stringify_Rect::apply( const Rect& rect, bool binary )
	{
		if ( binary )
		{
			return plus::string( (const char*) &rect, sizeof rect );
		}
		
		const Point* points = (const Point*) &rect;  // reinterpret_cast
		
		plus::var_string result;
		
		append_Point( result, points[0] );  // top left
		
		result += ':';
		
		append_Point( result, points[1] );  // bottom right
		
		return result;
	}
	
}

