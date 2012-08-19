/*
	data_method_set.hh
	------------------
*/

#ifndef VFS_METHODS_DATAMETHODSET_HH
#define VFS_METHODS_DATAMETHODSET_HH

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/filehandle_ptr.hh"
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	typedef off_t (*geteof_method)( const node* );
	typedef void  (*seteof_method)( const node*, off_t );
	
	typedef filehandle_ptr (*open_method)( const node*, int flags, mode_t );
	
	
	struct data_method_set
	{
		open_method    open;
		geteof_method  geteof;
		seteof_method  seteof;
	};
	
}

#endif

