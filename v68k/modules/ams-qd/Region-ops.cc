/*
	Region-ops.cc
	-------------
*/

#include "Region-ops.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// iota
#include "iota/swap.hh"

// debug
#include "debug/assert.hh"

// quickdraw
#include "qd/regions.hh"
#include "qd/region_detail.hh"
#include "qd/region_scanner.hh"
#include "qd/sect_region.hh"
#include "qd/xor_region.hh"

// ams-qd
#include "Rect-utils.hh"
#include "Rects.hh"
#include "Regions.hh"
#include "segments_box.hh"


using quickdraw::offset_region;
using quickdraw::Region_end;
using quickdraw::region_geometry;
using quickdraw::region_scanner;
using quickdraw::sect_rect_region;
using quickdraw::sect_regions;
using quickdraw::set_region_bbox;
using quickdraw::xor_region;

typedef quickdraw::region_geometry_t geometry_t;


short MemErr : 0x0220;


static inline short min( short a, short b )
{
	return b < a ? b : a;
}

static inline short max( short a, short b )
{
	return a > b ? a : b;
}

static inline const short* rgn_extent( const MacRegion* rgn )
{
	return (const short*) &rgn[ 1 ];
}

static inline short* rgn_extent( MacRegion* rgn )
{
	return (short*) &rgn[ 1 ];
}

static geometry_t region_geometry( const MacRegion* rgn )
{
	return region_geometry( rgn_extent( rgn ) );
}

static size_t max_new_region_size( geometry_t g )
{
	return sizeof (MacRegion) + g.n_v_coords * (g.n_h_coords + 2) * 2 + 2;
}

static size_t max_new_region_size( geometry_t a, geometry_t b )
{
	const short v =      a.n_v_coords + b.n_v_coords;
	const short h = max( a.n_h_coords,  b.n_h_coords );
	
	const geometry_t aggregate_geometry = { v, h };
	
	return max_new_region_size( aggregate_geometry );
}

static void extend_rectangular_region( const MacRegion& in, short* out )
{
	// output buffer must be at least 28 bytes
	
	*out++ = in.rgnSize;
	
	*out++ = in.rgnBBox.top;
	*out++ = in.rgnBBox.left;
	*out++ = in.rgnBBox.bottom;
	*out++ = in.rgnBBox.right;
	
	*out++ = in.rgnBBox.top;
	*out++ = in.rgnBBox.left;
	*out++ = in.rgnBBox.right;
	*out++ = Region_end;
	
	*out++ = in.rgnBBox.bottom;
	*out++ = in.rgnBBox.left;
	*out++ = in.rgnBBox.right;
	*out++ = Region_end;
	
	*out++ = Region_end;
}

static RgnHandle reify_region( RgnHandle in, short** out )
{
	if ( in[0]->rgnSize > sizeof (MacRegion) )
	{
		return in;
	}
	
	extend_rectangular_region( **in, *out );
	
	return (RgnHandle) out;
}

static short region_size( MacRegion* region, const short* end )
{
	const short rgn_size = (char*) end - (char*) region;
	
	if ( rgn_size <= 28 )
	{
		const geometry_t g = region_geometry( region );
		
		if ( g.n_v_coords <= 2  &&  g.n_h_coords <= 2 )
		{
			return sizeof (MacRegion);
		}
	}
	
	return rgn_size;
}

static
bool is_valid_region( RgnHandle rgn )
{
	const short End = 0x7FFF;
	
	ASSERT(  rgn != NULL );
	ASSERT( *rgn != NULL );
	
	const Size h_size = GetHandleSize( (Handle) rgn );
	
	ASSERT( h_size >= sizeof (MacRegion) );
	
	const short rgn_size = rgn[0]->rgnSize;
	
	ASSERT( h_size >= rgn_size );
	
	ASSERT( rgn_size % 2 == 0 );
	
	Rect bbox = rgn[0]->rgnBBox;
	
	ASSERT( bbox.top <= bbox.bottom );
	ASSERT( bbox.left <= bbox.right );
	
	if ( rgn_size == sizeof (MacRegion) )
	{
		return true;
	}
	
	ASSERT( rgn_size >= 28 );
	
	const short n = rgn_size / 2u - sizeof (MacRegion) / 2;
	
	const short* extent = rgn_extent( *rgn );
	
	ASSERT( extent[ n - 1 ] == End );  // vertical sentinel
	ASSERT( extent[ n - 2 ] == End );  // horizontal sentinel of last row
	
	short top  = bbox.top;
	short left = 32767;
	short bottom = -32767;
	short right  = -32767;
	
	EXPECT( extent[ 0 ] == top );
	
	const short* p = extent;
	
	short v = -32768;
	
	while ( *p != End )
	{
		ASSERT( *p > v );
		
		v = *p++;
		
		top    = min( top,    v );
		bottom = max( bottom, v );
		
		ASSERT( *p != End );
		
		short h = -32768;
		
		while ( *p != End )
		{
			EXPECT( *p > h );
			ASSERT( *p >= h );
			
			h = *p++;
			
			left  = min( left,  h );
			right = max( right, h );
		}
		
		++p;
	}
	
	ASSERT( top    == bbox.top    );
	EXPECT( left   == bbox.left   );
	EXPECT( bottom == bbox.bottom );
	EXPECT( right  == bbox.right  );
	
	return true;
}

pascal short BitMapToRegion_patch( MacRegion** rgn, const BitMap* bitmap )
{
	typedef unsigned short uint16_t;
	
	const Rect& bounds = bitmap->bounds;
	
	if ( bounds.bottom <= bounds.top  ||  bounds.right <= bounds.left )
	{
		SetEmptyRgn( rgn );
		
		return noErr;
	}
	
	const short rowBytes = bitmap->rowBytes;
	
	const UInt16 height = bounds.bottom - bounds.top;
	const UInt16 width  = bounds.right - bounds.left;
	
	// width and height are at least 1 and at most 65534 (32767 - -32767)
	
	const UInt16 max_h_coords = width + 1 & ~0x1;
	
	const unsigned max_h_bytes = (1 + max_h_coords + 1) * 2;
	const unsigned max_rgn_size = 10 + max_h_bytes * (height + 1) + 2;
	
	SetHandleSize( (Handle) rgn, max_rgn_size + rowBytes );
	
	uint16_t* temp_space = (uint16_t*) *rgn + max_rgn_size / 2;
	
	if ( MemErr )
	{
		return MemErr;
	}
	
	int margin = -width & 0xF;
	
	short v = bounds.top;
	
	short* extent = rgn_extent( *rgn );
	
	region_scanner scanner( extent, temp_space, rowBytes );
	
	const uint16_t* p = (const uint16_t*) bitmap->baseAddr;
	
	const uint16_t* prev = temp_space;
	
	do
	{
		scanner.scan( bounds.left, v, p, prev, margin );
		
		prev = p;
		
		p += rowBytes / 2u;
	}
	while ( ++v < bounds.bottom );
	
	scanner.finish( bounds.left, v, prev, margin );
	
	if ( *extent == Region_end )
	{
		SetEmptyRgn( rgn );
		
		return noErr;
	}
	
	const short* end = set_region_bbox( &rgn[0]->rgnBBox.top, extent );
	
	const short rgn_size = region_size( *rgn, end );
	
	rgn[0]->rgnSize = rgn_size;
	
	SetHandleSize( (Handle) rgn, rgn_size );
	
	return noErr;
}

pascal void OffsetRgn_patch( MacRegion** rgn, short dh, short dv )
{
	ASSERT( is_valid_region( rgn ) );
	
	rgn[0]->rgnBBox.top    += dv;
	rgn[0]->rgnBBox.left   += dh;
	rgn[0]->rgnBBox.bottom += dv;
	rgn[0]->rgnBBox.right  += dh;
	
	if ( rgn[0]->rgnSize > sizeof (MacRegion) )
	{
		offset_region( rgn_extent( *rgn ), dh, dv );
	}
}

/*
	For {squish,stretch}_region_[hv]():
	
	tmp must be a copy of rgn.
*/

static
void squish_region_h( RgnHandle rgn, short dh, RgnHandle tmp )
{
	for ( short i = dh;  i > 0;  --i )
	{
		OffsetRgn( rgn, -1, 0 );
		SectRgn( rgn, tmp, rgn );
		
		OffsetRgn( tmp, 1, 0 );
		SectRgn( rgn, tmp, rgn );
	}
}

static
void squish_region_v( RgnHandle rgn, short dv, RgnHandle tmp )
{
	for ( short i = dv;  i > 0;  --i )
	{
		OffsetRgn( rgn, 0, -1 );
		SectRgn( rgn, tmp, rgn );
		
		OffsetRgn( tmp, 0, 1 );
		SectRgn( rgn, tmp, rgn );
	}
}

/*
	For stretch_region_h() and stretch_region_v():
	
	dh/dv is negative.
*/

static
void stretch_region_h( RgnHandle rgn, short dh, RgnHandle tmp )
{
	for ( short i = dh;  i < 0;  ++i )
	{
		OffsetRgn( rgn, 0, -1 );
		UnionRgn( rgn, tmp, rgn );
		
		OffsetRgn( tmp, 0, 1 );
		UnionRgn( rgn, tmp, rgn );
	}
}

static
void stretch_region_v( RgnHandle rgn, short dv, RgnHandle tmp )
{
	for ( short i = dv;  i < 0;  ++i )
	{
		OffsetRgn( rgn, -1, 0 );
		UnionRgn( rgn, tmp, rgn );
		
		OffsetRgn( tmp, 1, 0 );
		UnionRgn( rgn, tmp, rgn );
	}
}

pascal void InsetRgn_patch ( MacRegion** rgn, short dh, short dv )
{
	static RgnHandle tmp = NewRgn();
	
	if ( dh != 0 )
	{
		CopyRgn( rgn, tmp );
		
		if ( dh > 0 )
		{
			squish_region_h( rgn, dh, tmp );
		}
		else
		{
			stretch_region_h( rgn, dh, tmp );
		}
	}
	
	if ( dv != 0 )
	{
		CopyRgn( rgn, tmp );
		
		if ( dv > 0 )
		{
			squish_region_v( rgn, dv, tmp );
		}
		else
		{
			stretch_region_v( rgn, dv, tmp );
		}
	}
}

static void finish_region( RgnHandle r )
{
	if ( const bool empty = *rgn_extent( *r ) == Region_end )
	{
		SetEmptyRgn( r );
	}
	else
	{
		const short* end = set_region_bbox( &r[0]->rgnBBox.top, rgn_extent( *r ) );
		
		const size_t size = r[0]->rgnSize = region_size( *r, end );
		
		SetHandleSize( (Handle) r, size );
	}
}

static void sect_regions( RgnHandle a, RgnHandle b, RgnHandle dst )
{
	ASSERT( is_valid_region( a ) );
	ASSERT( is_valid_region( b ) );
	
	ASSERT( a[0]->rgnSize > sizeof (MacRegion) );
	ASSERT( b[0]->rgnSize > sizeof (MacRegion) );
	
	ASSERT( EqualRect_patch( &a[0]->rgnBBox, &b[0]->rgnBBox ) );
	
	const size_t a_max_bytes = a[0]->rgnSize;
	const size_t b_max_bytes = b[0]->rgnSize;
	
	SetHandleSize( (Handle) dst, a_max_bytes + b_max_bytes );  // TODO:  Prove this is enough
	
	const size_t r_max_bytes = a_max_bytes + b_max_bytes;
	
	segments_box a_segments( a_max_bytes );
	segments_box b_segments( b_max_bytes );
	segments_box c_segments( r_max_bytes );
	segments_box r_segments( r_max_bytes );
	
	sect_regions( (const short*) &a[0]->rgnBBox,
	              rgn_extent( *a ),
	              a_segments,
	              rgn_extent( *b ),
	              b_segments,
	              c_segments,
	              r_segments,
	              rgn_extent( *dst ) );
	
	finish_region( dst );
}

static void sect_rect_region( const Rect& rect, RgnHandle src, RgnHandle dst )
{
	ASSERT( is_valid_region( src ) );
	
	if ( src == dst )
	{
		static RgnHandle tmp = NewRgn();
		
		CopyRgn( src, tmp );
		
		src = tmp;
	}
	
	const size_t max_bytes = src[0]->rgnSize;
	
	SetHandleSize( (Handle) dst, max_bytes );  // TODO:  Prove this is enough
	
	if ( max_bytes == sizeof (MacRegion) )
	{
		RectRgn( dst, &rect );
		
		return;
	}
	
	segments_box segments( max_bytes );
	
	sect_rect_region( (const short*) &rect,
	                  (const short*) &src[0]->rgnBBox,
	                  rgn_extent( *src ),
	                  segments,
	                  rgn_extent( *dst ) );
	
	finish_region( dst );
}

pascal void SectRgn_patch( MacRegion** a, MacRegion** b, MacRegion** dst )
{
	ASSERT( is_valid_region( a ) );
	ASSERT( is_valid_region( b ) );
	
	if ( empty_rect( a[0]->rgnBBox )  ||  empty_rect( b[0]->rgnBBox ) )
	{
		SetEmptyRgn( dst );
		
		return;
	}
	
	// Neither input region is empty.
	
	Rect bbox_intersection;
	
	const bool intersected = SectRect( &a[0]->rgnBBox, &b[0]->rgnBBox, &bbox_intersection );
	
	if ( !intersected )
	{
		SetEmptyRgn( dst );
		
		return;
	}
	
	// The input regions have overlapping bounding boxes.
	
	if ( a[0]->rgnSize > b[0]->rgnSize )
	{
		using iota::swap;
		
		swap( a, b );
	}
	
	const bool a_is_rectangular = a[0]->rgnSize <= 10;
	const bool b_is_rectangular = b[0]->rgnSize <= 10;
	
	/*
		Because we sorted a and b by region size, a's size is no larger
		than b's.  So b_is_rectangular implies a_is_rectangular.
	*/
	
	if ( b_is_rectangular )
	{
		RectRgn( dst, &bbox_intersection );
		
		return;
	}
	
	// At least one of the input regions (b) is not rectangular.
	
	const Rect& a_bbox = a[0]->rgnBBox;
	const Rect& b_bbox = b[0]->rgnBBox;
	
	if ( a_is_rectangular  &&  EqualRect( &bbox_intersection, &b_bbox ) )
	{
		// One of the input regions (a) is a rectangle enclosing the other.
		
		CopyRgn( b, dst );
		
		return;
	}
	
	if ( a_is_rectangular )
	{
		// One region (a) is rectangular and clips the other region.
		
		sect_rect_region( bbox_intersection, b, dst );
		
		return;
	}
	
	/*
		Neither input region is rectangular.
		Clip each input region to the intersection of the bounding boxes.
	*/
	
	static RgnHandle one = NewRgn();
	static RgnHandle two = NewRgn();
	
	sect_rect_region( bbox_intersection, a, one );  // clobbers one
	sect_rect_region( bbox_intersection, b, two );  // clobbers two
	
	a = one;
	b = two;
	
	while ( true )
	{
		Rect old_intersection = bbox_intersection;
		
		/*
			Reexamine the intersection of the bounding boxes.
			If they don't intersect, we're done.
		*/
		
		if ( !SectRect( &a[0]->rgnBBox, &b[0]->rgnBBox, &bbox_intersection ) )
		{
			SetEmptyRgn( dst );
			
			break;
		}
		
		/*
			Each input region has been clipped against the old intersection.
			Therefore, the old intersection encloses its bounding box.
			The new intersection is enclosed by each bounding box.
			If the new and old intersections match, then the bounding boxes
			must also be equal.
		*/
		
		if ( EqualRect( &bbox_intersection, &old_intersection ) )
		{
			if ( a[0]->rgnSize > b[0]->rgnSize )
			{
				using iota::swap;
				
				swap( a, b );
			}
			
			if ( a[0]->rgnSize <= 10 )
			{
				CopyRgn( b, dst );
				
				break;
			}
			
			// Precondition:  a and b are non-rect regions with equal bboxes
			
			sect_regions( a, b, dst );
			
			break;
		}
		
		// Clip a and b against the new intersection...
		
		if ( !EqualRect( &a[0]->rgnBBox, &bbox_intersection ) )
		{
			sect_rect_region( bbox_intersection, a, a );
		}
		
		if ( !EqualRect( &b[0]->rgnBBox, &bbox_intersection ) )
		{
			sect_rect_region( bbox_intersection, b, b );
		}
		
		// ... and try again.
	}
}

static RgnHandle union_diff_tmp = NewRgn_patch();

pascal void UnionRgn_patch( MacRegion** a, MacRegion** b, MacRegion** dst )
{
	RgnHandle tmp = union_diff_tmp;
	
	SectRgn( a, b, tmp );
	
	XorRgn( a, tmp, tmp );
	XorRgn( b, tmp, dst );
}

pascal void DiffRgn_patch( MacRegion** a, MacRegion** b, MacRegion** dst )
{
	if ( empty_rect( a[0]->rgnBBox ) )
	{
		SetEmptyRgn( dst );
		
		return;
	}
	
	RgnHandle tmp = union_diff_tmp;
	
	SectRgn( a, b, tmp );
	
	XorRgn( a, tmp, dst );
}

pascal void XOrRgn_patch( MacRegion** a, MacRegion** b, MacRegion** dst )
{
	ASSERT( is_valid_region( a ) );
	ASSERT( is_valid_region( b ) );
	
	if ( empty_rect( a[0]->rgnBBox ) )
	{
		CopyRgn( b, dst );
		
		return;
	}
	
	if ( empty_rect( b[0]->rgnBBox ) )
	{
		CopyRgn( a, dst );
		
		return;
	}
	
	short a_rect_extent[ 14 ];
	short b_rect_extent[ 14 ];
	
	short* a_rect = a_rect_extent;
	short* b_rect = b_rect_extent;
	
	a = reify_region( a, &a_rect );
	b = reify_region( b, &b_rect );
	
	const geometry_t g_a = region_geometry( *a );
	const geometry_t g_b = region_geometry( *b );
	
	const size_t size = max_new_region_size( g_a, g_b );
	
	Handle h = NewHandle( size );
	
	RgnHandle r = (RgnHandle) h;
	
	xor_region( rgn_extent( *a ), rgn_extent( *b ), rgn_extent( *r ) );
	
	if ( *rgn_extent( *r ) == Region_end )
	{
		SetEmptyRgn( dst );
	}
	else
	{
		const short* end = set_region_bbox( &r[0]->rgnBBox.top, rgn_extent( *r ) );
		
		r[0]->rgnSize = region_size( *r, end );
		
		CopyRgn( r, dst );
	}
	
	DisposeHandle( h );
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

pascal unsigned char PtInRgn_patch( Point pt, MacRegion** rgn )
{
	if ( ! PtInRect_patch( pt, &rgn[0]->rgnBBox ) )
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
