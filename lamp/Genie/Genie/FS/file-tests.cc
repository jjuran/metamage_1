/*
	file-tests.cc
	-------------
*/

#include "Genie/FS/file-tests.hh"

// POSIX
#include <sys/stat.h>

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	bool exists( const FSTree* file )
	{
		const mode_t mode = file->FileMode();
		
		return mode != 0;
	}
	
	bool is_file( const FSTree* file )
	{
		const mode_t mode = file->FileMode();
		
		// A file as used here is any existing non-directory.
		return mode != 0  &&  !S_ISDIR( mode );
	}
	
	bool is_directory( const FSTree* file )
	{
		const mode_t mode = file->FileMode();
		
		return S_ISDIR( mode );
	}
	
	bool is_symlink( const FSTree* file )
	{
		const mode_t mode = file->FileMode();
		
		return S_ISLNK( mode );
	}
	
	bool is_fifo( const FSTree* file )
	{
		const mode_t mode = file->FileMode();
		
		return S_ISFIFO( mode );
	}
	
}

