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
		bitmap( int x )
		{
			its_magnifier = x;
		}
		
		void set_pixel( unsigned x, unsigned y, const Pattern& color );
};

}  // namespace nyancat

#endif
