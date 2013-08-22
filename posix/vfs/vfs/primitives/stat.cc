/*
	stat.cc
	-------
*/

#include "vfs/primitives/stat.hh"

// POSIX
#include <time.h>
#include <sys/stat.h>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/primitives/inode.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void stat( const node& that, struct ::stat& sb )
	{
		const node_method_set* methods = that.methods();
		
		if ( methods  &&  methods->stat )
		{
			methods->stat( &that, sb );
			
			return;
		}
		
		if ( that.filemode() == 0 )
		{
			p7::throw_errno( ENOENT );
		}
		
		const time_t now = time( NULL );
		
		sb.st_ino = inode( that );
		
		sb.st_mode = that.filemode();
		
		sb.st_nlink = 1;
		
		// fstatat() or fstat() will fill in sb.st_size
		
		sb.st_blksize = 4096;
		
		sb.st_atime = now;
		sb.st_mtime = now;
		sb.st_ctime = now;
	}
	
}

