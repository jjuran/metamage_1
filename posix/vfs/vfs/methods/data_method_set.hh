/*
	data_method_set.hh
	------------------
*/

#ifndef VFS_METHODS_DATAMETHODSET_HH
#define VFS_METHODS_DATAMETHODSET_HH

// POSIX
#include <sys/types.h>

// plus
#include "plus/string.hh"

// vfs
#include "vfs/filehandle_ptr.hh"
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	typedef off_t (*getfsize_method)( const node* );
	typedef void  (*truncate_method)( const node*, off_t );
	
	typedef filehandle_ptr (*open_method)( const node*, int flags, mode_t );
	
	typedef plus::string (*slurp_method)( const node* );
	typedef void         (*splat_method)( const node*, const char*, size_t );
	
	
	struct data_method_set
	{
		open_method      open;
		getfsize_method  geteof;
		truncate_method  seteof;
		slurp_method     slurp;
		splat_method     splat;
	};
	
}

#endif
