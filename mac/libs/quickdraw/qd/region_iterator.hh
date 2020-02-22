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
			region_iterator( const segments_box& seg_box, const short* extent );
			
			const region_band* next();
	};
	
	/*
		malloc_region_iterator is morally equivalent to a derived class of
		region_iterator, but we can't actually make it one, because we need
		to construct the malloc_segments_box before we construct the base.
	*/
	
	class malloc_region_iterator
	{
		private:
			malloc_segments_box its_segments;
			region_iterator     its_base;
		
		public:
			malloc_region_iterator( short size, const short* extent );
			
			const region_band* next()  { return its_base.next(); }
	};
	
}

#endif
