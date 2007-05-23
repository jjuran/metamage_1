/*	============
 *	DeepFiles.hh
 *	============
 */

#ifndef ALINE_DEEPFILES_HH
#define ALINE_DEEPFILES_HH

// C++
#include <string>
#include <vector>

// Io
#include "io/files.hh"

// Nitrogen
#include "Nitrogen/Files.h"

// Nitrogen Extras / Iteration
#include "Iteration/FSContents.h"


namespace ALine
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
#ifndef __MWERKS__
	
	typedef const FSSpec  ConstFSSpecParam;
	
#else
	
	typedef const FSSpec& ConstFSSpecParam;
	
#endif
	
	template < class Filter >
	class DeepFileSearch
	{
		private:
			const Filter& filter;
			std::vector< FSSpec > result;
		
		public:
			DeepFileSearch( const Filter& filter ) : filter( filter )  {}
			
			operator const std::vector< FSSpec >&() const  { return result; }
			
			DeepFileSearch< Filter >& SearchItem( ConstFSSpecParam item );
			
			DeepFileSearch< Filter >& SearchDir( const N::FSDirSpec& dir );
	};
	
	template < class Filter >
	DeepFileSearch< Filter >& DeepFileSearch< Filter >::SearchItem( ConstFSSpecParam item )
	{
		if ( io::file_exists( item ) )
		{
			if ( filter( item ) )
			{
				result.push_back( item );
			}
		}
		else if ( io::directory_exists( item ) )
		{
			SearchDir( N::FSDirSpec( item ) );
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
	std::vector< FSSpec > DeepFiles( const N::FSDirSpec& dir, const Filter& filter )
	{
		return DeepFileSearch< Filter >( filter ).SearchDir( dir );
	}
	
	template <class Filter>
	std::vector< FSSpec > DeepFiles( const FSSpec& item, const Filter& filter )
	{
		return DeepFileSearch< Filter >( filter ).SearchItem( item );
	}
	
	std::vector< FSSpec > DeepFiles( const N::FSDirSpec&  dir );
	std::vector< FSSpec > DeepFiles( const FSSpec&        item );
	
	std::vector< FSSpec > DeepFiles( const N::FSDirSpec&  dir,  const std::string& name );
	std::vector< FSSpec > DeepFiles( const FSSpec&        item, const std::string& name );
	
}

#endif

