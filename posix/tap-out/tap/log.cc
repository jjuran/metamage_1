/*
	log.cc
	------
*/

#include "tap/log.hh"

// Standard C
#include <stdlib.h>

// POSIX
#include <unistd.h>

// more-libc
#include "more/string.h"

// iota
#include "iota/strings.hh"

// gear
#include "gear/hexadecimal.hh"


#pragma exceptions off


namespace tap
{
	
	static inline size_t max( size_t a, size_t b )
	{
		return a > b ? a : b;
	}
	
	static void log_mem( char* buffer, const char* prefix, size_t p_len, const char* mem, size_t len )
	{
		char* q = buffer;
		
		q = (char*) mempcpy( q, prefix, p_len );
		
		const char* end = mem + len;
		const char* p   = mem;
		
		while ( p < end - 1 )
		{
			gear::encode_8_bit_HEX( *p++, q );  q += 2;
			gear::encode_8_bit_HEX( *p++, q );  q += 2;
			
			*q++ = ' ';
		}
		
		if ( p < end )
		{
			gear::encode_8_bit_HEX( *p++, q );  q += 3;
		}
		
		q[ -1 ] = '\n';
		
		write( STDERR_FILENO, buffer, q - buffer );
	}
	
	void log( const void* p, unsigned p_len, const void* q, unsigned q_len )
	{
		const size_t buffer_len = max( p_len, q_len );
		
		char* buffer = (char*) alloca( STRLEN( "EXPECTED: " ) + buffer_len / 2 * 5 );
	
		log_mem( buffer, STR_LEN( "EXPECTED: " ), (const char*) q, q_len );
		log_mem( buffer, STR_LEN( "RECEIVED: " ), (const char*) p, p_len );
	}
	
	static void log_message( const char* msg, size_t len, const iota::span& s )
	{
		const size_t buffer_size = len + s.len + 1;
		
		char* buffer = (char*) alloca( buffer_size );
		
		char* mid = (char*) mempcpy( buffer, msg, len );
		char* end = (char*) mempcpy( mid, s.ptr, s.len );
		
		*end++ = '\n';
		
		write( STDERR_FILENO, buffer, buffer_size );
	}
	
	void log_expected( const iota::span& s )
	{
		log_message( STR_LEN( "EXPECTED: " ), s );
	}
	
	void log_received( const iota::span& s )
	{
		log_message( STR_LEN( "RECEIVED: " ), s );
	}
	
}
