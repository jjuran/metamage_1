/*
	file-tests.cc
	-------------
*/

#include "Genie/FS/file-tests.hh"


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
		return file->IsPipe();
	}
	
}

