/*
	Rect.hh
	-------
*/

#ifndef MACTYPES_RECT_HH
#define MACTYPES_RECT_HH

// Mac OS
//#include <MacTypes.h>
struct Rect;


namespace mac   {
namespace types {
	
	struct Rect
	{
		short  top;
		short  left;
		short  bottom;
		short  right;
		
		operator ::Rect const&() const  { return *(::Rect const*) this; }
		operator ::Rect      &()        { return *(::Rect      *) this; }
	};
	
}
}

#endif
