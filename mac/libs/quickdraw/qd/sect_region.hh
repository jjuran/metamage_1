/*
	sect_region.hh
	-------------
*/

#ifndef QUICKDRAW_SECTREGION_HH
#define QUICKDRAW_SECTREGION_HH


namespace quickdraw
{
	
	class segments_box;
	
	void sect_rect_region( const short*   rect,
	                       const short*   bbox,
	                       const short*   extent,
	                       segments_box&  segments,
	                       short*         r );
	
	void sect_rect_region( const short*  rect,
	                       const short*  bbox,
	                       const short*  extent,
	                       unsigned      max_bytes,
	                       short*        r );
	
	void sect_regions( const short*   bbox,
	                   const short*   a_extent,
	                   segments_box&  a_segments,
	                   const short*   b_extent,
	                   segments_box&  b_segments,
	                   segments_box&  c_segments,
	                   segments_box&  r_segments,
	                   short*         r );
	
	void sect_regions( const short*  bbox,
	                   const short*  a_extent,
	                   unsigned      a_max_bytes,
	                   const short*  b_extent,
	                   unsigned      b_max_bytes,
	                   short*        r );
	
}

#endif
