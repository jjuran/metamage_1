/*
	regions.cc
	----------
*/

#include "qd/regions.hh"

// quickdraw
#include "qd/rect.hh"


namespace quickdraw
{
	
	region_geometry_t region_geometry( const short* extent )
	{
		short v = 0;
		
		short max_h = 0;
		
		// pointing at first v coordinate
		
		while ( *extent++ != 0x7FFF )
		{
			++v;
			
			// pointing at first h coordinate in the line
			
			short h = 0;
			
			while ( *extent++ != 0x7FFF )
			{
				++h;
			}
			
			if ( h > max_h )
			{
				max_h = h;
			}
			
			// pointing at next v coordinate, or v terminator
		}
		
		const region_geometry_t result = { v, max_h };
		
		return result;
	}
	
	void set_region_bbox( Rect& bbox, const short* extent )
	{
		short v = 0;
		
		short max_h = 0;
		
		// pointing at first v coordinate
		
		bbox.top  = *extent++;
		bbox.left = *extent++;
		
		while ( *extent != 0x7FFF )
		{
			bbox.right = *extent++;
		}
		
		++extent;
		
		// pointing at second v coordinate
		
		while ( *extent != 0x7FFF )
		{
			bbox.bottom = *extent++;
			
			// pointing at first h coordinate in the line
			
			if ( *extent < bbox.left )
			{
				bbox.left = *extent;
			}
			
			short right;
			
			while ( *extent != 0x7FFF )
			{
				right = *extent++;
			}
			
			if ( right > bbox.right )
			{
				bbox.right = right;
			}
			
			++extent;
			
			// pointing at next v coordinate, or v terminator
		}
	}
	
}
