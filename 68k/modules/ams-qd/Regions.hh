/*
	Regions.hh
	----------
*/

#ifndef REGIONS_HH
#define REGIONS_HH

struct MacRegion;
struct Pattern;
struct Rect;

typedef MacRegion** RgnHandle;


pascal void StdRgn_patch( signed char verb, RgnHandle rgn );

pascal void FrameRgn_patch( RgnHandle rgn );
pascal void PaintRgn_patch( RgnHandle rgn );
pascal void EraseRgn_patch( RgnHandle rgn );
pascal void InverRgn_patch( RgnHandle rgn );

pascal void FillRgn_patch( RgnHandle rgn, const Pattern* pattern );

pascal RgnHandle NewRgn_patch();

pascal void OpenRgn_patch();

pascal void CloseRgn_patch( RgnHandle rgn );

pascal void DisposeRgn_patch( RgnHandle rgn );

pascal void CopyRgn_patch( RgnHandle src, RgnHandle dst );

pascal void SetEmptyRgn_patch( RgnHandle rgn );

pascal void SetRectRgn_patch( RgnHandle  rgn,
                              short      left,
                              short      top,
                              short      right,
                              short      bottom );

pascal void RectRgn_patch( RgnHandle rgn, const Rect* r );

#endif
