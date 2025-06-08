/*
	conv/utf8_16.cc
	---------------
*/

#include "conv/utf8_16.hh"

// chars
#include "encoding/utf16.hh"
#include "encoding/utf8.hh"


namespace conv
{

using chars::unichar_t;


static inline
unsigned measure_utf8_bytes_for_utf16( utf16_t uc )
{
	return   uc <   0x80 ? 1  // ASCII
	       : uc <  0x800 ? 2
	       : uc < 0xD800 ? 3
	       : uc < 0xE000 ? 2  // count each surrogate pair half as 2 bytes
	       :               3;
}

std::size_t sizeof_utf8_from_utf16( const utf16_t* begin, const utf16_t* end )
{
	std::size_t size = 0;
	
	for ( const utf16_t* p = begin;  p < end;  ++p )
	{
		size += measure_utf8_bytes_for_utf16( *p );
	}
	
	return size;
}

std::size_t utf8_from_utf16_nothrow( char*            buffer_out,
                                     std::size_t      length,
                                     const utf16_t**  pp_in,
                                     std::size_t      n )
{
	const char* buffer_end = buffer_out + length;
	
	const utf16_t*& p = *pp_in;
	
	const utf16_t* end = p + n;
	
	char* q = buffer_out;
	
	while ( p < end )
	{
		const unichar_t uc = chars::get_next_code_point_from_utf16( p, end );
		
		if ( (int) uc < 0 )
		{
			return invalid_utf16;
		}
		
		const unsigned n_bytes = chars::measure_utf8_bytes_for_unicode( uc );
		
		if ( q + n_bytes > buffer_end )
		{
			break;
		}
		
		q = chars::put_code_point_into_utf8( uc, n_bytes, q );
	}
	
	return q - buffer_out;
}

std::size_t utf8_from_utf16( char*            buffer,
                             std::size_t      length,
                             const utf16_t**  pp_in,
                             std::size_t      n )
{
	std::size_t result = utf8_from_utf16_nothrow( buffer, length, pp_in, n );
	
	switch ( result )
	{
		case invalid_utf16:  throw utf16_decoding_error();
		
		default:
			return result;
	}
}

}
