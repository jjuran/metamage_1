/*
	file-tests.cc
	-------------
*/

#include "vfs/functions/file-tests.hh"

// POSIX
#include <sys/stat.h>

// vfs
#include "vfs/node.hh"


namespace vfs
{
	
	bool exists( const node* file )
	{
		const mode_t mode = file->filemode();
		
		return mode != 0;
	}
	
	bool is_file( const node* file )
	{
		const mode_t mode = file->filemode();
		
		// A file as used here is any existing non-directory.
		return mode != 0  &&  !S_ISDIR( mode );
	}
	
	bool is_directory( const node* file )
	{
		const mode_t mode = file->filemode();
		
		return S_ISDIR( mode );
	}
	
	bool is_symlink( const node* file )
	{
		const mode_t mode = file->filemode();
		
		return S_ISLNK( mode );
	}
	
	bool is_fifo( const node* file )
	{
		const mode_t mode = file->filemode();
		
		return S_ISFIFO( mode );
	}
	
}

