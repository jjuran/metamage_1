/*
	hashed_long_name.cc
	-------------------
*/

#include "Genie/FS/HFS/hashed_long_name.hh"

// more-libc
#include "more/string.h"

// gear
#include "gear/find.hh"

// crypto
#include "md5/md5.hh"

// Debug
#include "debug/assert.hh"


#pragma exceptions off


namespace Genie
{

using crypto::md5;
using crypto::md5_digest;


static char base32_encode( unsigned x )
{
	return ( x < 10 ? '0' : 'a' - 10 ) + x;
}

void hash_long_name( unsigned char* hashed, const char* long_name, size_t long_length )
{
	const size_t max_length = 31;
	
	uint8_t* p = hashed;
	
	if ( long_length <= max_length )
	{
		*p++ = long_length;
		
		mempcpy( p, long_name, long_length );
		
		return;
	}
	
	const char* begin = long_name;
	const char* end   = begin + long_length;
	
	const char* dot = gear::find_last_match( begin, end, '.' );
	
	const unsigned minimum_remaining_base = 8;
	
	const unsigned n_delimiters = 1;
	
	const unsigned hash_length = 6;
	
	const unsigned inserted_length = n_delimiters + hash_length;  // 7
	
	const unsigned extension_limit =   max_length              // 31
	                                 - minimum_remaining_base  // -8
	                                 - inserted_length;        // -7
	                                                           // == 16
	
	const bool dotted = dot != NULL  &&  end - dot <= extension_limit;
	
	// Set the end of the base to end of string if there's no dot,
	// or if the extension (including the dot) exceeds the limit.
	const char* base_end = dotted ? dot : end;
	
	const size_t base_length = base_end - begin;
	
	const size_t replaced_length = long_length - max_length + inserted_length;
	
	const size_t shortened_base_length = base_length - replaced_length;
	
	ASSERT( shortened_base_length >= minimum_remaining_base );
	
	md5_digest hash = md5( begin, long_length );
	
	const unsigned char* data = (const unsigned char*) &hash;
	
	*p++ = 31;
	
	p = (uint8_t*) mempcpy( p, begin, shortened_base_length );
	
	*p++ = 0xA5;  // bullet
	
	for ( int i = 0;  i != hash_length;  ++i )
	{
		*p++ = base32_encode( data[ i ] >> 3 );
	}
	
	p = (uint8_t*) mempcpy( p, base_end, end - base_end );
}

}
