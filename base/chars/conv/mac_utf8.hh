/*
	conv/mac_utf8.hh
	----------------
*/

#ifndef CONV_MACUTF8_HH
#define CONV_MACUTF8_HH

// Standard C/c++
#include <cstddef>


namespace conv
{
	
	const std::size_t invalid_utf8 = std::size_t( -1 );
	const std::size_t non_Mac_utf8 = std::size_t( -2 );
	
	class utf8_decoding_error {};
	
	std::size_t sizeof_utf8_from_mac( const char* begin, const char* end );
	std::size_t sizeof_mac_from_utf8( const char* begin, const char* end );
	
	std::size_t utf8_from_mac( char*         buffer_out,
	                           std::size_t   length,
	                           const char**  pp_in,
	                           std::size_t   n );
	
	inline std::size_t utf8_from_mac( char*        buffer_out,
	                                  std::size_t  length,
	                                  const char*  begin,
	                                  std::size_t  n )
	{
		return utf8_from_mac( buffer_out, length, &begin, n );
	}
	
	std::size_t mac_from_utf8( char*         buffer_out,
	                           std::size_t   length,
	                           const char**  pp_in,
	                           std::size_t   n );
	
	inline std::size_t mac_from_utf8( char*        buffer_out,
	                                  std::size_t  length,
	                                  const char*  begin,
	                                  std::size_t  n )
	{
		return mac_from_utf8( buffer_out, length, &begin, n );
	}
	
	std::size_t mac_from_utf8_nothrow( char*         buffer_out,
	                                   std::size_t   length,
	                                   const char**  pp_in,
	                                   std::size_t   n );
	
	inline
	std::size_t mac_from_utf8_nothrow( char*        buffer_out,
	                                   std::size_t  length,
	                                   const char*  begin,
	                                   std::size_t  n )
	{
		return mac_from_utf8_nothrow( buffer_out, length, &begin, n );
	}
	
}

#endif
