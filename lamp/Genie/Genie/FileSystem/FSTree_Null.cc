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
	
	namespace p7 = poseven;
	
	FSTreePtr FSTree_Null::Parent() const
	{
		p7::throw_errno( ENOENT );
		
		throw;
	}
	
}

