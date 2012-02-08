/*
	Union.cc
	--------
	
	Copyright 2010, Joshua Juran
*/

#include "Genie/FS/Union.hh"

// Standard C++
#include <set>

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/file-tests.hh"
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/Iterate.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	FSTreePtr FSTree_Union::Lookup_Child( const plus::string& name, const FSTree* parent ) const
	{
		if ( parent == NULL )
		{
			parent = this;
		}
		
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
	
}

