/*	===================
 *	FSTree_Directory.cc
 *	===================
 */

#include "Genie/FS/FSTree_Directory.hh"

// POSIX
#include <sys/stat.h>

// Genie
#include "Genie/FS/file-tests.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/FSTree_Null.hh"
#include "Genie/FS/dir_method_set.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	namespace premapped
	{
		
		const mapping empty_mappings[] = { { NULL, NULL } };
		
	}
	
	
	static void premapped_remove( const FSTree* node );
	
	static FSTreePtr premapped_lookup( const FSTree*        node,
	                                   const plus::string&  name,
	                                   const FSTree*        parent );
	
	static void premapped_listdir( const FSTree*  node,
	                               FSTreeCache&   cache );
	
	static const dir_method_set premapped_dir_methods =
	{
		&premapped_lookup,
		&premapped_listdir
	};
	
	static const node_method_set premapped_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		&premapped_remove,
		NULL,
		NULL,
		NULL,
		&premapped_dir_methods
	};
	
	class FSTree_Premapped : public FSTree
	{
		private:
			typedef const premapped::mapping* Mappings;
			
			typedef premapped::destructor Destructor;
			
			Mappings    itsMappings;
		
		public:
			FSTree_Premapped( const FSTreePtr&     parent,
			                  const plus::string&  name,
			                  Mappings             mappings,
			                  Destructor           dtor );
			
			void Delete() const;
			
			FSTreePtr Lookup_Child( const plus::string& name, const FSTree* parent ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	FSTree_Premapped::FSTree_Premapped( const FSTreePtr&     parent,
	                                    const plus::string&  name,
	                                    Mappings             mappings,
	                                    Destructor           dtor )
	:
		FSTree( parent,
		        name,
		        S_IFDIR | 0700,
		        &premapped_methods,
		        0,
		        dtor ),
		itsMappings( mappings )
	{
	}
	
	static void premapped_remove( const FSTree* node )
	{
		const FSTree_Premapped* file = static_cast< const FSTree_Premapped* >( node );
		
		file->Delete();
	}
	
	static FSTreePtr premapped_lookup( const FSTree*        node,
	                                   const plus::string&  name,
	                                   const FSTree*        parent )
	{
		const FSTree_Premapped* file = static_cast< const FSTree_Premapped* >( node );
		
		return file->Lookup_Child( name, parent );
	}
	
	static void premapped_listdir( const FSTree*  node,
	                               FSTreeCache&   cache )
	{
		const FSTree_Premapped* file = static_cast< const FSTree_Premapped* >( node );
		
		file->IterateIntoCache( cache );
	}
	
	
	static const premapped::mapping*
	//
	find_mapping( const premapped::mapping* mappings, const plus::string& name )
	{
		for ( const premapped::mapping* it = mappings;  it->name;  ++it )
		{
			if ( it->name == name )
			{
				return it;
			}
		}
		
		return NULL;
	}
	
	void FSTree_Premapped::Delete() const
	{
		if ( node_destructor dtor = destructor() )
		{
			dtor( static_cast< const FSTree* >( this ) );
		}
	}
	
	FSTreePtr FSTree_Premapped::Lookup_Child( const plus::string& name, const FSTree* parent ) const
	{
		if ( const premapped::mapping* it = find_mapping( itsMappings, name ) )
		{
			return it->f( parent, name, it->args );
		}
		
		return FSNull();
	}
	
	void FSTree_Premapped::IterateIntoCache( FSTreeCache& cache ) const
	{
		for ( const premapped::mapping* it = itsMappings;  it->name != NULL;  ++it )
		{
			const plus::string& name = it->name;
			
			premapped::function f = it->f;
			
			try
			{
				FSTreePtr file = f( Self(), name, it->args );
				
				if ( !exists( file ) )
				{
					continue;
				}
				
				ino_t inode = 0;
				
				cache.push_back( FSNode( inode, name ) );
			}
			catch ( ... )
			{
			}
		}
	}
	
	
	FSTreePtr Premapped_Factory( const FSTreePtr&            parent,
	                             const plus::string&         name,
	                             const premapped::mapping    mappings[],
	                             void                      (*dtor)(const FSTree*) )
	{
		return FSTreePtr( new FSTree_Premapped( parent, name, mappings, dtor ) );
	}
	
}

