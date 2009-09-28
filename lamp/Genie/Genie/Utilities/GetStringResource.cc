/*	====================
 *	GetStringResource.cc
 *	====================
 */

#include "Genie/Utilities/GetStringResource.hh"

// Standard C/++
#include <algorithm>

// Nitrogen
#include "Nitrogen/Resources.h"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	std::string GetStringResource( ::ResID id )
	{
		::Handle h = N::GetResource( N::ResType( 'STR ' ), N::ResID( id ) );
		
		std::size_t length = **h;
		
		const char* begin = *h + 1;
		
		std::string result;
		
		result.resize( length );
		
		std::copy( begin, begin + length, result.begin() );
		
		return result;
	}
	
}

