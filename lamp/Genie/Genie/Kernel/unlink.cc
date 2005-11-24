/*	=========
 *	unlink.cc
 *	=========
 */

// Standard C
#include <errno.h>

// POSIX
#include "unistd.h"

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
	
	int unlink( const char* pathname )
	{
		try
		{
			const N::FSDirSpec cwd = CurrentProcess().CurrentDirectory();
			
			FSSpec file = ResolveUnixPathname( pathname, cwd );
			
			CInfoPBRec paramBlock;
			
			N::FSpGetCatInfo( file, paramBlock );
			
			bool isDir = N::PBTestIsDirectory( paramBlock );
			
			if ( isDir )
			{
				return CurrentProcess().SetErrno( EISDIR );
			}
			
			N::FSpDelete( file );
		}
		catch ( N::FNFErr& )
		{
			return CurrentProcess().SetErrno( ENOENT );
		}
		catch ( ... )
		{
			return CurrentProcess().SetErrno( EINVAL );
		}
		
		return 0;
	}
	
#pragma export reset

