/*
	data_method_set.hh
	------------------
*/

#ifndef GENIE_FS_DATAMETHODSET_HH
#define GENIE_FS_DATAMETHODSET_HH

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/handle_ptr.hh"
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	typedef off_t (*geteof_method)( const node* );
	typedef void  (*seteof_method)( const node*, off_t );
	
	typedef handle_ptr (*open_method)( const node*, int flags, mode_t );
	
	
	struct data_method_set
	{
		open_method    open;
		geteof_method  geteof;
		seteof_method  seteof;
	};
	
}

namespace Genie
{
	
	using vfs::open_method;
	using vfs::geteof_method;
	using vfs::seteof_method;
	
	using vfs::data_method_set;
	
}

#endif

