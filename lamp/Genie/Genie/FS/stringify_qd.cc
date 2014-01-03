/*
	stringify_qd.cc
	---------------
*/

#include "Genie/FS/stringify_qd.hh"

// mac-types
#include "mac_types/Point.hh"
#include "mac_types/Rect.hh"

// gear
#include "gear/inscribe_decimal.hh"

// plus
#include "plus/var_string.hh"


namespace Genie
{
	
	using mac::types::Point;
	
	
	void stringify_Point::apply( plus::var_string& out, const ::Point& pt_ )
	{
		const Point& pt = (const Point&) pt_;
		
		out += gear::inscribe_decimal( pt.h );
		
		out += ',';
		
		out += gear::inscribe_decimal( pt.v );
	}
	
	void stringify_Rect::apply( plus::var_string& out, const ::Rect& rect )
	{
		const Point* points = (const Point*) &rect;  // reinterpret_cast
		
		stringify_Point::apply( out, points[0] );  // top left
		
		out += ':';
		
		stringify_Point::apply( out, points[1] );  // bottom right
	}
	
}

