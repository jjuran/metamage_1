/*
	file-tests.hh
	-------------
*/

#ifndef VFS_FUNCTIONS_FILETESTS_HH
#define VFS_FUNCTIONS_FILETESTS_HH

// vfs
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	bool exists( const node& file );
	
	bool is_file( const node& file );
	
	bool is_directory( const node& file );
	
	bool is_symlink( const node& file );
	
	bool is_fifo( const node& file );
	
}

#endif

