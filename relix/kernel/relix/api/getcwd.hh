/*
	getcwd.hh
	---------
*/

#ifndef RELIX_API_GETCWD_HH
#define RELIX_API_GETCWD_HH

// vfs
#include "vfs/node_ptr.hh"


namespace relix
{
	
	class process;
	
	
	vfs::node_ptr getcwd( const process& proc );
	
	vfs::node_ptr getcwd();
	
}

#endif

