/*
	Regions.hh
	----------
*/

#ifndef REGIONS_HH
#define REGIONS_HH

struct MacRegion;
struct Pattern;
struct Rect;

pascal void StdRgn_patch( signed char verb, MacRegion** rgn );

pascal void FrameRgn_patch( MacRegion** rgn );
pascal void PaintRgn_patch( MacRegion** rgn );
pascal void EraseRgn_patch( MacRegion** rgn );
pascal void InverRgn_patch( MacRegion** rgn );

pascal void FillRgn_patch( MacRegion** rgn, const Pattern* pattern );

pascal MacRegion** NewRgn_patch();

pascal void OpenRgn_patch();

pascal void CloseRgn_patch( MacRegion** rgn );

pascal void DisposeRgn_patch( MacRegion** rgn );

pascal void CopyRgn_patch( MacRegion** src, MacRegion** dst );

pascal void SetEmptyRgn_patch( MacRegion** rgn );

pascal void SetRectRgn_patch( MacRegion**  rgn,
                              short        left,
                              short        top,
                              short        right,
                              short        bottom );

pascal void RectRgn_patch( MacRegion** rgn, const Rect* r );

pascal unsigned char RectInRgn_patch( const Rect* r, MacRegion** rgn );

pascal unsigned char EqualRgn_patch( MacRegion** a, MacRegion** b );

pascal unsigned char EmptyRgn_patch( MacRegion** rgn );

#endif
