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
		private:
			Lookup_Proc   itsLookup;
			Iterate_Proc  itsIterate;
		
		public:
			basic_directory( const FSTreePtr&     parent,
			                 const plus::string&  name,
			                 Lookup_Proc          lookup,
			                 Iterate_Proc         iterate );
			
			FSTreePtr Lookup_Child( const plus::string& name, const FSTree* parent ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	
	FSTreePtr basic_directory::Lookup_Child( const plus::string& name, const FSTree* parent ) const
	{
		return itsLookup( parent, name );
	}
	
	void basic_directory::IterateIntoCache( FSTreeCache& cache ) const
	{
		if ( itsIterate != NULL )
		{
			itsIterate( Self(), cache );
		}
	}
	
	static FSTreePtr basic_lookup( const FSTree*        node,
	                               const plus::string&  name,
	                               const FSTree*        parent )
	{
		const basic_directory* file = static_cast< const basic_directory* >( node );
		
		return file->Lookup_Child( name, parent );
	}
	
	static void basic_listdir( const FSTree*  node,
	                           FSTreeCache&   cache )
	{
		const basic_directory* file = static_cast< const basic_directory* >( node );
		
		file->IterateIntoCache( cache );
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
		        &basic_methods ),
		itsLookup ( lookup  ),
		itsIterate( iterate )
	{
	}
	
}

