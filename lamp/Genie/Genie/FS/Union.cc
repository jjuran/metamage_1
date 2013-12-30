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

// plus
#include "plus/string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/node.hh"
#include "vfs/functions/file-tests.hh"
#include "vfs/methods/dir_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/primitives/listdir.hh"
#include "vfs/primitives/lookup.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	using vfs::node;
	using vfs::node_ptr;
	using vfs::dir_method_set;
	using vfs::node_method_set;
	
	
	struct union_extra
	{
		const node* top;
		const node* bottom;
	};
	
	static void dispose_union( const node* that )
	{
		union_extra& extra = *(union_extra*) that->extra();
		
		intrusive_ptr_release( extra.top    );
		intrusive_ptr_release( extra.bottom );
	}
	
	
	static node_ptr union_lookup( const node*          that,
	                              const plus::string&  name,
	                              const node*          parent )
	{
		union_extra& extra = *(union_extra*) that->extra();
		
		try
		{
			vfs::node_ptr child = lookup( *extra.top, name, parent );
			
			if ( exists( *child ) )
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
		
		return lookup( *extra.bottom, name, parent );
	}
	
	static void union_listdir( const node*         that,
	                           vfs::dir_contents&  cache )
	{
		union_extra& extra = *(union_extra*) that->extra();
		
		std::set< plus::string > names_that_have_been_seen;
		
		size_t i = cache.size();
		
		listdir( *extra.top, cache );
		
		for ( i = i;  i < cache.size();  ++i )
		{
			vfs::dir_entry node = cache.at( i );
			
			names_that_have_been_seen.insert( node.name );
		}
		
		vfs::dir_contents bottom;
		
		listdir( *extra.bottom, bottom );
		
		for ( size_t i = 0;  i < bottom.size();  ++i )
		{
			vfs::dir_entry node = bottom.at( i );
			
			if ( !names_that_have_been_seen.count( node.name ) )
			{
				cache.push_back( node );
			}
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
	
	
	node_ptr New_FSTree_Union( const node*          parent,
	                           const plus::string&  name,
	                           const node*          top,
	                           const node*          bottom )
	{
		node* result = new node( parent,
		                         name,
		                         S_IFDIR | 0700,
		                         &union_methods,
		                         sizeof (union_extra),
		                         &dispose_union );
		
		union_extra& extra = *(union_extra*) result->extra();
		
		intrusive_ptr_add_ref( top    );
		intrusive_ptr_add_ref( bottom );
		
		extra.top    = top;
		extra.bottom = bottom;
		
		return result;
	}
	
}

