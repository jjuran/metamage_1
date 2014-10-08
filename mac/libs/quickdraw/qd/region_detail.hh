/*
	region_detail.hh
	----------------
*/

#ifndef QUICKDRAW_REGIONDETAIL_HH
#define QUICKDRAW_REGIONDETAIL_HH


namespace quickdraw
{
	
	const short Region_end = 0x7FFF;
	
	inline bool precedes_in_region( short a, short b )
	{
		return a < b;
	}
	
}

#endif
