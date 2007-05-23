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
			ScanDirForProjects( N::FSDirSpec( item ), output );
		}
	}
	
	void ScanDirForProjects( const N::FSDirSpec&                                 dir,
	                         std::back_insert_iterator< std::vector< FSSpec > >  output )
	{
		FSSpec conf = dir / "A-line.conf";
		
		if ( io::file_exists( conf ) )
		{
			*output++ = conf;
			return;
		}
		
		FSSpec confd = dir / "A-line.confd";
		
		if ( io::directory_exists( confd ) )
		{
			N::FSDirSpec confdir = N::FSDirSpec( confd );
			
			std::copy( N::FSContents( confdir ).begin(),
			           N::FSContents( confdir ).end(),
			           output );
			return;
		}
		
		std::for_each( N::FSContents( dir ).begin(),
		               N::FSContents( dir ).end(),
		               std::bind2nd( N::PtrFun( ScanItemForProjects ),
		                             output ) );
		
	}
	
}

