/*
	basic_directory.cc
	------------------
*/

#include "vfs/node/types/basic_directory.hh"

// POSIX
#include <sys/stat.h>

// vfs
#include "vfs/node.hh"
#include "vfs/methods/dir_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	struct basic_dir_extra
	{
		lookup_proc   lookup;
		iterate_proc  iterate;
	};
	
	static node_ptr basic_lookup( const node*          that,
	                              const plus::string&  name,
	                              const node*          parent )
	{
		basic_dir_extra& extra = *(basic_dir_extra*) that->extra();
		
		return extra.lookup( parent, name );
	}
	
	static void basic_listdir( const node*    that,
	                           dir_contents&  cache )
	{
		basic_dir_extra& extra = *(basic_dir_extra*) that->extra();
		
		if ( extra.iterate != NULL )
		{
			extra.iterate( that, cache );
		}
	}
	
	
	static const dir_method_set basic_dir_methods =
	{
		&basic_lookup,
		&basic_listdir
	};
	
	static const node_method_set basic_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&basic_dir_methods
	};
	
	
	node_ptr new_basic_directory( const node*          parent,
	                              const plus::string&  name,
	                              lookup_proc          lookup,
	                              iterate_proc         iterate )
	{
		node* result = new node( parent,
		                         name,
		                         iterate ? S_IFDIR | 0500
		                                 : S_IFDIR | 0100,
		                         &basic_methods,
		                         sizeof (basic_dir_extra) );
		
		basic_dir_extra& extra = *(basic_dir_extra*) result->extra();
		
		extra.lookup  = lookup;
		extra.iterate = iterate;
		
		return result;
	}
	
}

