/*	==============================
 *	MacFilenameFromUnixFilename.cc
 *	==============================
 */

#include "Genie/MacFilenameFromUnixFilename.hh"

// Standard C++
#include <algorithm>

// plus
#include "plus/var_string.hh"

// Genie
#include "Genie/FS/HFS/hashed_long_name.hh"


namespace Kerosene
{
	
	plus::string MacFilenameFromUnixFilename( const plus::string& unixName )
	{
		plus::var_string macName = Genie::hashed_long_name( unixName );
		
		std::replace( macName.begin(), macName.end(), ':', '/' );
		
		return macName;
	}
	
}

