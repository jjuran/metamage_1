/*	=======
 *	Base.hh
 *	=======
 */

#ifndef GENIE_IO_BASE_HH
#define GENIE_IO_BASE_HH

// vfs
#include "vfs/filehandle.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"
#include "Genie/IO/IOPtr.hh"
#include "Genie/mmap/memory_mapping.hh"


namespace Genie
{
	
	enum
	{
		kPollRead   = 1,
		kPollWrite  = 2,
		kPollExcept = 4
	};
	
	using vfs::memory_mapping_ptr;
	
	typedef vfs::filehandle IOHandle;
	
}

#endif

