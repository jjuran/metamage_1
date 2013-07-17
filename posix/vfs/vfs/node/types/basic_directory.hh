/*
	basic_directory.hh
	------------------
*/

#ifndef VFS_NODE_TYPES_BASICDIRECTORY_HH
#define VFS_NODE_TYPES_BASICDIRECTORY_HH

// plus
#include "plus/string_fwd.hh"

// vfs
#include "vfs/dir_contents_fwd.hh"
#include "vfs/node_fwd.hh"
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	typedef node_ptr (*lookup_proc)( const node* parent, const plus::string& name );
	
	typedef void (*iterate_proc)( const node* parent, dir_contents& cache );
	
	
	node_ptr new_basic_directory( const node*          parent,
	                              const plus::string&  name,
	                              lookup_proc          lookup,
	                              iterate_proc         iterate );
	
}

#endif

