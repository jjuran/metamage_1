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

// Nitrogen Extras / Templates
#include "Templates/PointerToFunction.h"

// Nitrogen Extras / Iteration
#include "Iteration/FSContents.h"


namespace CompileDriver
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	using namespace io::path_descent_operators;
	
	
	static void ScanItemForProjects( const FSSpec&                                       item,
	                                 std::back_insert_iterator< std::vector< FSSpec > >  output )
	{
		if ( io::directory_exists( item ) )
		{
			typedef io::filespec_traits< FSSpec >::optimized_directory_spec directory_spec;
			
			ScanDirForProjects( directory_spec( item ), output );
		}
	}
	
	void ScanDirForProjects( const N::FSDirSpec&                                 dir,
	                         std::back_insert_iterator< std::vector< FSSpec > >  output )
	{
		typedef io::filespec_traits< N::FSDirSpec >::file_spec file_spec;
		
		file_spec conf = dir / "A-line.conf";
		
		if ( io::file_exists( conf ) )
		{
			*output++ = conf;
			return;
		}
		
		typedef io::directory_contents_traits< N::FSDirSpec >::container_type directory_container;
		
		file_spec confd = dir / "A-line.confd";
		
		if ( io::directory_exists( confd ) )
		{
			directory_container contents = io::directory_contents( confd );
			
			std::copy( contents.begin(),
			           contents.end(),
			           output );
			return;
		}
		
		directory_container contents = io::directory_contents( dir );
		
		std::for_each( contents.begin(),
		               contents.end(),
		               std::bind2nd( N::PtrFun( ScanItemForProjects ),
		                             output ) );
		
	}
	
}

