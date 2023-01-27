/*
	open_rsrc_fork.cc
	-----------------
*/

#include "mac_file/open_rsrc_fork.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifdef __MACOS__
#ifndef __FILES__
#include <Files.h>
#endif
#endif


#pragma exceptions off


namespace mac  {
namespace file {

FSIORefNum open_rsrc_fork( const FSSpec& file, signed char perm )
{
	OSErr err;
	FSIORefNum result;
	
#if ! __LP64__
	
	if ( TARGET_CPU_68K )
	{
		err = HOpenRF( file.vRefNum, file.parID, file.name, perm, &result );
	}
	else
	{
		err = FSpOpenRF( &file, perm, &result );
	}
	
	return err ? err : result;
	
#endif
	
	return 0;
}

FSIORefNum open_rsrc_fork( const FSRef& file, signed char perm )
{
	OSErr err;
	FSIORefNum result;
	HFSUniStr255 forkName;
	
	err = FSGetResourceForkName( &forkName );
	
	err = FSOpenFork( &file, forkName.length, forkName.unicode, perm, &result );
	
	return err ? err : result;
}

}
}
