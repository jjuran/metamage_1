/*
	Regions.hh
	----------
*/

#ifndef REGIONS_HH
#define REGIONS_HH

struct MacRegion;
struct Rect;

pascal MacRegion** NewRgn_patch();

pascal void DisposeRgn_patch( MacRegion** rgn );

pascal void CopyRgn_patch( MacRegion** src, MacRegion** dst );

pascal void SetEmptyRgn_patch( MacRegion** rgn );

pascal void SetRectRgn_patch( MacRegion**  rgn,
                              short        left,
                              short        top,
                              short        right,
                              short        bottom );

pascal void RectRgn_patch( MacRegion** rgn, const Rect* r );

pascal void OffsetRgn_patch( MacRegion** rgn, short dh, short dv );

pascal void XOrRgn_patch( MacRegion** a, MacRegion** b, MacRegion** dst );

pascal unsigned char EmptyRgn_patch( MacRegion** rgn );

#endif
