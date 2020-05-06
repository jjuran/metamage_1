/*
	utility_region.hh
	-----------------
*/

#ifndef UTILITYREGION_HH
#define UTILITYREGION_HH

struct MacRegion;
struct Rect;

typedef MacRegion** RgnHandle;

RgnHandle rectangular_utility_region( const Rect& rect );

#endif
