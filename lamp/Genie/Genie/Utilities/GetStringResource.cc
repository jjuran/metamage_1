/*	====================
 *	GetStringResource.cc
 *	====================
 */

#include "Genie/Utilities/GetStringResource.hh"

// Standard C
#include <string.h>

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
		
		char* p = result.reset( length );
		
		memcpy( p, begin, length );
		
		return result;
	}
	
}

