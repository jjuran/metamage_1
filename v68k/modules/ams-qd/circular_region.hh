/*
	circular_region.hh
	------------------
*/

#ifndef CIRCULARREGION_HH
#define CIRCULARREGION_HH

struct MacRegion;

MacRegion** circular_region( short diameter, short width, short height );

inline
MacRegion** circular_region( short diameter )
{
	return circular_region( diameter, diameter, diameter );
}

#endif
