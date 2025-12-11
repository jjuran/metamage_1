/*
	Region-test.hh
	--------------
*/

#ifndef REGIONMATH_HH
#define REGIONMATH_HH


struct MacRegion;
struct Point;
struct Rect;

typedef unsigned char Boolean;

typedef MacRegion** RgnHandle;


pascal Boolean PtInRgn_patch( Point pt, RgnHandle rgn );

pascal Boolean RectInRgn_patch( const Rect* r, RgnHandle rgn );

pascal Boolean EqualRgn_patch( RgnHandle a, RgnHandle b );

pascal Boolean EmptyRgn_patch( RgnHandle rgn );

#endif
