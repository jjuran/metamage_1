/*	====================
 *	GetStringResource.cc
 *	====================
 */

#include "Genie/Utilities/GetStringResource.hh"

// Standard C
#include <string.h>

// Nitrogen
#include "Nitrogen/Resources.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	plus::string GetStringResource( ::ResID id )
	{
		::Handle h = N::GetResource( N::ResType( 'STR ' ), N::ResID( id ) );
		
		std::size_t length = **h;
		
		plus::string result;
		
		char* p = result.reset( length );
		
		const char* begin = *h + 1;
		
		memcpy( p, begin, length );
		
		return result;
	}
	
}

