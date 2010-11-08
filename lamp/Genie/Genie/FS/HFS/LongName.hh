/*
	Genie/FS/HFS/LongName.hh
	------------------------
*/

#ifndef GENIE_FS_HFS_LONGNAME_HH
#define GENIE_FS_HFS_LONGNAME_HH

//#include <Files.h>
struct FSSpec;


namespace plus
{
	
	class string;
	
}

namespace Genie
{
	
	void SetLongName( const FSSpec& item, const plus::string& name );
	
}

#endif

