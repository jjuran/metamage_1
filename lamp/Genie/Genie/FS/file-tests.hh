/*
	file-tests.hh
	-------------
*/


#ifndef GENIE_FS_FILETESTS_HH
#define GENIE_FS_FILETESTS_HH

#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	bool exists( const FSTreePtr& file );
	
	bool is_file( const FSTreePtr& file );
	
	bool is_directory( const FSTreePtr& file );
	
	bool is_symlink( const FSTreePtr& file );
	
	bool is_fifo( const FSTreePtr& file );
	
}

#endif

