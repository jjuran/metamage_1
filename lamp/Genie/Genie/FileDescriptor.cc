/*	=================
 *	FileDescriptor.cc
 *	=================
 */

#include "Genie/FileDescriptor.hh"

// iota
#include "iota/swap.hh"


namespace Genie
{
	
	void FileDescriptor::swap( FileDescriptor& other )
	{
		using iota::swap;
		
		swap( handle,      other.handle      );
		swap( closeOnExec, other.closeOnExec );
	}
	
}

