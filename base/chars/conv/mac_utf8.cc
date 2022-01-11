/*
	conv/mac_utf8.cc
	----------------
*/

#include "conv/mac_utf8.hh"

// more-libc
#include "more/string.h"

// chars
#include "charsets/ascii.hh"
#include "charsets/extended_ascii.hh"
#include "charsets/MacRoman.hh"
#include "encoding/utf8.hh"


namespace conv
{
	
	using chars::unichar_t;
	
	
	static inline
	std::size_t min( std::size_t a, std::size_t b )
	{
		return b < a ? b : a;
	}
	
	static inline
	const char* find_non_ascii( const char* begin, const char* end )
	{
		return chars::find_non_ascii( begin, end, end );
	}
	
	static inline unichar_t unicode_from_MacRoman( char c )
	{
		using chars::unicode_from_extended_ascii;
		using chars::MacRoman_decoder_table;
		
		return unicode_from_extended_ascii( c, MacRoman_decoder_table );
	}
	
	static inline char MacRoman_from_unicode( unichar_t uc )
	{
		using chars::extended_ascii_from_unicode;
		using chars::MacRoman_encoder_map;
		
		return extended_ascii_from_unicode( uc, MacRoman_encoder_map );
	}
	
	std::size_t sizeof_utf8_from_mac( const char* begin, const char* end )
	{
		std::size_t size = 0;
		
		for ( const char* p = begin;  p < end;  ++p )
		{
			const unichar_t uc = unicode_from_MacRoman( *p );
			
			size += chars::measure_utf8_bytes_for_unicode( uc );
		}
		
		return size;
	}
	
	std::size_t sizeof_mac_from_utf8( const char* begin, const char* end )
	{
		const char* p = begin;
		
		std::size_t size = 0;
		
		while ( p < end )
		{
			++size;
			
			const unsigned n_bytes = chars::count_utf8_bytes_in_char( *p );
			
			if ( n_bytes == 0 )
			{
				// subsequent conversion will throw exception
				break;
			}
			
			p += n_bytes;
		}
		
		return size;
	}
	
	std::size_t utf8_from_mac( char*         buffer_out,
	                           std::size_t   length,
	                           const char**  pp_in,
	                           std::size_t   n )
	{
		const char* buffer_end = buffer_out + length;
		
		const char*& p = *pp_in;
		
		const char* end = p + n;
		
		char* q = buffer_out;
		
		while ( p < end )
		{
			const std::size_t remaining = min( end - p, buffer_end - q );
			
			const char* it = find_non_ascii( p, p + remaining );
			
			q = (char*) mempcpy( q, p, it - p );
			
			p = it;
			
			if ( p == end )
			{
				break;
			}
			
			const unichar_t uc = unicode_from_MacRoman( *p );
			
			const unsigned n_bytes = chars::measure_utf8_bytes_for_unicode( uc );
			
			if ( q + n_bytes > buffer_end )
			{
				break;
			}
			
			q = chars::put_code_point_into_utf8( uc, n_bytes, q );
			
			++p;
		}
		
		return q - buffer_out;
	}
	
	std::size_t mac_from_utf8( char*         buffer,
	                           std::size_t   length,
	                           const char**  pp_in,
	                           std::size_t   n )
	{
		std::size_t result = mac_from_utf8_nothrow( buffer, length, pp_in, n );
		
		switch ( result )
		{
			case invalid_utf8:  throw utf8_decoding_error();
			case non_Mac_utf8:  throw chars::unrepresentable_character();
			
			default:
				return result;
		}
	}
	
	std::size_t mac_from_utf8_nothrow( char*         buffer_out,
	                                   std::size_t   length,
	                                   const char**  pp_in,
	                                   std::size_t   n )
	{
		const char* buffer_end = buffer_out + length;
		
		const char*& p = *pp_in;
		
		const char* end = p + n;
		
		char* q = buffer_out;
		
		while ( p < end )
		{
			const std::size_t remaining = min( end - p, buffer_end - q );
			
			const char* it = find_non_ascii( p, p + remaining );
			
			q = (char*) mempcpy( q, p, it - p );
			
			p = it;
			
			if ( p == end )
			{
				break;
			}
			
			const unichar_t uc = chars::get_next_code_point_from_utf8( p, end );
			
			if ( !~uc )
			{
				return invalid_utf8;
			}
			
			if ( const char c = MacRoman_from_unicode( uc ) )
			{
				*q++ = c;
			}
			else
			{
				return non_Mac_utf8;
			}
		}
		
		return q - buffer_out;
	}
	
}
