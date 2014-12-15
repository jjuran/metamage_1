/*
	Region-ops.hh
	-------------
*/

#ifndef REGIONOPS_HH
#define REGIONOPS_HH

struct BitMap;
struct MacRegion;

pascal short BitMapToRegion_patch( MacRegion** rgn, const BitMap* bitmap );

pascal void OffsetRgn_patch( MacRegion** rgn, short dh, short dv );

pascal void SectRgn_patch( MacRegion** a, MacRegion** b, MacRegion** dst );
pascal void XOrRgn_patch ( MacRegion** a, MacRegion** b, MacRegion** dst );

#endif
