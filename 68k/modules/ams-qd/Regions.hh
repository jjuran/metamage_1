/*
	Regions.hh
	----------
*/

#ifndef REGIONS_HH
#define REGIONS_HH

struct MacRegion;
struct Pattern;

typedef MacRegion** RgnHandle;


pascal void StdRgn_patch( signed char verb, RgnHandle rgn );

pascal void FrameRgn_patch( RgnHandle rgn );
pascal void PaintRgn_patch( RgnHandle rgn );
pascal void EraseRgn_patch( RgnHandle rgn );
pascal void InverRgn_patch( RgnHandle rgn );

pascal void FillRgn_patch( RgnHandle rgn, const Pattern* pattern );

#endif
