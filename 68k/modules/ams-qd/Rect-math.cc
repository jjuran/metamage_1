/*
	Rect-math.cc
	------------
*/

#include "Rect-math.hh"

// Mac OS
#ifndef __FIXMATH__
#include <FixMath.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// ams-common
#include "math.hh"


#pragma exceptions off


pascal void SetRect_patch( Rect*  rect,
                           short  left,
                           short  top,
                           short  right,
                           short  bottom )
{
	rect->top    = top;
	rect->left   = left;
	rect->bottom = bottom;
	rect->right  = right;
}

pascal void OffsetRect_patch( Rect* rect, short dh, short dv )
{
	if ( (dv < 0 ? -rect->top - dv : rect->bottom + dv) > 32767 )
	{
		return;
	}
	
	if ( (dh < 0 ? -rect->left - dh : rect->right + dh) > 32767 )
	{
		return;
	}
	
	rect->top    += dv;
	rect->left   += dh;
	rect->bottom += dv;
	rect->right  += dh;
}

pascal void InsetRect_patch( Rect* rect, short dh, short dv )
{
	rect->top    += dv;
	rect->left   += dh;
	rect->bottom -= dv;
	rect->right  -= dh;
}

pascal Boolean SectRect_patch( const Rect* a, const Rect* b, Rect* c )
{
	c->top    = a->top    > b->top    ? a->top    : b->top;
	c->left   = a->left   > b->left   ? a->left   : b->left;
	c->bottom = b->bottom < a->bottom ? b->bottom : a->bottom;
	c->right  = b->right  < a->right  ? b->right  : a->right;
	
	if ( EmptyRect( c ) )
	{
		c->top    = 0;
		c->left   = 0;
		c->bottom = 0;
		c->right  = 0;
		
		return false;
	}
	
	return true;
}

pascal void UnionRect_patch( const Rect* a, const Rect* b, Rect* c )
{
	c->top    = b->top    < a->top    ? b->top    : a->top;
	c->left   = b->left   < a->left   ? b->left   : a->left;
	c->bottom = a->bottom > b->bottom ? a->bottom : b->bottom;
	c->right  = a->right  > b->right  ? a->right  : b->right;
}

pascal Boolean PtInRect_patch( Point pt, const Rect* rect )
{
	return rect->top  <= pt.v  &&  pt.v < rect->bottom  &&
	       rect->left <= pt.h  &&  pt.h < rect->right;
}

pascal void Pt2Rect_patch( Point a, Point b, Rect* c )
{
	if ( b.v < a.v )
	{
		c->top    = b.v;
		c->bottom = a.v;
	}
	else
	{
		c->top    = a.v;
		c->bottom = b.v;
	}
	
	if ( b.h < a.h )
	{
		c->left  = b.h;
		c->right = a.h;
	}
	else
	{
		c->left  = a.h;
		c->right = b.h;
	}
}

pascal void PtToAngle_patch( const Rect* rect, Point pt, short* result )
{
	const UInt16 height = rect->bottom - rect->top;
	const UInt16 width  = rect->right - rect->left;
	
	const short cv = rect->bottom + rect->top >> 1;  // rect center V
	const short ch = rect->right + rect->left >> 1;  // rect center H
	
	const UInt16 rise = pt.v - cv;
	const UInt16 run  = pt.h - ch;
	
	short angle = 0;
	
	if ( run == 0 )
	{
		if ( pt.v > cv )
		{
			angle = 180;
		}
	}
	else
	{
		if ( rise == 0 )
		{
			angle = 90;
		}
		else
		{
			const uint32_t run_x_height = mulu_w( run, height );
			const uint32_t rise_x_width = mulu_w( rise, width );
			
			angle = AngleFromSlope( FixRatio( run_x_height, rise_x_width ) );
		}
		
		if ( pt.h < ch )
		{
			angle += 180;
		}
	}
	
	*result = angle;
}

pascal Boolean EqualRect_patch( const Rect* a, const Rect* b )
{
	return a->top    == b->top     &&
	       a->left   == b->left    &&
	       a->bottom == b->bottom  &&
	       a->right  == b->right;
}

pascal Boolean EmptyRect_patch( const Rect* rect )
{
	return rect->top >= rect->bottom  ||  rect->left >= rect->right;
}
