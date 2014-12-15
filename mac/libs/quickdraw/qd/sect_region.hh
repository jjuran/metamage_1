/*
	sect_region.hh
	-------------
*/

#ifndef QUICKDRAW_SECTREGION_HH
#define QUICKDRAW_SECTREGION_HH


namespace quickdraw
{
	
	void sect_rect_region( const short*  rect,
	                       const short*  bbox,
	                       const short*  extent,
	                       short*        r );
	
	void sect_regions( const short*  bbox,
	                   const short*  a_extent,
	                   const short*  b_extent,
	                   short*        r );
	
}

#endif
