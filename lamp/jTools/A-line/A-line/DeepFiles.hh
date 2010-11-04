/*	============
 *	DeepFiles.hh
 *	============
 */

#ifndef ALINE_DEEPFILES_HH
#define ALINE_DEEPFILES_HH

// C++
#include <vector>

// plus
#include "plus/functional_extensions.hh"
#include "plus/pointer_to_function.hh"
#include "plus/string.hh"

// Io
#ifndef IO_FILES_HH
#include "io/files.hh"
#endif
#ifndef IO_WALK_HH
#include "io/walk.hh"
#endif

// poseven
#ifndef POSEVEN_FUNCTIONS_LSTAT_HH
#include "poseven/functions/lstat.hh"
#endif
#ifndef POSEVEN_SEQUENCES_DIRECTORYCONTENTS_HH
#include "poseven/sequences/directory_contents.hh"
#endif

// pfiles
#include "pfiles/common.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	
	using namespace io::path_descent_operators;
	
	
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
		struct stat sb = p7::lstat( item );
		
		if ( S_ISREG( sb.st_mode ) )
		{
			if ( filter( item ) )
			{
				result.push_back( item );
			}
		}
		else if ( S_ISDIR( sb.st_mode ) )
		{
			SearchDir( ( item ) );
		}
		
		return *this;
	}
	
	template < class FileSpec, class Filter >
	DeepFileSearch< FileSpec, Filter >& DeepFileSearch< FileSpec, Filter >::SearchDir( const FileSpec& dir )
	{
		typedef typename io::directory_contents_traits< FileSpec >::container_type directory_container;
		
		typedef FileSpec (*path_descender)(const FileSpec&, const char*);  // FIXME
		
		directory_container contents = io::directory_contents( dir );
		
		std::for_each( contents.begin(),
		               contents.end(),
		               plus::compose1( std::bind1st( std::mem_fun( &DeepFileSearch< FileSpec, Filter >::SearchItem ),
		                                             this ),
		                               std::bind1st( plus::ptr_fun( path_descender( io::path_descent ) ),
		                                             dir ) ) );
		
		return *this;
	}
	
	
	template < class Filter >
	std::vector< plus::string > DeepFiles( const plus::string& item, const Filter& filter )
	{
		return DeepFileSearch< plus::string, Filter >( filter ).SearchItem( item );
	}
	
	std::vector< plus::string > DeepFiles( const plus::string& item );
	
}

#endif

