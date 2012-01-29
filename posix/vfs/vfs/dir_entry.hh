/*
	dir_entry.hh
	------------
*/

#ifndef VFS_DIRENTRY_HH
#define VFS_DIRENTRY_HH

// POSIX
#include <sys/types.h>

// plus
#include "plus/string.hh"


namespace vfs
{
	
	struct dir_entry
	{
		ino_t         inode;
		plus::string  name;
		
		dir_entry()
		{
		}
		
		dir_entry( ino_t i, const plus::string& n ) : inode( i ), name( n )
		{
		}
		
		~dir_entry();
	};
	
}

#endif

