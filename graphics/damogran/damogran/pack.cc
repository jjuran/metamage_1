/*
	pack.cc
	-------
*/

#include "damogran/pack.hh"

// Standard C
#include <string.h>


namespace damogran
{

typedef unsigned char byte_t;

static
const byte_t* find_next_aligned_pair( const byte_t* begin, const byte_t* end )
{
	const byte_t* p = begin;
	
	do
	{
		byte_t c0 = *p++;
		byte_t c1 = *p++;
		
		if ( c0 == c1 )
		{
			return p - 2;
		}
	}
	while ( p < end );
	
	return NULL;
}

static
const byte_t* find_next_aligned_2pair( const byte_t* begin, const byte_t* end )
{
	while ( const byte_t* p = find_next_aligned_pair( begin, end ) )
	{
		const short* p2 = (const short*) p;
		
		short pair = *p2++;
		
		if ( (const byte_t*) p2 == end )
		{
			return NULL;
		}
		
		if ( *p2 == pair )
		{
			return p;
		}
		
		begin = (const byte_t*) p2;
	}
	
	return NULL;
}

static
const short* advance_repeated_pairs( const short* begin, const short* end )
{
	const short* p = begin;
	
	short pair = *p++;
	
	while ( p < end )
	{
		if ( *p++ != pair )
		{
			return --p;
		}
	}
	
	return p;
}

static inline
const byte_t* advance_repeated_pairs( const byte_t* begin, const byte_t* end )
{
	return (const byte_t*) advance_repeated_pairs( (const short*) begin,
	                                               (const short*) end );
}

unsigned long pack_preflight( const uint8_t* src, const uint8_t* end )
{
	unsigned long packed_byte_count = 0;
	
	#define TOTAL  packed_byte_count
	
	#include "damogran/pack_body.hh"
	
	return packed_byte_count;
}

uint8_t* pack( const uint8_t* src, const uint8_t* end, uint8_t* dst )
{
	#define WRITE_1( c )       *dst++ = c
	#define WRITE_N( src, n )  memcpy( dst, src, n ); dst += n
	
	#include "damogran/pack_body.hh"
	
	return dst;
}

}  // namespace damogran
