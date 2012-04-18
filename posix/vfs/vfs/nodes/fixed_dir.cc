/*
	fixed_dir.hh
	------------
*/

#include "vfs/nodes/fixed_dir.hh"

// POSIX
#include <sys/stat.h>

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/node.hh"
#include "vfs/functions/file-tests.hh"
#include "vfs/methods/dir_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/nodes/null.hh"


namespace vfs
{
	
	const fixed_mapping empty_mappings[] = { { NULL, NULL } };
	
	
	struct fixed_dir_extra
	{
		fixed_mapping const*  mappings;
	};
	
	static void fixed_dir_remove( const node* dir );
	
	static node_ptr fixed_dir_lookup( const node*          dir,
	                                  const plus::string&  name,
	                                  const node*          parent );
	
	static void fixed_dir_listdir( const node*         dir,
	                               vfs::dir_contents&  cache );
	
	static const dir_method_set fixed_dir_dir_methods =
	{
		&fixed_dir_lookup,
		&fixed_dir_listdir
	};
	
	static const node_method_set fixed_dir_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		&fixed_dir_remove,
		NULL,
		NULL,
		NULL,
		&fixed_dir_dir_methods
	};
	
	
	static const fixed_mapping*
	//
	find_mapping( const fixed_mapping* mappings, const plus::string& name )
	{
		for ( const fixed_mapping* it = mappings;  it->name;  ++it )
		{
			if ( it->name == name )
			{
				return it;
			}
		}
		
		return NULL;
	}
	
	static void fixed_dir_remove( const node* dir )
	{
		if ( node_destructor dtor = dir->destructor() )
		{
			dtor( dir );
		}
	}
	
	static node_ptr fixed_dir_lookup( const node*          dir,
	                                  const plus::string&  name,
	                                  const node*          parent )
	{
		fixed_dir_extra& extra = *(fixed_dir_extra*) dir->extra();
		
		if ( const fixed_mapping* it = find_mapping( extra.mappings, name ) )
		{
			return it->f( parent, name, it->args );
		}
		
		return vfs::null();
	}
	
	static void fixed_dir_listdir( const node*         dir,
	                               vfs::dir_contents&  cache )
	{
		fixed_dir_extra& extra = *(fixed_dir_extra*) dir->extra();
		
		for ( const fixed_mapping* it = extra.mappings;  it->name != NULL;  ++it )
		{
			const plus::string& name = it->name;
			
			node_factory f = it->f;
			
			try
			{
				node_ptr file = f( dir, name, it->args );
				
				if ( !exists( file ) )
				{
					continue;
				}
				
				ino_t inode = 0;
				
				cache.push_back( vfs::dir_entry( inode, name ) );
			}
			catch ( ... )
			{
			}
		}
	}
	
	
	node_ptr fixed_dir( const node*            parent,
	                    const plus::string&    name,
	                    const fixed_mapping    mappings[],
	                    void                 (*dtor)(const node*) )
	{
		node* result = new node( parent,
		                         name,
		                         S_IFDIR | 0700,
		                         &fixed_dir_methods,
		                         sizeof (fixed_dir_extra),
		                         dtor );
		
		fixed_dir_extra& extra = *(fixed_dir_extra*) result->extra();
		
		extra.mappings = mappings;
		
		return result;
	}
	
}

