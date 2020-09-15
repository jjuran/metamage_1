/*
	Region-ops.hh
	-------------
*/

#ifndef REGIONOPS_HH
#define REGIONOPS_HH

struct BitMap;
struct MacRegion;
struct Point;

pascal short BitMapToRegion_patch( MacRegion** rgn, const BitMap* bitmap );

pascal void OffsetRgn_patch( MacRegion** rgn, short dh, short dv );
pascal void InsetRgn_patch ( MacRegion** rgn, short dh, short dv );

pascal void SectRgn_patch ( MacRegion** a, MacRegion** b, MacRegion** dst );
pascal void UnionRgn_patch( MacRegion** a, MacRegion** b, MacRegion** dst );
pascal void DiffRgn_patch ( MacRegion** a, MacRegion** b, MacRegion** dst );
pascal void XOrRgn_patch  ( MacRegion** a, MacRegion** b, MacRegion** dst );

pascal unsigned char PtInRgn_patch( Point pt, MacRegion** rgn );

#endif
