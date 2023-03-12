/*
	desktop.hh
	----------
*/

#ifndef MACFILE_DESKTOP_HH
#define MACFILE_DESKTOP_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif


namespace mac  {
namespace file {
	
	OSErr get_desktop_APPL( FSSpec&  result,
	                        short    vRefNum,
	                        OSType   signature,
	                        short    index );
	
	long get_desktop_comment( short            vRefNum,
	                          long             dirID,
	                          ConstStr63Param  name,
	                          char*            buffer,
	                          UInt32           length );
	
	OSErr set_desktop_comment( short            vRefNum,
	                           long             dirID,
	                           ConstStr63Param  name,
	                           const char*      buffer,
	                           UInt32           length );
	
#ifndef __LP64__
	
	inline
	long get_desktop_comment( const FSSpec& item, char* p, UInt32 n )
	{
		return get_desktop_comment( item.vRefNum, item.parID, item.name, p, n );
	}
	
	inline
	OSErr set_desktop_comment( const FSSpec& item, const char* p, UInt32 n )
	{
		return set_desktop_comment( item.vRefNum, item.parID, item.name, p, n );
	}
	
#else
	
	int dummy;
	
#endif
	
}
}

#endif
