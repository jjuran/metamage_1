/*
	Genie/FS/HFS/LongName.hh
	------------------------
*/

#ifndef GENIE_FS_HFS_LONGNAME_HH
#define GENIE_FS_HFS_LONGNAME_HH

// plus
#include "plus/string_fwd.hh"

//#include <Files.h>
struct FSSpec;


namespace Genie
{
	
	void SetLongName( const FSSpec& item, const plus::string& name );
	
}

#endif

