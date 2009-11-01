/*	==============
 *	Name_OSType.cc
 *	==============
 */

#include "Genie/FS/Name_OSType.hh"

// iota
#include "iota/hexidecimal.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	std::string OSType_KeyName_Traits::NameFromKey( const Key& key )
	{
		std::string bytes = NN::Convert< std::string >( key );
		
		std::string result;
		
		result.reserve( sizeof (Key) );  // the usual case
		
		char escape[] = "%00";
		
		for ( int i = 0;  i < sizeof (Key);  ++i )
		{
			unsigned char c = bytes[ i ];
			
			// We escape
			// * NUL because it can't be used in POSIX filenames
			// * other control characters because they're non-printable
			// * percent because it's the escape char
			// * slash because it's the path separator
			
			if ( c < ' '  ||  c == '%'  ||  c == '/' )
			{
				escape[ 1 ] = iota::encoded_hex_char( c >> 4 );
				escape[ 2 ] = iota::encoded_hex_char( c      );
				
				result.append( escape, sizeof escape - 1 );
			}
			else
			{
				result += c;
			}
		}
		
		return result;
	}
	
	OSType_KeyName_Traits::Key OSType_KeyName_Traits::KeyFromName( const std::string& name )
	{
		std::string decoded;
		
		decoded.reserve( sizeof (Key) );
		
		const char* end = name.data() + name.size();
		
		for ( const char* p = name.data();; )
		{
			const char* escape = std::find( p, end, '%' );
			
			decoded.append( p, escape );
			
			if ( escape == end )
			{
				break;
			}
			
			p = escape + 3;
			
			if ( p > end )
			{
				break;  // conversion below will fail and throw exception
			}
			
			const char c = iota::decoded_hex_digit( escape[1] ) << 4
			             | iota::decoded_hex_digit( escape[2] );
			
			decoded += c;
		}
		
		if ( decoded.length() < sizeof (Key)  &&  decoded.length() >= 2 )
		{
			decoded.resize( sizeof (Key), ' ' );
		}
		
		return NN::Convert< N::OSType >( decoded );
	}
	
}

