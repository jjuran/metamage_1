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
	
	bool exists( const FSTreePtr& file )
	{
		return file->Exists();
	}
	
	bool is_file( const FSTreePtr& file )
	{
		return file->IsFile();
	}
	
	bool is_directory( const FSTreePtr& file )
	{
		return file->IsDirectory();
	}
	
	bool is_symlink( const FSTreePtr& file )
	{
		return file->IsLink();
	}
	
	bool is_fifo( const FSTreePtr& file )
	{
		return S_ISFIFO( file->FileMode() );
	}
	
}

