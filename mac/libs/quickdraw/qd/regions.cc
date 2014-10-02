/*
	regions.cc
	----------
*/

#include "qd/regions.hh"


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
	
}
