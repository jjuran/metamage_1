/*	==============================
 *	MacFilenameFromUnixFilename.cc
 *	==============================
 */

#include "MacFilenameFromUnixFilename.hh"

// Standard C++
#include <algorithm>

// POSeven
#include "POSeven/Errno.hh"

// Arcana / MD5
#include "MD5.hh"


namespace Kerosene
{
	
	namespace p7 = poseven;
	
	
	static char base32_encode( unsigned x )
	{
		return ( x < 10 ? '0' : 'a' - 10 ) + x;
	}
	
	std::string MacFilenameFromUnixFilename( const std::string& unixName )
	{
		const std::size_t max_length = 31;
		
		std::string macName;
		
		if ( unixName.length() <= max_length )
		{
			macName = unixName;
		}
		else
		{
			std::size_t dot = unixName.find_last_of( "." );
			
			const bool has_dot = dot != unixName.npos;
			
			const unsigned n_delimiters = 1;
			
			const unsigned hash_length = 6;
			
			std::size_t base_length = has_dot ? dot : unixName.size();
			
			std::size_t replaced_length = unixName.size() - max_length + n_delimiters + hash_length;
			
			if ( replaced_length >= base_length )
			{
				p7::throw_errno( ENAMETOOLONG );  // extension is too long
			}
			
			std::size_t shortened_base_length = base_length - replaced_length;
			
			MD5::Result hash = MD5::Digest( unixName.data(), unixName.size() );
			
			macName.assign( unixName.begin(), unixName.begin() + shortened_base_length );
			
			macName += '¥';
			
			for ( int i = 0;  i != hash_length;  ++i )
			{
				macName += base32_encode( hash.data[ i ] >> 3 );
			}
			
			if ( has_dot )
			{
				macName.append( unixName.begin() + dot, unixName.end() );
			}
		}
		
		std::replace( macName.begin(), macName.end(), ':', '/' );
		
		return macName;
	}
	
}

