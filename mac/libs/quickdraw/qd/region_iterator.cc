/*
	region_iterator.cc
	------------------
*/

#include "qd/region_iterator.hh"

// quickdraw
#include "qd/region_detail.hh"


namespace quickdraw
{
	
	region_iterator::region_iterator( const segments_box&  seg_box,
	                                  const short*         extent )
	:
		its_segments( seg_box )
	{
		its_next_coordinate = extent;
		
		its_next_band.v1 = *its_next_coordinate++;
	}
	
	void region_iterator::advance()
	{
		using quickdraw::Region_end;
		
		while ( true )
		{
			short h = *its_next_coordinate++;
			
			if ( h == Region_end )
			{
				break;
			}
			
			its_segments ^= h;
		}
		
		if ( its_segments.size() & 0x1 )
		{
			its_segments ^= 0x7FFF;
		}
		
		its_next_band.v0 = its_next_band.v1;
		its_next_band.v1 = *its_next_coordinate++;
		
		its_next_band.h_begin = its_segments.begin();
		its_next_band.h_end   = its_segments.end  ();
	}
	
	const region_band* region_iterator::next()
	{
		using quickdraw::Region_end;
		using quickdraw::precedes_in_region;
		
		if ( its_next_band.v1 == Region_end )
		{
			return NULL;
		}
		
		advance();
		
		if ( its_segments.empty()  &&  its_next_band.v1 == Region_end )
		{
			return NULL;
		}
		
		return &its_next_band;
	}
	
	malloc_region_iterator::malloc_region_iterator( short         size,
	                                                const short*  extent )
	:
		its_segments( size ),
		its_base( its_segments, extent )
	{
	}
	
}
