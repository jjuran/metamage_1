/*
	file-tests.hh
	-------------
*/


#ifndef GENIE_FS_FILETESTS_HH
#define GENIE_FS_FILETESTS_HH

#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	bool exists( const FSTree* file );
	
	bool is_file( const FSTree* file );
	
	bool is_directory( const FSTree* file );
	
	bool is_symlink( const FSTree* file );
	
	bool is_fifo( const FSTree* file );
	
	
	inline bool exists( const FSTreePtr& file )
	{
		return exists( file.get() );
	}
	
	inline bool is_file( const FSTreePtr& file )
	{
		return is_file( file.get() );
	}
	
	inline bool is_directory( const FSTreePtr& file )
	{
		return is_directory( file.get() );
	}
	
	inline bool is_symlink( const FSTreePtr& file )
	{
		return is_symlink( file.get() );
	}
	
	inline bool is_fifo( const FSTreePtr& file )
	{
		return is_fifo( file.get() );
	}
	
}

#endif

