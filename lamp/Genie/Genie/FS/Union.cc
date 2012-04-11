/*
	Union.cc
	--------
	
	Copyright 2010, Joshua Juran
*/

#include "Genie/FS/Union.hh"

// POSIX
#include <sys/stat.h>

// Standard C++
#include <set>

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/file-tests.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/Iterate.hh"
#include "Genie/FS/dir_method_set.hh"
#include "Genie/FS/lookup.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	struct union_extra
	{
		const FSTree* top;
		const FSTree* bottom;
	};
	
	static void dispose_union( const FSTree* node )
	{
		union_extra& extra = *(union_extra*) node->extra();
		
		intrusive_ptr_release( extra.top    );
		intrusive_ptr_release( extra.bottom );
	}
	
	
	static FSTreePtr union_lookup( const FSTree*        node,
	                               const plus::string&  name,
	                               const FSTree*        parent )
	{
		union_extra& extra = *(union_extra*) node->extra();
		
		try
		{
			FSTreePtr child = lookup( extra.top, name, parent );
			
			if ( exists( child ) )
			{
				return child;
			}
		}
		catch ( const p7::errno_t& errnum )
		{
			if ( errnum != ENOENT )
			{
				throw;
			}
		}
		
		return lookup( extra.bottom, name, parent );
	}
	
	static void union_listdir( const FSTree*  node,
	                           FSTreeCache&   cache )
	{
		union_extra& extra = *(union_extra*) node->extra();
		
		std::set< plus::string > names_that_have_been_seen;
		
		FSIteratorPtr top = Genie::Iterate( extra.top );
		
		top->Advance();  // .
		top->Advance();  // ..
		
		while ( true )
		{
			vfs::dir_entry node = top->Get();
			
			if ( node.name.empty() )
			{
				break;
			}
			
			names_that_have_been_seen.insert( node.name );
			
			cache.push_back( node );
			
			top->Advance();
		}
		
		FSIteratorPtr bottom = Genie::Iterate( extra.bottom );
		
		bottom->Advance();  // .
		bottom->Advance();  // ..
		
		while ( true )
		{
			vfs::dir_entry node = bottom->Get();
			
			if ( node.name.empty() )
			{
				break;
			}
			
			if ( !names_that_have_been_seen.count( node.name ) )
			{
				cache.push_back( node );
			}
			
			bottom->Advance();
		}
	}
	
	static const dir_method_set union_dir_methods =
	{
		&union_lookup,
		&union_listdir
	};
	
	static const node_method_set union_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&union_dir_methods
	};
	
	
	FSTreePtr New_FSTree_Union( const FSTreePtr&     parent,
	                            const plus::string&  name,
	                            const FSTreePtr&     top,
	                            const FSTreePtr&     bottom )
	{
		FSTree* result = new FSTree( parent,
		                             name,
		                             S_IFDIR | 0700,
		                             &union_methods,
		                             sizeof (union_extra),
		                             &dispose_union );
		
		union_extra& extra = *(union_extra*) result->extra();
		
		intrusive_ptr_add_ref( top   .get() );
		intrusive_ptr_add_ref( bottom.get() );
		
		extra.top    = top.get();
		extra.bottom = bottom.get();
		
		return result;
	}
	
}

