/*	==============
 *	FSTree_Null.cc
 *	==============
 */

#include "Genie/FileSystem/FSTree_Null.hh"

// Standard C/C++
#include <cerrno>

// POSeven
#include "POSeven/Errno.hh"


namespace Genie
{
	
	namespace P7 = POSeven;
	
	FSTreePtr FSTree_Null::Parent() const
	{
		P7::ThrowErrno( ENOENT );
		
		throw;
	}
	
}

