/*
	stat.cc
	-------
*/

#include "Genie/FS/stat.hh"

// POSIX
#include <time.h>
#include <sys/stat.h>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/primitives/inode.hh"

// Genie
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	void stat( const FSTree* it, struct ::stat& sb )
	{
		const node_method_set* methods = it->methods();
		
		if ( methods  &&  methods->stat )
		{
			methods->stat( it, sb );
			
			return;
		}
		
		if ( it->filemode() == 0 )
		{
			p7::throw_errno( ENOENT );
		}
		
		const time_t now = time( NULL );
		
		sb.st_ino = inode( it );
		
		sb.st_mode = it->filemode();
		
		sb.st_nlink = 1;
		
		// fstatat() or fstat() will fill in sb.st_size
		
		sb.st_blksize = 4096;
		
		sb.st_atime = now;
		sb.st_mtime = now;
		sb.st_ctime = now;
	}
	
}

