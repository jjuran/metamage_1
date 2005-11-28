/*	========
 *	rmdir.cc
 *	========
 */

// Standard C
#include <errno.h>

// POSIX
#include "sys/stat.h"

// Nitrogen / Carbon
#include "Nitrogen/OSStatus.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Genie
#include "Genie/pathnames.hh"
#include "Genie/Process.hh"
#include "Genie/Yield.hh"


namespace N = Nitrogen;

using Genie::CurrentProcess;
using Genie::ResolveUnixPathname;


#pragma export on
	
	int	rmdir( const char* pathname )
	{
		try
		{
			N::FSDirSpec current = N::Convert< N::FSDirSpec >( CurrentProcess().CurrentDirectory() );
			
			FSSpec dir = ResolveUnixPathname( pathname, current );
			
			CInfoPBRec paramBlock;
			
			N::FSpGetCatInfo( dir, paramBlock );
			
			bool isDir = N::PBTestIsDirectory( paramBlock );
			
			if ( !isDir )
			{
				return CurrentProcess().SetErrno( ENOTDIR );
			}
			
			N::FSpDelete( dir );
		}
		catch ( ... )
		{
		}
		
		return CurrentProcess().SetErrno( EINVAL );
	}
	
#pragma export reset

