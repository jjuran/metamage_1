/*
	command.cc
	----------
*/

#include "mac_adb/command.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#if ! TARGET_API_MAC_CARBON
#ifndef __DESKBUS__
#include <DeskBus.h>
#endif
#endif

// mac-config
#if ! TARGET_API_MAC_CARBON
#include "mac_config/upp-macros.hh"
#endif


#pragma exceptions off


namespace mac {
namespace adb {

#if ! TARGET_API_MAC_CARBON

#if TARGET_CPU_68K

static
asm pascal void ADBOp_completion()
{
	// buffer  in A0
	// refCon  in A2 (we always pass a non-NULL pointer)
	// command in D0 (byte, assured to be non-zero)
	
	MOVE.B   D0,(A2)  // The command is a byte, assured to be non-zero
	RTS
}

#else

static
pascal void ADBOp_completion( Ptr buffer, Ptr refCon, long command )
{
	*refCon = true;
}

#endif  // #if TARGET_CPU_68K

OSErr send_command( Ptr buffer, short command )
{
	DEFINE_UPP( ADBCompletion, ADBOp_completion )
	
	char refCon = false;
	
	OSErr err = ADBOp( &refCon, UPP_ARG( ADBOp_completion ), buffer, command );
	
	volatile const char& done = refCon;
	
	while ( ! done )
	{
		continue;
	}
	
	return err;
}

#else

int dummy;

#endif  // #if ! TARGET_API_MAC_CARBON

}
}
