/*	=======
 *	Base.hh
 *	=======
 */

#ifndef GENIE_IO_BASE_HH
#define GENIE_IO_BASE_HH

// vfs
#include "vfs/filehandle.hh"
#include "vfs/memory_mapping.hh"
#include "vfs/enum/poll_result.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"
#include "Genie/IO/IOPtr.hh"


namespace Genie
{
	
	enum
	{
		kPollRead   = vfs::Poll_read,
		kPollWrite  = vfs::Poll_write,
		kPollExcept = vfs::Poll_except
	};
	
	using vfs::memory_mapping_ptr;
	
	typedef vfs::filehandle IOHandle;
	
}

#endif

