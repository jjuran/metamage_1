/*	==============
 *	Name_OSType.hh
 *	==============
 */

#ifndef GENIE_FILESYSTEM_NAMEOSTYPE_HH
#define GENIE_FILESYSTEM_NAMEOSTYPE_HH

// Standard C++
#include <string>

// Nitrogen
#include "Nitrogen/MacTypes.hh"


namespace Genie
{
	
	struct OSType_KeyName_Traits
	{
		typedef Nitrogen::OSType Key;
		
		static std::string NameFromKey( const Key& key );
		
		static Key KeyFromName( const std::string& name );
	};
	
}

#endif

