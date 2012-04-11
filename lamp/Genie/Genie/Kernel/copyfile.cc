/*	===========
 *	copyfile.cc
 *	===========
 */

// Relix
#include "relix/copyfile.h"

// Genie
#include "Genie/current_process.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/ResolvePathAt.hh"
#include "Genie/FS/copyfile.hh"
#include "Genie/SystemCallRegistry.hh"


int copyfileat( int olddirfd, const char* oldpath, int newdirfd, const char* newpath, unsigned flags )
{
	using namespace Genie;
	
	try
	{
		FSTreePtr srcFile  = ResolvePathAt( olddirfd, oldpath );
		FSTreePtr destFile = ResolvePathAt( newdirfd, newpath );
		
		// Do not resolve links
		
		copyfile( srcFile.get(), destFile.get() );
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

