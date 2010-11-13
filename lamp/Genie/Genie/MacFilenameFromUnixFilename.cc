/*	==============================
 *	MacFilenameFromUnixFilename.cc
 *	==============================
 */

#include "Genie/MacFilenameFromUnixFilename.hh"

// Standard C++
#include <algorithm>

// Debug
#include "debug/assert.hh"

// plus
#include "plus/var_string.hh"

// Arcana / MD5
#include "MD5/MD5.hh"


namespace Kerosene
{
	
	static char base32_encode( unsigned x )
	{
		return ( x < 10 ? '0' : 'a' - 10 ) + x;
	}
	
	plus::string MacFilenameFromUnixFilename( const plus::string& unixName )
	{
		const std::size_t max_length = 31;
		
		const std::size_t long_length = unixName.length();
		
		plus::var_string macName;
		
		if ( long_length <= max_length )
		{
			macName = unixName;
		}
		else
		{
			const char* begin = unixName.data();
			const char* end   = begin + long_length;
			
			std::size_t dot = unixName.find_last_of( "." );
			
			const bool has_dot = dot != unixName.npos;
			
			const unsigned minimum_remaining_base = 8;
			
			const unsigned n_delimiters = 1;
			
			const unsigned hash_length = 6;
			
			const unsigned inserted_length = n_delimiters + hash_length;  // 7
			
			const unsigned extension_limit =   max_length              // 31
			                                 - minimum_remaining_base  // -8
			                                 - inserted_length;        // -7
			                                                           // == 16
			
			if ( !has_dot  ||  long_length - dot > extension_limit )
			{
				// Set the end of the base to end of string if there's no dot,
				// or if the extension (including the dot) exceeds the limit.
				dot = long_length;
			}
			
			const std::size_t base_length = dot;
			
			const std::size_t replaced_length = long_length - max_length + inserted_length;
			
			const std::size_t shortened_base_length = base_length - replaced_length;
			
			ASSERT( shortened_base_length >= minimum_remaining_base );
			
			MD5::Result hash = MD5::Digest_Bytes( begin, long_length );
			
			macName.assign( begin, shortened_base_length );
			
			macName += 0xA5;  // bullet
			
			for ( int i = 0;  i != hash_length;  ++i )
			{
				macName += base32_encode( hash.data[ i ] >> 3 );
			}
			
			if ( has_dot )
			{
				macName.append( begin + dot, end );
			}
		}
		
		std::replace( macName.begin(), macName.end(), ':', '/' );
		
		return macName;
	}
	
}

