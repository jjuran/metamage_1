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
	                       unsigned      max_bytes,
	                       short*        r );
	
	void sect_regions( const short*  bbox,
	                   const short*  a_extent,
	                   unsigned      a_max_bytes,
	                   const short*  b_extent,
	                   unsigned      b_max_bytes,
	                   short*        r );
	
}

#endif
