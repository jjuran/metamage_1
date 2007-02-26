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
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/Process.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace P7 = POSeven;
	
	FSTreePtr TTYHandle::GetFile() const
	{
		return ResolvePathname( ttyName, FSRoot() );
	}
	
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

