/*
	link_method_set.hh
	------------------
*/

#ifndef GENIE_FS_LINKMETHODSET_HH
#define GENIE_FS_LINKMETHODSET_HH

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

namespace Genie
{
	
	using vfs::readlink_method;
	using vfs::resolve_method;
	using vfs::symlink_method;
	
	using vfs::link_method_set;
	
}

#endif

