/*
	conv/utf8_16.hh
	---------------
*/

#ifndef CONV_UTF816_HH
#define CONV_UTF816_HH

// Standard C/c++
#include <cstddef>


namespace conv
{
	
	typedef unsigned short utf16_t;
	
	const std::size_t invalid_utf16 = std::size_t( -3 );
	
	class utf16_decoding_error {};
	
	std::size_t sizeof_utf8_from_utf16( const utf16_t*  begin,
	                                    const utf16_t*  end );
	
	std::size_t utf8_from_utf16_nothrow( char*            buffer_out,
	                                     std::size_t      length,
	                                     const utf16_t**  pp_in,
	                                     std::size_t      n );
	
	inline
	std::size_t utf8_from_utf16_nothrow( char*           buffer_out,
	                                     std::size_t     length,
	                                     const utf16_t*  begin,
	                                     std::size_t     n )
	{
		return utf8_from_utf16_nothrow( buffer_out, length, &begin, n );
	}
	
	std::size_t utf8_from_utf16( char*            buffer,
	                             std::size_t      length,
	                             const utf16_t**  pp_in,
	                             std::size_t      n );
	
	inline
	std::size_t utf8_from_utf16( char*           buffer_out,
	                             std::size_t     length,
	                             const utf16_t*  begin,
	                             std::size_t     n )
	{
		return utf8_from_utf16( buffer_out, length, &begin, n );
	}
	
}

#endif
