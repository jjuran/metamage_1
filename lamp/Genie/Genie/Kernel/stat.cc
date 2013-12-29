/*	=======
 *	stat.cc
 *	=======
 */

// Standard C/C++
#include <cstring>

// POSIX
//#include "stdlib.h"
#include "sys/stat.h"

// vfs
#include "vfs/node.hh"
#include "vfs/filehandle/primitives/geteof.hh"
#include "vfs/primitives/geteof.hh"
#include "vfs/primitives/stat.hh"

// relix-kernel
#include "relix/api/get_fd_handle.hh"
#include "relix/syscall/faccessat.hh"
#include "relix/syscall/fchmod.hh"
#include "relix/syscall/fchmodat.hh"
#include "relix/syscall/fstatat.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/IO/RegularFile.hh"
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	using relix::faccessat;
	using relix::fchmod;
	using relix::fchmodat;
	using relix::fstatat;
	
	
	static int fstat( int fd, struct stat* sb )
	{
		std::memset( (void*) sb, '\0', sizeof (struct stat) );
		
		sb->st_size = off_t( -1 );
		
		try
		{
			vfs::filehandle& handle = relix::get_fd_handle( fd );
			
			stat( *handle.GetFile(), *sb );
			
			if ( sb->st_size == off_t( -1 ) )
			{
				try
				{
					sb->st_size = geteof( handle );
				}
				catch ( ... )
				{
					sb->st_size = 0;
				}
			}
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( faccessat );
	REGISTER_SYSTEM_CALL( fchmodat  );
	REGISTER_SYSTEM_CALL( fchmod    );
	REGISTER_SYSTEM_CALL( fstatat   );
	REGISTER_SYSTEM_CALL( fstat     );
	
	#pragma force_active reset
	
}

