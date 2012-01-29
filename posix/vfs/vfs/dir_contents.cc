/*
	dir_contents.cc
	---------------
*/

#include "vfs/dir_contents_impl.hh"


namespace vfs
{
	
	void dir_contents::push_back( const dir_entry& entry )
	{
		static_cast< dir_contents_impl* >( this )->push_back( entry );
	}
	
}

