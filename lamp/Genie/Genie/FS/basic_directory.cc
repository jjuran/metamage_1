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
	
	static void basic_listdir( const FSTree*  node,
	                           FSTreeCache&   cache );
	
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
	
	class basic_directory : public FSTree
	{
		public:
			basic_directory( const FSTreePtr&     parent,
			                 const plus::string&  name,
			                 Lookup_Proc          lookup,
			                 Iterate_Proc         iterate );
	};
	
	
	static FSTreePtr basic_lookup( const FSTree*        node,
	                               const plus::string&  name,
	                               const FSTree*        parent )
	{
		basic_dir_extra& extra = *(basic_dir_extra*) node->extra();
		
		return extra.lookup( parent, name );
	}
	
	static void basic_listdir( const FSTree*  node,
	                           FSTreeCache&   cache )
	{
		basic_dir_extra& extra = *(basic_dir_extra*) node->extra();
		
		if ( extra.iterate != NULL )
		{
			extra.iterate( node, cache );
		}
	}
	
	
	FSTreePtr new_basic_directory( const FSTreePtr&     parent,
	                               const plus::string&  name,
	                               Lookup_Proc          lookup,
	                               Iterate_Proc         iterate )
	{
		return new basic_directory( parent, name, lookup, iterate );
	}
	
	basic_directory::basic_directory( const FSTreePtr&     parent,
	                                  const plus::string&  name,
	                                  Lookup_Proc          lookup,
	                                  Iterate_Proc         iterate )
	:
		FSTree( parent,
		        name,
		        iterate ? S_IFDIR | 0500
		                : S_IFDIR | 0100,
		        &basic_methods,
		        sizeof (basic_dir_extra) )
	{
		basic_dir_extra& extra = *(basic_dir_extra*) this->extra();
		
		extra.lookup  = lookup;
		extra.iterate = iterate;
	}
	
}

