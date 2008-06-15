/*	===========================
 *	FSTree_sys_mac_user_name.cc
 *	===========================
 */

#include "Genie/FileSystem/FSTree_sys_mac_user_name.hh"

// Nitrogen
#include "Nitrogen/Resources.h"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	std::string sys_mac_user_name_Query::operator()() const
	{
		::Handle h = N::GetResource( N::ResType( 'STR ' ), N::ResID( -16096 ) );
		
		std::size_t length = **h;
		
		const char* begin = *h + 1;
		
		std::string result;
		
		result.resize( length + 1 );
		
		std::copy( begin, begin + length, result.begin() );
		
		result.end()[ -1 ] = '\n';
		
		return result;
	}
	
}

