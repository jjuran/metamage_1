/*	============
 *	DeepFiles.hh
 *	============
 *	
 *	Implemented by DeepFiles.cc
 */

#pragma once

// C++
#include <string>
#include <vector>

// Nitrogen
#include "Nitrogen/Files.h"

// Nitrogen Extras / Iteration
#include "Iteration/FSContents.h"


namespace ALine
{
	
	namespace N = Nitrogen;
	
	using std::string;
	using std::vector;
	
	
	template < class Filter >
	class DeepFileSearch
	{
		public:
			DeepFileSearch( const Filter& filter ) : filter( filter )  {}
			
			operator const vector< FSSpec >&() const  { return result; }
			
		#ifndef __MWERKS__
			
			DeepFileSearch< Filter >& SearchItem( const FSSpec item );
			
		#else
			
			DeepFileSearch< Filter >& SearchItem( const FSSpec& item );
			
		#endif
			
			DeepFileSearch< Filter >& SearchDir( const N::FSDirSpec& dir );
		
		private:
			const Filter& filter;
			vector< FSSpec > result;
	};
	
	template < class Filter >
	DeepFileSearch< Filter >&
	
#ifndef __MWERKS__
	
	DeepFileSearch<Filter>::SearchItem(const FSSpec item)
	
#else
	
	DeepFileSearch<Filter>::SearchItem(const FSSpec& item)
	
#endif
	
	{
		if ( N::FSpTestFileExists( item ) )
		{
			if ( filter( item ) )
			{
				result.push_back( item );
			}
		}
		else if ( N::FSpTestDirectoryExists( item ) )
		{
			SearchDir( N::Convert< N::FSDirSpec >( item ) );
		}
		return *this;
	}
	
	template < class Filter >
	DeepFileSearch< Filter >& DeepFileSearch< Filter >::SearchDir( const N::FSDirSpec& dir )
	{
		std::for_each( N::FSContents( dir ).begin(),
		               N::FSContents( dir ).end(),
		               std::bind1st( std::mem_fun( &DeepFileSearch< Filter >::SearchItem ),
		                             this ) );
		
		return *this;
	}
	
	
	template < class Filter >
	vector< FSSpec > DeepFiles( const N::FSDirSpec& dir, const Filter& filter )
	{
		return DeepFileSearch< Filter >( filter ).SearchDir( dir );
	}
	
	template <class Filter>
	vector< FSSpec > DeepFiles( const FSSpec& item, const Filter& filter )
	{
		return DeepFileSearch< Filter >( filter ).SearchItem( item );
	}
	
	vector< FSSpec > DeepFiles( const N::FSDirSpec& dir );
	vector< FSSpec > DeepFiles( const FSSpec& item );
	
	vector< FSSpec > DeepFiles( const N::FSDirSpec& dir, const string& name );
	vector< FSSpec > DeepFiles( const FSSpec& item, const string& name );
	
}

