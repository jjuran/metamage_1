/*	======
 *	TTY.cc
 *	======
 */

#include "Genie/IO/TTY.hh"

// POSIX
#include "sys/ttycom.h"

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/Process.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace P7 = POSeven;
	
	void TTYHandle::IOCtl( unsigned long request, int* argp )
	{
		switch ( request )
		{
			case TIOCSCTTY:
				CurrentProcess().SetControllingTerminal( this );
				break;
			
			default:
				FileHandle::IOCtl( request, argp );
				break;
		};
		
		P7::ThrowErrno( EINVAL );
	}
	
}

