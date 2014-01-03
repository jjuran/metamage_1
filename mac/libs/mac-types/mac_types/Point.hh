/*
	Point.hh
	--------
*/

#ifndef MACTYPES_POINT_HH
#define MACTYPES_POINT_HH

// Mac OS
//#include <MacTypes.h>
struct Point;


namespace mac   {
namespace types {
	
	struct Point
	{
		short  v;
		short  h;
		
		operator ::Point const&() const  { return *(::Point const*) this; }
		operator ::Point      &()        { return *(::Point      *) this; }
	};
	
}
}

#endif
