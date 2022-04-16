/*
	ovoid_region.hh
	---------------
*/

#ifndef OVOIDREGION_HH
#define OVOIDREGION_HH

struct MacRegion;

typedef MacRegion** RgnHandle;

RgnHandle ovoid_region( short hdiam, short vdiam, short width, short height );

inline
RgnHandle ovoid_region( short hdiam, short vdiam )
{
	return ovoid_region( hdiam, vdiam, hdiam, vdiam );
}

#endif
