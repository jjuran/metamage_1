/*
	cursor_location.hh
	------------------
*/

#ifndef MACSYS_CURSORLOCATION_HH
#define MACSYS_CURSORLOCATION_HH


struct Point;

namespace mac {
namespace sys {
	
	Point get_cursor_location();
	
	void set_cursor_location( Point location );
	
}
}

#endif
