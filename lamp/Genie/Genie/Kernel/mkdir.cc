/*	========
 *	mkdir.cc
 *	========
 */

// Standard C
#include <errno.h>

// POSIX
#include "sys/stat.h"

// Nitrogen / Carbon
#include "Nitrogen/Files.h"
#include "Nitrogen/OSStatus.h"

// Genie
#include "Genie/pathnames.hh"
#include "Genie/Process.hh"
#include "Genie/Yield.hh"


namespace N = Nitrogen;

using Genie::CurrentProcess;
using Genie::ResolveUnixPathname;


#pragma export on
	
	int	mkdir( const char* pathname, mode_t /*mode*/ )
	{
		try
		{
			N::FSDirSpec current = N::Convert< N::FSDirSpec >( CurrentProcess().CurrentDirectory() );
			
			FSSpec spec = ResolveUnixPathname( pathname, current );
			
			N::FSpDirCreate( spec );
		}
		catch ( ... )
		{
		}
		
		return CurrentProcess().SetErrno( EINVAL );
	}
	
#pragma export reset

