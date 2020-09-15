/*
	regions.hh
	----------
*/

#ifndef QUICKDRAW_REGIONS_HH
#define QUICKDRAW_REGIONS_HH


namespace quickdraw
{
	
	struct region_geometry_t
	{
		short n_v_coords;
		short n_h_coords;  // max number of horizontal coordinates
	};
	
	region_geometry_t region_geometry( const short* extent );
	
	const short* set_region_bbox( short* bbox, const short* extent );
	
	void offset_region( short* extent, short dh, short dv );
	
}

#endif
