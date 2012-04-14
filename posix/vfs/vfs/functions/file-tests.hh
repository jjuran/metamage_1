/*
	file-tests.hh
	-------------
*/

#ifndef VFS_FUNCTIONS_FILETESTS_HH
#define VFS_FUNCTIONS_FILETESTS_HH

// vfs
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	bool exists( const node* file );
	
	bool is_file( const node* file );
	
	bool is_directory( const node* file );
	
	bool is_symlink( const node* file );
	
	bool is_fifo( const node* file );
	
	
	inline bool exists( const node_ptr& file )
	{
		return exists( file.get() );
	}
	
	inline bool is_file( const node_ptr& file )
	{
		return is_file( file.get() );
	}
	
	inline bool is_directory( const node_ptr& file )
	{
		return is_directory( file.get() );
	}
	
	inline bool is_symlink( const node_ptr& file )
	{
		return is_symlink( file.get() );
	}
	
	inline bool is_fifo( const node_ptr& file )
	{
		return is_fifo( file.get() );
	}
	
}

#endif

