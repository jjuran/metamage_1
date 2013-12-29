/*
	dir_contents.cc
	---------------
*/

#include "vfs/dir_contents_impl.hh"

// vfs
#include "vfs/dir_entry.hh"


namespace vfs
{
	
	dir_contents::~dir_contents()
	{
	}
	
	void dir_contents::push_back( const dir_entry& entry )
	{
		static_cast< dir_contents_impl* >( this )->push_back( entry );
	}
	
}

