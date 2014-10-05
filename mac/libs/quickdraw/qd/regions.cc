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
	
	static short scan_for_last_h_coord( const short*& p )
	{
		while ( *p++ != 0x7FFF )  continue;
		
		return p[ -2 ];
	}
	
	void set_region_bbox( short* bbox, const short* extent )
	{
		short v = 0;
		
		short max_h = 0;
		
		// pointing at first v coordinate
		
		*bbox++ = *extent++;  // top
		
		short left = *extent++;
		
		short right = scan_for_last_h_coord( extent );
		
		short bottom;
		
		// pointing at second v coordinate
		
		while ( *extent != 0x7FFF )
		{
			bottom = *extent++;
			
			// pointing at first h coordinate in the line
			
			if ( *extent < left )
			{
				left = *extent;
			}
			
			const short last = scan_for_last_h_coord( extent );
			
			if ( last > right )
			{
				right = last;
			}
			
			// pointing at next v coordinate, or v terminator
		}
		
		*bbox++ = left;
		*bbox++ = bottom;
		*bbox++ = right;
	}
	
}
