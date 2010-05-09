/*	====================
 *	GetStringResource.cc
 *	====================
 */

#include "Genie/Utilities/GetStringResource.hh"

// Standard C/++
#include <algorithm>

// plus
#include "plus/var_string.hh"

// Nitrogen
#include "Nitrogen/Resources.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	plus::string GetStringResource( ::ResID id )
	{
		::Handle h = N::GetResource( N::ResType( 'STR ' ), N::ResID( id ) );
		
		std::size_t length = **h;
		
		const char* begin = *h + 1;
		
		plus::var_string result;
		
		result.resize( length );
		
		std::copy( begin, begin + length, result.begin() );
		
		return result;
	}
	
}

