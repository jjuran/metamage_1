/*	==============================
 *	MacFilenameFromUnixFilename.hh
 *	==============================
 */

#ifndef KEROSENE_MACFILENAMEFROMUNIXFILENAME_HH
#define KEROSENE_MACFILENAMEFROMUNIXFILENAME_HH

// plus
#include "plus/string.hh"


namespace Genie
{
	
	plus::string hashed_long_name( const plus::string& long_name );
	
}

namespace Kerosene
{
	
	plus::string MacFilenameFromUnixFilename( const plus::string& unixName );
	
}

#endif

