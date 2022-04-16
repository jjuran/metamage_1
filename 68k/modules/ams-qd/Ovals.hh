/*
	Ovals.hh
	--------
*/

#ifndef OVALS_HH
#define OVALS_HH

struct Rect;
struct Pattern;

pascal void StdOval_patch( signed char verb, const Rect* r );

pascal void EraseOval_patch( const Rect* rect );
pascal void PaintOval_patch( const Rect* rect );
pascal void InvertOval_patch( const Rect* rect );
pascal void FrameOval_patch( const Rect* rect );

pascal void FillOval_patch( const Rect* rect, const Pattern* pattern );

#endif
