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
	private:
		int its_magnifier;
	
	public:
		typedef unsigned Coord;
		typedef unsigned Delta;
		
		typedef const Pattern& Color;
		
		bitmap( int x )
		{
			its_magnifier = x;
		}
		
		void set_pixel( unsigned x, unsigned y, const Pattern& color );
		
		void fill_rect( Coord x, Coord y, Delta dx, Delta dy, Color color );
};

}  // namespace nyancat

#endif
