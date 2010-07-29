/*
	conv/mac_utf8.cc
	----------------
*/

#include "conv/mac_utf8.hh"

// Standard C++
#include <algorithm>
#include <functional>

// chars
#include "charsets/extended_ascii.hh"
#include "charsets/MacRoman.hh"
#include "encoding/utf8.hh"


namespace conv
{
	
	using chars::unichar_t;
	
	
	static inline bool is_non_ascii( char c )
	{
		return c & 0x80;
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
			const std::size_t remaining = std::min( end - p, buffer_end - q );
			
			const char* it = std::find_if( p,
			                               p + remaining,
			                               std::ptr_fun( is_non_ascii ) );
			
			std::copy( p, it, q );
			
			q += it - p;
			
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
			
			chars::put_code_point_into_utf8( uc, n_bytes, q );
			
			q += n_bytes;
			
			++p;
		}
		
		return q - buffer_out;
	}
	
	std::size_t mac_from_utf8( char*         buffer_out,
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
			const std::size_t remaining = std::min( end - p, buffer_end - q );
			
			const char* it = std::find_if( p,
			                               p + remaining,
			                               std::ptr_fun( is_non_ascii ) );
			
			std::copy( p, it, q );
			
			q += it - p;
			
			p = it;
			
			if ( p == end )
			{
				break;
			}
			
			const unichar_t uc = chars::get_next_code_point_from_utf8( p, end );
			
			if ( !~uc )
			{
				throw utf8_decoding_error();
			}
			
			if ( const char c = MacRoman_from_unicode( uc ) )
			{
				*q++ = c;
			}
			else
			{
				throw chars::unrepresentable_character();
			}
		}
		
		return q - buffer_out;
	}
	
}

