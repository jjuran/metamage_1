/*	==============
 *	Name_OSType.hh
 *	==============
 */

#ifndef GENIE_FILESYSTEM_NAMEOSTYPE_HH
#define GENIE_FILESYSTEM_NAMEOSTYPE_HH

// plus
#include "plus/string.hh"

// Nitrogen
#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif


namespace Genie
{
	
	struct OSType_KeyName_Traits
	{
		typedef Nitrogen::OSType Key;
		
		static plus::string NameFromKey( const Key& key );
		
		static Key KeyFromName( const plus::string& name );
	};
	
}

#endif

