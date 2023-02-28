/*
	Genie/FS/HFS/LongName.cc
	------------------------
*/

#include "Genie/FS/HFS/LongName.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// mac-file-utils
#include "mac_file/desktop.hh"

// plus
#include "plus/string.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


namespace Genie
{
	
	void SetLongName( const FSSpec& item, const plus::string& name )
	{
		using mac::file::set_desktop_comment;
		
		if ( name.length() > 31 )
		{
			OSErr err = set_desktop_comment( item, name.data(), name.size() );
			
			if ( err != paramErr )
			{
				Mac::ThrowOSStatus( err );
			}
		}
	}
	
}
