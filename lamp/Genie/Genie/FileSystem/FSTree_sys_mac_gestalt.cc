/*	=========================
 *	FSTree_sys_mac_gestalt.cc
 *	=========================
 */

#include "Genie/FileSystem/FSTree_sys_mac_gestalt.hh"

// Nitrogen
#include "Nitrogen/Gestalt.h"
#include "Nitrogen/OSStatus.h"

// BitsAndBytes
#include "HexCodes.hh"
#include "HexStrings.hh"


namespace Nitrogen
{
	
	GestaltTable_Container GestaltTable()
	{
		UInt32 tabl = Nitrogen::Gestalt( Nitrogen::GestaltSelector( 'tabl' ) );
		
		Gestalt_Handle h = reinterpret_cast< Gestalt_Handle >( tabl );
		
		if ( h == NULL  ||  *h == NULL )
		{
			ThrowOSStatus( nilHandleErr );
		}
		
		return GestaltTable_Container( h );
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace Bits = BitsAndBytes;
	
	
	std::string OSType_KeyName_Traits::NameFromKey( const Key& key )
	{
		std::string bytes = Nucleus::Convert< std::string >( key );
		
		std::string result;
		
		result.reserve( sizeof (Key) );  // the usual case
		
		try
		{
			N::Gestalt( N::GestaltSelector( key ) );
		}
		catch ( ... )
		{
			result = '.';
		}
		
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
		
		return Nucleus::Convert< Nitrogen::OSType >( decoded );
	}
	
	
	bool sys_mac_gestalt_Details::KeyIsValid( const Key& key )
	{
		try
		{
			N::Gestalt( N::GestaltSelector( key ) );
		}
		catch ( ... )
		{
			return false;
		}
		
		return true;
	}
	
	FSTreePtr sys_mac_gestalt_Details::GetChildNode( const FSTreePtr&    parent,
		                                             const std::string&  name,
		                                             const Key&          key )
	{
		return FSTreePtr( new FSTree_Functional< Gestalt_KeyName_Traits::Key >( parent, name, key ) );
	}
	
}

