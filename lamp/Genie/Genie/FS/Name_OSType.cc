/*	==============
 *	Name_OSType.cc
 *	==============
 */

#include "Genie/FS/Name_OSType.hh"

// BitsAndBytes
#include "HexCodes.hh"
#include "HexStrings.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace Bits = BitsAndBytes;
	
	
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
				escape[ 1 ] = Bits::NibbleAsHex( Bits::HighNibble( c ) );
				escape[ 2 ] = Bits::NibbleAsHex( Bits::LowNibble ( c ) );
				
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
			
			char c = Bits::DecodeHexNibbles( escape[1], escape[2] );
			
			decoded += c;
		}
		
		if ( decoded.length() < sizeof (Key)  &&  decoded.length() >= 2 )
		{
			decoded.resize( sizeof (Key), ' ' );
		}
		
		return NN::Convert< N::OSType >( decoded );
	}
	
}

