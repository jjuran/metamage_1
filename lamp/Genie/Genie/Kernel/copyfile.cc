/*	===========
 *	copyfile.cc
 *	===========
 */

// vfs
#include "vfs/node.hh"
#include "vfs/primitives/copyfile.hh"

// Relix
#include "relix/copyfile.h"

// Genie
#include "Genie/current_process.hh"
#include "Genie/FS/ResolvePathAt.hh"
#include "Genie/SystemCallRegistry.hh"


int copyfileat( int olddirfd, const char* oldpath, int newdirfd, const char* newpath, unsigned flags )
{
	using namespace Genie;
	
	try
	{
		FSTreePtr srcFile  = ResolvePathAt( olddirfd, oldpath );
		FSTreePtr destFile = ResolvePathAt( newdirfd, newpath );
		
		// Do not resolve links
		
		copyfile( *srcFile, *destFile );
	}
	catch ( ... )
	{
		return set_errno_from_exception();
	}
	
	return 0;
}

#pragma force_active on

REGISTER_SYSTEM_CALL( copyfileat );

#pragma force_active reset

