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

enum
{
	rts = 0x4E75,
};

enum
{
	sizeof_rect   = sizeof (short) * 4,
	sizeof_region = sizeof (short) * 5,
};

static
bool is_valid_region( const short* rgn, short size )
{
	if ( size & 1  ||  size < 12 )
	{
		return false;
	}
	
	const short* end = (const short*) ((const char*) rgn + size);
	
	++rgn;
	
	const short top    = *rgn++;
	const short left   = *rgn++;
	const short bottom = *rgn++;
	const short right  = *rgn++;
	
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
	
	if ( bottom == 0x7FFF  ||  right == 0x7FFF )
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
	
	short last_v = -32768;
	
	short v = *rgn++;
	
	if ( v != top )
	{
		return false;
	}
	
	while ( v != 0x7FFF )
	{
		if ( v <= last_v  ||  rgn == end )
		{
			return false;
		}
		
		last_v = v;
		
		++v_count;
		
		short h_count = 0;
		
		short last_h = -32768;
		
		short h = *rgn++;
		
		if ( h == left )
		{
			hit_left = true;
		}
		else if ( h < left )
		{
			return false;
		}
		
		while ( h != 0x7FFF )
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
	          (src_size = u16_from_big( *(short*) p )) >= sizeof_region      &&
	          (p = s.translate( src, src_size, data_space, mem_read ))       &&
	          (q = s.translate( dst, src_size, data_space, mem_write ))      &&
	          ((long) q & 0x1) == 0;
	
	if ( ! ok )
	{
		return ((long) p | (long) q | (long) r) & 0x1 ? v68k::Address_error
		                                              : v68k::Bus_error;
	}
	
	const long extent_size = src_size - sizeof_region;
	
	const short* r2 = (const short*) r;
	const short* p2 = (const short*) p;
	
	const short* rect = r2;
	
	if ( iota::is_little_endian() )
	{
		short* r = (short*) alloca( sizeof (short) * 8 );
		
		rect = r;
		
		*r++ = u16_from_big( *r2++ );
		*r++ = u16_from_big( *r2++ );
		*r++ = u16_from_big( *r2++ );
		*r++ = u16_from_big( *r2++ );
		
		short* p = (short*) alloca( src_size );
		
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
	
	const short* bbox = ++p2;
	
	p2 += 4;
	
	short* q2 = (short*) q + 5;
	
	quickdraw::segments_box segments( (short*) alloca( src_size ) );
	
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
	          (a_size = u16_from_big( *(short*) p )) >= sizeof_region        &&
	          (p = s.translate( one, a_size, data_space, mem_read ))         &&
	          (q = s.translate( two, sizeof_region, data_space, mem_read ))  &&
	          ((long) q & 0x1) == 0                                          &&
	          (b_size = u16_from_big( *(short*) q )) >= sizeof_region        &&
	          (q = s.translate( two, b_size, data_space, mem_read ))         &&
	          ((r_size = a_size + b_size), true)                             &&
	          (r = s.translate( dst, r_size, data_space, mem_write ))        &&
	          ((long) r & 0x1) == 0;
	
	if ( ! ok )
	{
		return ((long) p | (long) q | (long) r) & 0x1 ? v68k::Address_error
		                                              : v68k::Bus_error;
	}
	
	const short* p2 = (const short*) p;
	const short* q2 = (const short*) q;
	
	if ( iota::is_little_endian() )
	{
		short* p = (short*) alloca( a_size );
		
		for ( int i = 0;  i < a_size / 2;  ++i )
		{
			p[ i ] = u16_from_big( p2[ i ] );
		}
		
		p2 = p;
		
		
		short* q = (short*) alloca( b_size );
		
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
	
	const short* bbox = ++p2;
	
	p2 += 4;
	q2 += 5;
	
	short* r2 = (short*) r + 5;
	
	quickdraw::segments_box a_segments( (short*) alloca( a_size ) );
	quickdraw::segments_box b_segments( (short*) alloca( b_size ) );
	quickdraw::segments_box c_segments( (short*) alloca( r_size ) );
	quickdraw::segments_box r_segments( (short*) alloca( r_size ) );
	
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
	          (a_size = u16_from_big( *(short*) p )) >= sizeof_region        &&
	          (p = s.translate( one, a_size, data_space, mem_read ))         &&
	          (q = s.translate( two, sizeof_region, data_space, mem_read ))  &&
	          ((long) q & 0x1) == 0                                          &&
	          (b_size = u16_from_big( *(short*) q )) >= sizeof_region        &&
	          (q = s.translate( two, b_size, data_space, mem_read ))         &&
	          ((r_size = a_size + b_size), true)                             &&
	          (r = s.translate( dst, r_size, data_space, mem_write ))        &&
	          ((long) r & 0x1) == 0;
	
	if ( ! ok )
	{
		return ((long) p | (long) q | (long) r) & 0x1 ? v68k::Address_error
		                                              : v68k::Bus_error;
	}
	
	const short* p2 = (const short*) p;
	const short* q2 = (const short*) q;
	
	if ( iota::is_little_endian() )
	{
		short* p = (short*) alloca( a_size );
		
		for ( int i = 0;  i < a_size / 2;  ++i )
		{
			p[ i ] = u16_from_big( p2[ i ] );
		}
		
		p2 = p;
		
		
		short* q = (short*) alloca( b_size );
		
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
	
	short* r2 = (short*) r + 5;
	
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
