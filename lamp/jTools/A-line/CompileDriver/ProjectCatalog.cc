/*	=================
 *	ProjectCatalog.cc
 *	=================
 */

#include "CompileDriver/ProjectCatalog.hh"

// Standard C++
#include <string>
#include <vector>

// Io
#include "io/files.hh"
#include "io/walk.hh"

// POSeven
#include "POSeven/Directory.hh"
#include "POSeven/Pathnames.hh"
#include "POSeven/Stat.hh"


namespace CompileDriver
{
	
	using namespace io::path_descent_operators;
	
	
	static void ScanItemForProjects( const std::string&                                       item,
	                                 std::back_insert_iterator< std::vector< std::string > >  output )
	{
		if ( io::directory_exists( item ) )
		{
			ScanDirForProjects( item, output );
		}
	}
	
	void ScanDirForProjects( const std::string&                                       dir,
	                         std::back_insert_iterator< std::vector< std::string > >  output )
	{
		std::string conf = dir / "A-line.conf";
		
		if ( io::file_exists( conf ) )
		{
			*output++ = conf;
			
			return;
		}
		
		typedef io::directory_contents_traits< std::string >::container_type directory_container;
		
		std::string confd = dir / "A-line.confd";
		
		typedef directory_container::const_iterator Iter;
		
		if ( io::directory_exists( confd ) )
		{
			directory_container contents = io::directory_contents( ( confd ) );
			
			/*
			std::copy( contents.begin(),
			           contents.end(),
			           output );
			*/
			
			for ( Iter it = contents.begin();  it != contents.end();  ++it )
			{
				output++ = confd / it->d_name;
			}
			
			return;
		}
		
		directory_container contents = io::directory_contents( ( dir ) );
		
		/*
		std::for_each( contents.begin(),
		               contents.end(),
		               std::bind2nd( N::PtrFun( ScanItemForProjects ),
		                             output ) );
		*/
		
		for ( Iter it = contents.begin();  it != contents.end();  ++it )
		{
			ScanItemForProjects( dir / it->d_name, output );
		}
	}
	
}

