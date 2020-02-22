/*
	region_iterator.hh
	------------------
*/

#ifndef QUICKDRAW_REGIONITERATOR_HH
#define QUICKDRAW_REGIONITERATOR_HH

// quickdraw
#include "qd/segments.hh"


namespace quickdraw
{
	
	struct region_band
	{
		short v0;
		short v1;
		
		const short* h_begin;
		const short* h_end;
	};
	
	class region_iterator
	{
		private:
			segments_box its_segments;
			
			const short* its_next_coordinate;
			
			region_band its_next_band;
			
			void advance();
		
		public:
			region_iterator( short size, const short* extent );
			
			const region_band* next();
	};
	
	typedef region_iterator malloc_region_iterator;
	
}

#endif
