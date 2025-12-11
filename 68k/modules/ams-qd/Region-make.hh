/*
	Region-make.hh
	--------------
*/

#ifndef REGIONMAKE_HH
#define REGIONMAKE_HH

struct MacRegion;
struct Rect;

typedef MacRegion** RgnHandle;


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
