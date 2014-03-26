/*
	getexe.hh
	---------
*/

#ifndef RELIX_API_GETEXE_HH
#define RELIX_API_GETEXE_HH

// vfs
#include "vfs/node_ptr.hh"


namespace relix
{
	
	class process;
	
	
	vfs::node_ptr getexe( const process& proc );
	
}

#endif
