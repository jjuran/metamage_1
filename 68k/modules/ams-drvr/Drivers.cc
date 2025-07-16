/*
	Drivers.cc
	----------
*/

#include "Drivers.hh"

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// ams-common
#include "callouts.hh"


enum
{
	kHFSFlagMask = 0x0200,
	kAsyncIOMask = 0x0400,
};

Open_ProcPtr old_Open;

short Open_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	OSErr err = old_Open( trap_word, (FileParam*) pb );
	
	if ( err != fnfErr )
	{
		// call succeeded, or failed somehow other than file-not-found
	}
	else if ( trap_word & (kHFSFlagMask | kAsyncIOMask) )
	{
		// either HOpen, or asynchronous (or both), so not OpenDriver
	}
	else if ( pb->ioNamePtr  &&  *pb->ioNamePtr  &&  pb->ioNamePtr[ 1 ] == '.' )
	{
		/*
			If we get here, the Open call failed with fnfErr, isn't obviously
			not an OpenDriver() call, and requested a name beginning with '.',
			which probably indicates a driver.  Try to find it.
		*/
		
		if ( Handle h = GetNamedResource( 'DRVR', pb->ioNamePtr ) )
		{
			ResType type;
			short id;
			Str255 name;
			
			GetResInfo( h, &id, &type, name );
			
			HLock( h );
			
			err = DriverInstallReserveMem( (DRVRHeader*) *h, ~id );
			
			if ( err == noErr )
			{
				err = old_Open( trap_word, (FileParam*) pb );
			}
		}
	}
	
	return pb->ioResult = err;
}
