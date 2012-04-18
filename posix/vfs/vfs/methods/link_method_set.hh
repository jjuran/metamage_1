/*
	link_method_set.hh
	------------------
*/

#ifndef VFS_METHODS_LINKMETHODSET_HH
#define VFS_METHODS_LINKMETHODSET_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	typedef plus::string (*readlink_method)( const node* );
	
	typedef node_ptr (*resolve_method)( const node* );
	
	typedef void (*symlink_method)( const node*, const plus::string& );
	
	
	struct link_method_set
	{
		readlink_method readlink;
		resolve_method  resolve;
		symlink_method  symlink;
	};
	
}

#endif

