/*
	Region-test.cc
	--------------
*/

#include "Region-test.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// quickdraw
#include "qd/region_detail.hh"

// ams-common
#include "callouts.hh"


#pragma exceptions off


using quickdraw::Region_end;

static inline
const short* rgn_extent( const MacRegion* rgn )
{
	return (const short*) &rgn[ 1 ];
}

static inline
size_t byte_distance( const void* begin, const void* end )
{
	return (const char*) end - (const char*) begin;
}

static inline
bool odd_count_between( const short* begin, const short* end )
{
	return byte_distance( begin, end ) & 2;
}

pascal Boolean PtInRgn_patch( Point pt, RgnHandle rgn )
{
	if ( ! PtInRect( pt, &rgn[0]->rgnBBox ) )
	{
		return false;
	}
	
	if ( const bool is_rectangular = rgn[0]->rgnSize <= 10 )
	{
		return true;
	}
	
	bool contained = false;
	
	const short* extent = rgn_extent( *rgn );
	
	while ( *extent++ <= pt.v )
	{
		const short* h1 = extent;
		
		while ( *extent <= pt.h )
		{
			++extent;
		}
		
		contained ^= odd_count_between( h1, extent );
		
		while ( *extent++ != Region_end )  continue;
	}
	
	return contained;
}


pascal Boolean RectInRgn_patch( const Rect* r, RgnHandle rgn )
{
	/*
		Inside Macintosh Volume I documents that RectInRgn() sometimes
		returns true when the rectangle merely intersects the region's
		bounding box but not the region itself.  It's not clear if Apple's
		implementation is more than a glorified SectRect() call, but in any
		case the documentation doesn't require it.
	*/
	
	Rect intersection;
	
	return SectRect( r, &rgn[0]->rgnBBox, &intersection );
}

pascal Boolean EqualRgn_patch( RgnHandle a, RgnHandle b )
{
	if ( a == b )
	{
		return true;
	}
	
	if ( a[0]->rgnSize != b[0]->rgnSize )
	{
		return false;
	}
	
	if ( EmptyRgn( a )  &&  EmptyRgn( b ) )
	{
		return true;
	}
	
	return fast_memcmp( *a, *b, a[0]->rgnSize ) == 0;
}

pascal Boolean EmptyRgn_patch( RgnHandle rgn )
{
	/*
		Empty regions should be 0,0 - 0,0, but Postel's Law suggests we
		allow for the caller manipulating the bounding box directly --
		i.e. don't just check for all zero data.
	*/
	
	return EmptyRect( &rgn[0]->rgnBBox );
}
