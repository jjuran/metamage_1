/*
	Rect-math.hh
	------------
*/

#ifndef RECTMATH_HH
#define RECTMATH_HH


struct Rect;
struct Point;

typedef unsigned char Boolean;


pascal void SetRect_patch( Rect*  rect,
                           short  left,
                           short  top,
                           short  right,
                           short  bottom );

pascal void OffsetRect_patch( Rect* rect, short dh, short dv );
pascal void InsetRect_patch ( Rect* rect, short dh, short dv );

pascal Boolean SectRect_patch( const Rect* a, const Rect* b, Rect* c );

pascal void UnionRect_patch( const Rect* a, const Rect* b, Rect* c );

pascal Boolean PtInRect_patch( Point pt, const Rect* rect );

pascal void Pt2Rect_patch( Point a, Point b, Rect* c );

pascal void PtToAngle_patch( const Rect* rect, Point pt, short* angle );

pascal Boolean EqualRect_patch( const Rect* a, const Rect* b );

pascal Boolean EmptyRect_patch( const Rect* rect );

#endif
