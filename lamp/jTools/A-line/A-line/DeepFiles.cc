/*	============
 *	DeepFiles.cc
 *	============
 */

#include "A-line/DeepFiles.hh"

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
	
	
	DeepFileSearch& DeepFileSearch::SearchItem( plus::string item )
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
	
	DeepFileSearch& DeepFileSearch::SearchDir( const plus::string& dir )
	{
		typedef io::directory_contents_traits< plus::string >::container_type directory_container;
		
		typedef plus::string (*path_descender)(const plus::string&, const char*);  // FIXME
		
		directory_container contents = io::directory_contents( dir );
		
		typedef directory_container::const_iterator Iter;
		
		const Iter begin = contents.begin();
		const Iter end   = contents.end  ();
		
		for ( Iter it = begin;  it != end;  ++it )
		{
			SearchItem( dir / *it );
		}
		
		return *this;
	}
	
}

