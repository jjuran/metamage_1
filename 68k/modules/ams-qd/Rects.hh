/*
	Rects.hh
	--------
*/

#ifndef RECTS_HH
#define RECTS_HH

struct Rect;
struct Pattern;
struct Point;

pascal void StdRect_patch( signed char verb, const Rect* r );

pascal void EraseRect_patch( const Rect* rect );
pascal void PaintRect_patch( const Rect* rect );
pascal void InverRect_patch( const Rect* rect );
pascal void FrameRect_patch( const Rect* rect );

pascal void FillRect_patch( const Rect* rect, const Pattern* pattern );

pascal unsigned char EqualRect_patch( const Rect* a, const Rect* b );

pascal void SetRect_patch( Rect*  rect,
                           short  left,
                           short  top,
                           short  right,
                           short  bottom );

pascal void OffsetRect_patch( Rect* rect, short dh, short dv );
pascal void InsetRect_patch ( Rect* rect, short dh, short dv );

pascal unsigned char SectRect_patch( const Rect* a, const Rect* b, Rect* c );

pascal void UnionRect_patch( const Rect* a, const Rect* b, Rect* c );

pascal void Pt2Rect_patch( Point a, Point b, Rect* c );

pascal unsigned char PtInRect_patch( Point pt, const Rect* rect );

pascal void PtToAngle_patch( const Rect* rect, Point pt, short* angle );

pascal unsigned char EmptyRect_patch( const Rect* rect );

#endif
