/*
	basic_directory.cc
	------------------
	
	Copyright 2009, Joshua Juran
*/

#include "Genie/FS/basic_directory.hh"

// POSIX
#include <sys/stat.h>

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/dir_method_set.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	struct basic_dir_extra
	{
		Lookup_Proc   lookup;
		Iterate_Proc  iterate;
	};
	
	static FSTreePtr basic_lookup( const FSTree*        node,
	                               const plus::string&  name,
	                               const FSTree*        parent );
	
	static void basic_listdir( const FSTree*       node,
	                           vfs::dir_contents&  cache );
	
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
	
	
	static FSTreePtr basic_lookup( const FSTree*        node,
	                               const plus::string&  name,
	                               const FSTree*        parent )
	{
		basic_dir_extra& extra = *(basic_dir_extra*) node->extra();
		
		return extra.lookup( parent, name );
	}
	
	static void basic_listdir( const FSTree*       node,
	                           vfs::dir_contents&  cache )
	{
		basic_dir_extra& extra = *(basic_dir_extra*) node->extra();
		
		if ( extra.iterate != NULL )
		{
			extra.iterate( node, cache );
		}
	}
	
	
	FSTreePtr new_basic_directory( const FSTree*        parent,
	                               const plus::string&  name,
	                               Lookup_Proc          lookup,
	                               Iterate_Proc         iterate )
	{
		FSTree* result = new FSTree( parent,
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

