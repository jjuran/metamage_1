/*
	regions.cc
	----------
*/

#include "callout/regions.hh"

// Standard C
#include <stdlib.h>

// iota
#include "iota/endian.hh"

// log-of-war
#include "logofwar/report.hh"

// quickdraw
#include "qd/sect_region.hh"
#include "qd/segments.hh"
#include "qd/xor_region.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


#ifndef NULL
#define NULL  0L
#endif


namespace v68k    {
namespace callout {

typedef unsigned char Byte;

typedef short coord_t;

typedef unsigned short coord_t_BE;

enum
{
	rts = 0x4E75,
};

enum
{
	sizeof_rect   = sizeof (coord_t) * 4,
	sizeof_region = sizeof (coord_t) * 5,
	
	min_rgn_size  =    28,  // minimum size of a non-bbox-only region
	max_rgn_size  = 32766,
	
	Region_end    = 0x7FFF,
	Region_end_BE = 0xFF7F,
};

static
bool is_valid_region( const coord_t* rgn, short size )
{
	if ( size & 1  ||  size < 12 )
	{
		return false;
	}
	
	const coord_t* end = (const coord_t*) ((const char*) rgn + size);
	
	++rgn;
	
	const coord_t top    = *rgn++;
	const coord_t left   = *rgn++;
	const coord_t bottom = *rgn++;
	const coord_t right  = *rgn++;
	
	bool hit_left  = false;
	bool hit_right = false;
	
	if ( top >= bottom  ||  left >= right )
	{
		return false;
	}
	
	if ( top == -32768  ||  left == -32768 )
	{
		/*
			-32768 is outside of QuickDraw coordinate space.
		*/
		
		return false;
	}
	
	if ( bottom == Region_end  ||  right == Region_end )
	{
		/*
			32767 is within QuickDraw coordinate space,
			but it's used as a sentinel value and hence
			can't be used for complex regions.
			
			A rectangular region extending down or right to 32767
			is fine, but we don't handle those in this function.
		*/
		
		return false;
	}
	
	short v_count = 0;
	
	coord_t last_v = -32768;
	
	coord_t v = *rgn++;
	
	if ( v != top )
	{
		return false;
	}
	
	while ( v != Region_end )
	{
		if ( v <= last_v  ||  rgn == end )
		{
			return false;
		}
		
		last_v = v;
		
		++v_count;
		
		short h_count = 0;
		
		coord_t last_h = -32768;
		
		coord_t h = *rgn++;
		
		if ( h == left )
		{
			hit_left = true;
		}
		else if ( h < left )
		{
			return false;
		}
		
		while ( h != Region_end )
		{
			if ( h <= last_h  ||  rgn == end )
			{
				return false;
			}
			
			last_h = h;
			
			++h_count;
			
			h = *rgn++;
		}
		
		if ( last_h == right )
		{
			hit_right = true;
		}
		else if ( last_h > right )
		{
			return false;
		}
		
		if ( h_count == 0  ||  h_count & 1  ||  rgn == end )
		{
			return false;
		}
		
		v = *rgn++;
	}
	
	return last_v == bottom  &&  hit_left  &&  hit_right;
}

static
const coord_t_BE* find_full_stop( const coord_t_BE* p, const coord_t_BE* limit )
{
	while ( p < limit )
	{
		if ( *p++ == Region_end_BE  &&  p < limit  &&  *p++ == Region_end_BE )
		{
			return p;
		}
	}
	
	return NULL;
}

static
coord_t scan_for_last_h_coord( const coord_t*& p, const coord_t* end )
{
	while ( p < end  &&  *p++ != Region_end )  continue;
	
	return p[ -2 ];
}

static
bool set_region_bbox( coord_t* bbox, const coord_t* p, const coord_t* end )
{
	*bbox++ = *p++;  // top
	
	coord_t left  = *p++;
	coord_t right = scan_for_last_h_coord( p, end );
	
	if ( p == end )
	{
		return false;
	}
	
	// pointing at second v coordinate
	
	coord_t v;
	coord_t bottom;
	
	while ( (v = *p++) != Region_end )
	{
		bottom = v;
		
		// pointing at first h coordinate in the line
		
		const coord_t next = *p;
		const coord_t last = scan_for_last_h_coord( p, end );
		
		// pointing at next v coordinate, or v terminator
		
		if ( p == end )
		{
			return false;
		}
		
		if ( next < left )
		{
			left = next;
		}
		
		if ( last > right )
		{
			right = last;
		}
	}
	
	*bbox++ = left;
	*bbox++ = bottom;
	*bbox++ = right;
	
	return true;
}

int32_t finish_region_callout( v68k::processor_state& s )
{
	using iota::u16_from_big;
	
	const v68k::function_code_t data_space = s.data_space();
	
	const uint32_t size = s.d(0);
	const uint32_t rgn  = s.a(1);
	
	Byte* p = NULL;
	
	bool ok = size >= sizeof_region                                  &&
	          size <= max_rgn_size                                   &&
	          (size & 0x1) == 0                                      &&
	          (p = s.translate( rgn, size, data_space, mem_write ))  &&
	          ((long) p & 0x1) == 0;
	
	if ( ! ok )
	{
		return (long) p & 0x1 ? v68k::Address_error
		                      : v68k::Bus_error;
	}
	
	s.d(0) = sizeof_region;
	
	coord_t_BE* p_BE = (coord_t_BE*) p;
	
	*p_BE = iota::big_u16( sizeof_region );
	
	for ( int i = 1;  i < 5;  ++i )
	{
		p_BE[ i ] = 0;
	}
	
	if ( size < min_rgn_size  ||  p_BE[ 5 ] == Region_end_BE )
	{
		s.translate( rgn, sizeof_region, data_space, mem_update );
		
		return rts;
	}
	
	const coord_t_BE* limit = (const coord_t_BE*) (p + size);
	
	const coord_t_BE* rgn_end = find_full_stop( p_BE, limit );
	
	if ( rgn_end == NULL )
	{
		return v68k::CHK_exception;
	}
	
	unsigned rgn_size = (const Byte*) rgn_end - p;
	
	coord_t* p2 = (coord_t*) p;
	
	if ( iota::is_little_endian() )
	{
		p2 = (coord_t*) alloca( rgn_size );
		
		for ( int i = 0;  i < rgn_size / 2;  ++i )
		{
			p2[ i ] = u16_from_big( p_BE[ i ] );
		}
	}
	
	p = (Byte*) p2;
	
	coord_t* bbox = p2 + 1;
	
	if ( rgn_size == min_rgn_size )
	{
		rgn_size = sizeof_region;  // bbox-only region
		
		const coord_t* p = p2 + 5;
		
		coord_t top = *p++;
		coord_t left = *p++;
		coord_t right = *p++;
		coord_t marker = *p++;
		coord_t bottom = *p++;
		
		if ( marker == Region_end  &&  *p++ == left  &&  *p++ == right )
		{
			bbox[ 0 ] = top;
			bbox[ 1 ] = left;
			bbox[ 2 ] = bottom;
			bbox[ 3 ] = right;
		}
	}
	else if ( set_region_bbox( bbox, p2 + 5, (const coord_t*) (p + rgn_size) ) )
	{
		if ( ! is_valid_region( p2, rgn_size ) )
		{
			return v68k::CHK_exception;
		}
	}
	else
	{
		return v68k::CHK_exception;
	}
	
	s.d(0) = *p2 = rgn_size;
	
	if ( iota::is_little_endian() )
	{
		for ( int i = 0;  i < 5;  ++i )
		{
			p_BE[ i ] = iota::big_u16( p2[ i ] );
		}
	}
	
	s.translate( rgn, sizeof_region, data_space, mem_update );
	
	return rts;
}

int32_t sect_rect_region_callout( v68k::processor_state& s )
{
	using iota::u16_from_big;
	
	const v68k::function_code_t data_space = s.data_space();
	
	const uint32_t box = s.d(0);
	const uint32_t src = s.a(0);
	const uint32_t dst = s.a(1);
	
	const Byte* r = NULL;
	const Byte* p = NULL;
	
	Byte* q = NULL;
	
	short src_size = 0;
	
	bool ok = (r = s.translate( box, sizeof_rect, data_space, mem_read ))    &&
	          ((long) r & 0x1) == 0                                          &&
	          (p = s.translate( src, sizeof_region, data_space, mem_read ))  &&
	          ((long) p & 0x1) == 0                                          &&
	          (src_size = u16_from_big( *(coord_t*) p )) >= sizeof_region    &&
	          src_size <= max_rgn_size                                       &&
	          (p = s.translate( src, src_size, data_space, mem_read ))       &&
	          (q = s.translate( dst, src_size, data_space, mem_write ))      &&
	          ((long) q & 0x1) == 0;
	
	if ( ! ok )
	{
		return ((long) p | (long) q | (long) r) & 0x1 ? v68k::Address_error
		                                              : v68k::Bus_error;
	}
	
	const long extent_size = src_size - sizeof_region;
	
	const coord_t* r2 = (const coord_t*) r;
	const coord_t* p2 = (const coord_t*) p;
	
	const coord_t* rect = r2;
	
	if ( iota::is_little_endian() )
	{
		coord_t* r = (coord_t*) alloca( sizeof (coord_t) * 8 );
		
		rect = r;
		
		*r++ = u16_from_big( *r2++ );
		*r++ = u16_from_big( *r2++ );
		*r++ = u16_from_big( *r2++ );
		*r++ = u16_from_big( *r2++ );
		
		coord_t* p = (coord_t*) alloca( src_size );
		
		for ( int i = 0;  i < src_size / 2;  ++i )
		{
			p[ i ] = u16_from_big( p2[ i ] );
		}
		
		p2 = p;
	}
	
	if ( ! is_valid_region( p2, src_size ) )
	{
		return v68k::CHK_exception;
	}
	
	const coord_t* bbox = ++p2;
	
	p2 += 4;
	
	coord_t* q2 = (coord_t*) q + 5;
	
	quickdraw::segments_box segments( (coord_t*) alloca( src_size ) );
	
	sect_rect_region( rect,
	                  bbox,
	                  p2,
	                  segments,
	                  q2 );
	
	if ( iota::is_little_endian() )
	{
		for ( int i = 0;  i < extent_size / 2;  ++i )
		{
			q2[ i ] = iota::big_u16( q2[ i ] );
		}
	}
	
	s.translate( dst, src_size, data_space, mem_update );
	
	return rts;
}

int32_t sect_regions_callout( v68k::processor_state& s )
{
	using iota::u16_from_big;
	
	const v68k::function_code_t data_space = s.data_space();
	
	const uint32_t one = s.a(0);
	const uint32_t two = s.a(1);
	const uint32_t dst = s.d(0);
	
	const Byte* p = NULL;
	const Byte* q = NULL;
	
	Byte* r = NULL;
	
	short a_size = 0;
	short b_size = 0;
	
	short r_size;
	
	bool ok = (p = s.translate( one, sizeof_region, data_space, mem_read ))  &&
	          ((long) p & 0x1) == 0                                          &&
	          (a_size = u16_from_big( *(coord_t*) p )) >= sizeof_region      &&
	          a_size <= max_rgn_size                                         &&
	          (p = s.translate( one, a_size, data_space, mem_read ))         &&
	          (q = s.translate( two, sizeof_region, data_space, mem_read ))  &&
	          ((long) q & 0x1) == 0                                          &&
	          (b_size = u16_from_big( *(coord_t*) q )) >= sizeof_region      &&
	          b_size <= max_rgn_size                                         &&
	          (q = s.translate( two, b_size, data_space, mem_read ))         &&
	          ((r_size = a_size + b_size), true)                             &&
	          (r = s.translate( dst, r_size, data_space, mem_write ))        &&
	          ((long) r & 0x1) == 0;
	
	if ( ! ok )
	{
		return ((long) p | (long) q | (long) r) & 0x1 ? v68k::Address_error
		                                              : v68k::Bus_error;
	}
	
	const coord_t* p2 = (const coord_t*) p;
	const coord_t* q2 = (const coord_t*) q;
	
	if ( iota::is_little_endian() )
	{
		coord_t* p = (coord_t*) alloca( a_size );
		
		for ( int i = 0;  i < a_size / 2;  ++i )
		{
			p[ i ] = u16_from_big( p2[ i ] );
		}
		
		p2 = p;
		
		
		coord_t* q = (coord_t*) alloca( b_size );
		
		for ( int i = 0;  i < b_size / 2;  ++i )
		{
			q[ i ] = u16_from_big( q2[ i ] );
		}
		
		q2 = q;
	}
	
	if ( ! is_valid_region( p2, a_size ) )
	{
		return v68k::CHK_exception;
	}
	
	if ( ! is_valid_region( q2, b_size ) )
	{
		return v68k::CHK_exception;
	}
	
	const coord_t* bbox = ++p2;
	
	p2 += 4;
	q2 += 5;
	
	coord_t* r2 = (coord_t*) r + 5;
	
	quickdraw::segments_box a_segments( (coord_t*) alloca( a_size ) );
	quickdraw::segments_box b_segments( (coord_t*) alloca( b_size ) );
	quickdraw::segments_box c_segments( (coord_t*) alloca( r_size ) );
	quickdraw::segments_box r_segments( (coord_t*) alloca( r_size ) );
	
	sect_regions( bbox,
	              p2,
	              a_segments,
	              q2,
	              b_segments,
	              c_segments,
	              r_segments,
	              r2 );
	
	if ( iota::is_little_endian() )
	{
		const long r_extent_size = r_size - sizeof_region;
		
		for ( int i = 0;  i < r_extent_size / 2;  ++i )
		{
			r2[ i ] = iota::big_u16( r2[ i ] );
		}
	}
	
	s.translate( dst, r_size, data_space, mem_update );
	
	return rts;
}

int32_t xor_regions_callout( v68k::processor_state& s )
{
	using iota::u16_from_big;
	
	const v68k::function_code_t data_space = s.data_space();
	
	const uint32_t one = s.a(0);
	const uint32_t two = s.a(1);
	const uint32_t dst = s.d(0);
	
	const Byte* p = NULL;
	const Byte* q = NULL;
	
	Byte* r = NULL;
	
	short a_size = 0;
	short b_size = 0;
	
	short r_size;
	
	bool ok = (p = s.translate( one, sizeof_region, data_space, mem_read ))  &&
	          ((long) p & 0x1) == 0                                          &&
	          (a_size = u16_from_big( *(coord_t*) p )) >= sizeof_region      &&
	          a_size <= max_rgn_size                                         &&
	          (p = s.translate( one, a_size, data_space, mem_read ))         &&
	          (q = s.translate( two, sizeof_region, data_space, mem_read ))  &&
	          ((long) q & 0x1) == 0                                          &&
	          (b_size = u16_from_big( *(coord_t*) q )) >= sizeof_region      &&
	          b_size <= max_rgn_size                                         &&
	          (q = s.translate( two, b_size, data_space, mem_read ))         &&
	          ((r_size = a_size + b_size), true)                             &&
	          (r = s.translate( dst, r_size, data_space, mem_write ))        &&
	          ((long) r & 0x1) == 0;
	
	if ( ! ok )
	{
		return ((long) p | (long) q | (long) r) & 0x1 ? v68k::Address_error
		                                              : v68k::Bus_error;
	}
	
	const coord_t* p2 = (const coord_t*) p;
	const coord_t* q2 = (const coord_t*) q;
	
	if ( iota::is_little_endian() )
	{
		coord_t* p = (coord_t*) alloca( a_size );
		
		for ( int i = 0;  i < a_size / 2;  ++i )
		{
			p[ i ] = u16_from_big( p2[ i ] );
		}
		
		p2 = p;
		
		
		coord_t* q = (coord_t*) alloca( b_size );
		
		for ( int i = 0;  i < b_size / 2;  ++i )
		{
			q[ i ] = u16_from_big( q2[ i ] );
		}
		
		q2 = q;
	}
	
	if ( ! is_valid_region( p2, a_size ) )
	{
//		return v68k::CHK_exception;
		
		WARNING = "Invalid region A in xor_regions()";
	}
	
	if ( ! is_valid_region( q2, b_size ) )
	{
//		return v68k::CHK_exception;
		
		WARNING = "Invalid region B in xor_regions()";
	}
	
	p2 += 5;
	q2 += 5;
	
	coord_t* r2 = (coord_t*) r + 5;
	
	quickdraw::xor_region( p2, q2, r2 );
	
	if ( iota::is_little_endian() )
	{
		const long r_extent_size = r_size - sizeof_region;
		
		for ( int i = 0;  i < r_extent_size / 2;  ++i )
		{
			r2[ i ] = iota::big_u16( r2[ i ] );
		}
	}
	
	s.translate( dst, r_size, data_space, mem_update );
	
	return rts;
}

}  // namespace callout
}  // namespace v68k
