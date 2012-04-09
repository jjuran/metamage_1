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
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/Iterate.hh"
#include "Genie/FS/dir_method_set.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	class FSTree_Union : public FSTree
	{
		private:
			FSTreePtr itsTop;
			FSTreePtr itsBottom;
		
		public:
			FSTree_Union( const FSTreePtr&     parent,
			              const plus::string&  name );
			
			void SetTop( const FSTreePtr& top )
			{
				itsTop = top;
			}
			
			void SetBottom( const FSTreePtr& bottom )
			{
				itsBottom = bottom;
			}
			
			FSTreePtr Lookup_Child( const plus::string& name, const FSTree* parent ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	static FSTreePtr union_lookup( const FSTree*        node,
	                               const plus::string&  name,
	                               const FSTree*        parent )
	{
		const FSTree_Union* file = static_cast< const FSTree_Union* >( node );
		
		return file->Lookup_Child( name, parent );
	}
	
	static void union_listdir( const FSTree*  node,
	                           FSTreeCache&   cache )
	{
		const FSTree_Union* file = static_cast< const FSTree_Union* >( node );
		
		file->IterateIntoCache( cache );
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
	
	
	FSTree_Union::FSTree_Union( const FSTreePtr&     parent,
	                            const plus::string&  name )
	:
		FSTree( parent, name, S_IFDIR | 0700, &union_methods )
	{
	}
	
	FSTreePtr FSTree_Union::Lookup_Child( const plus::string& name, const FSTree* parent ) const
	{
		try
		{
			FSTreePtr child = itsTop->Lookup( name, parent );
			
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
		
		return itsBottom->Lookup( name, parent );
	}
	
	void FSTree_Union::IterateIntoCache( FSTreeCache& cache ) const
	{
		std::set< plus::string > names_that_have_been_seen;
		
		FSIteratorPtr top = Genie::Iterate( itsTop );
		
		top->Advance();  // .
		top->Advance();  // ..
		
		while ( true )
		{
			FSNode node = top->Get();
			
			if ( node.name.empty() )
			{
				break;
			}
			
			names_that_have_been_seen.insert( node.name );
			
			cache.push_back( node );
			
			top->Advance();
		}
		
		FSIteratorPtr bottom = Genie::Iterate( itsBottom );
		
		bottom->Advance();  // .
		bottom->Advance();  // ..
		
		while ( true )
		{
			FSNode node = bottom->Get();
			
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
	
	FSTreePtr New_FSTree_Union( const FSTreePtr&     parent,
	                            const plus::string&  name,
	                            const FSTreePtr&     top,
	                            const FSTreePtr&     bottom )
	{
		FSTree_Union* result = new FSTree_Union( parent, name );
		
		result->SetTop   ( top    );
		result->SetBottom( bottom );
		
		return result;
	}
	
}

