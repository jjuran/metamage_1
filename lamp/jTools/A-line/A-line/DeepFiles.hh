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
#include "io/walk.hh"

// POSeven
#include "POSeven/Directory.hh"
#include "POSeven/Pathnames.hh"
#include "POSeven/Stat.hh"

// Nitrogen Extras / Iteration
#include "Iteration/FSContents.h"


namespace ALine
{
	
	using namespace io::path_descent_operators;
	
	
	inline std::string FileSpecFromEntry( const std::string& parent, const dirent& entry )
	{
		return parent / entry.d_name;
	}
	
	inline FSSpec FileSpecFromEntry( const FSSpec& parent, const FSSpec& entry )
	{
		return entry;
	}
	
	
	template < class FileSpec, class Filter >
	class DeepFileSearch
	{
		private:
			const Filter& filter;
			std::vector< FileSpec > result;
		
		public:
			DeepFileSearch( const Filter& filter ) : filter( filter )  {}
			
			operator const std::vector< FileSpec >&() const  { return result; }
			
			DeepFileSearch< FileSpec, Filter >& SearchItem( FileSpec item );
			
			DeepFileSearch< FileSpec, Filter >& SearchDir( const FileSpec& dir );
	};
	
	template < class FileSpec, class Filter >
	DeepFileSearch< FileSpec, Filter >& DeepFileSearch< FileSpec, Filter >::SearchItem( FileSpec item )
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
			SearchDir( ( item ) );
		}
		
		return *this;
	}
	
	template < class FileSpec, class Filter >
	DeepFileSearch< FileSpec, Filter >& DeepFileSearch< FileSpec, Filter >::SearchDir( const FileSpec& dir )
	{
		typedef typename io::directory_contents_traits< FileSpec >::container_type directory_container;
		
		directory_container contents = io::directory_contents( dir );
		
		/*
		std::for_each( contents.begin(),
		               contents.end(),
		               std::bind1st( std::mem_fun( &DeepFileSearch< FileSpec, Filter >::SearchItem ),
		                             this ) );
		*/
		
		typename directory_container::const_iterator it;
		
		for ( it = contents.begin();  it != contents.end();  ++it )
		{
			
			SearchItem( FileSpecFromEntry( dir, *it ) );
		}
		
		return *this;
	}
	
	
	template < class Filter >
	std::vector< FSSpec > DeepFiles( const FSSpec& item, const Filter& filter )
	{
		return DeepFileSearch< FSSpec, Filter >( filter ).SearchItem( item );
	}
	
	template < class Filter >
	std::vector< std::string > DeepFiles( const std::string& item, const Filter& filter )
	{
		return DeepFileSearch< std::string, Filter >( filter ).SearchItem( item );
	}
	
	std::vector< FSSpec > DeepFiles( const FSSpec& item );
	
	std::vector< std::string > DeepFiles( const std::string& item );
	
}

#endif

