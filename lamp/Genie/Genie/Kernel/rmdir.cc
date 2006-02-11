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
#include "Genie/SystemCallRegistry.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	static int rmdir( const char* pathname )
	{
		try
		{
			N::FSDirSpec current = NN::Convert< N::FSDirSpec >( CurrentProcess().CurrentDirectory() );
			
			FSSpec dir = ResolveUnixPathname( pathname, current );
			
			CInfoPBRec paramBlock;
			
			N::FSpGetCatInfo( dir, paramBlock );
			
			bool isDir = N::PBTestIsDirectory( paramBlock );
			
			if ( !isDir )
			{
				return CurrentProcess().SetErrno( ENOTDIR );
			}
			
			N::FSpDelete( dir );
			
			return 0;
		}
		catch ( N::FNFErr& )
		{
			return CurrentProcess().SetErrno( ENOENT );
		}
		catch ( ... )
		{
		}
		
		return CurrentProcess().SetErrno( EINVAL );
	}
	
	REGISTER_SYSTEM_CALL( rmdir );
	
}

