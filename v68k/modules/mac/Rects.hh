/*
	Rects.hh
	--------
*/

#ifndef RECTS_HH
#define RECTS_HH

struct Rect;
struct Pattern;

pascal void EraseRect_patch( const Rect* rect );
pascal void PaintRect_patch( const Rect* rect );
pascal void InverRect_patch( const Rect* rect );
pascal void FrameRect_patch( const Rect* rect );

pascal void FillRect_patch( const Rect* rect, const Pattern* pattern );

pascal void InsetRect_patch( Rect* rect, short dh, short dv );

#endif

