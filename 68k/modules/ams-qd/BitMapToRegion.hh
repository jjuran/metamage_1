/*
	BitMapToRegion.hh
	-----------------
*/

#ifndef BITMAPTOREGION_HH
#define BITMAPTOREGION_HH


struct BitMap;
struct MacRegion;

typedef short OSErr;

typedef MacRegion** RgnHandle;


pascal OSErr BitMapToRegion_patch( RgnHandle rgn, const BitMap* bitmap );

#endif
