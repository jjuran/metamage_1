/*
	Bitmap.hh
	---------
*/

#ifndef BITMAP_HH
#define BITMAP_HH

//#include <Quickdraw.h>
struct Pattern;


namespace nyancat {

class bitmap
{
	public:
		typedef unsigned Coord;
		typedef unsigned Delta;
		
		typedef const Pattern& Color;
		
		static
		void set_pixel( Coord x, Coord y, Color color );
		
		static
		void fill_rect( Coord x, Coord y, Delta dx, Delta dy, Color color );
};

}  // namespace nyancat

#endif
