/*
	bitmap.hh
	---------
*/

#ifndef BITMAP_HH
#define BITMAP_HH

// Standard C
#include <stddef.h>
#include <stdint.h>


class backing_store
{
	private:
		uint8_t*  its_buffer;
		uint32_t  its_length;
		uint32_t  its_stride;
	
	public:
		backing_store( uint8_t* buffer, size_t length, size_t stride )
		{
			its_buffer = buffer;
			its_length = length;
			its_stride = stride;
		}
		
		uint8_t* begin()
		{
			return its_buffer;
		}
		
		uint8_t* end()
		{
			return its_buffer + its_length;
		}
		
		uint8_t* row( int y )
		{
			return its_buffer + y * its_stride;
		}
};

namespace nyancat {

class bitmap
{
	private:
		backing_store  its_store;
		size_t         its_width;
		size_t         its_height;
		
		uint32_t* row( int y )
		{
			return (uint32_t*) its_store.row( y );
		}
	
	public:
		typedef unsigned Coord;
		typedef unsigned Delta;
		
		bitmap( uint32_t* buffer, size_t length, size_t stride )
		:
			its_store( (uint8_t*) buffer, length, stride )
		{
			its_height = length / stride;
			its_width  = stride / 4;
		}
		
		uint32_t* begin()
		{
			return (uint32_t*) its_store.begin();
		}
		
		uint32_t* end()
		{
			return (uint32_t*) its_store.end();
		}
		
		void set_pixel( unsigned x, unsigned y, uint32_t pixel )
		{
			if ( y < its_height  &&  x < its_width )
			{
				row( y )[ x ] = pixel;
			}
		}
		
		void fill_rect( Coord x, Coord y, Delta dx, Delta dy, uint32_t pixel )
		{
			if ( y + dy <= its_height  &&  x + dx <= its_width )
			{
				for ( Coord i = y;  i < y + dy;  ++i )
				{
					for ( Coord j = x;  j < x + dx;  ++j )
					{
						row( i )[ j ] = pixel;
					}
				}
			}
		}
};

}  // namespace nyancat

#endif
