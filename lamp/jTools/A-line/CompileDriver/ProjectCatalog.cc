/*	=================
 *	ProjectCatalog.cc
 *	=================
 */

#include "CompileDriver/ProjectCatalog.hh"

// Standard C++
#include <string>
#include <vector>

// Nitrogen Extras / Templates
#include "Templates/PointerToFunction.h"

// Nitrogen Extras / Iteration
#include "Iteration/FSContents.h"


namespace CompileDriver
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	static void ScanItemForProjects( const FSSpec&                                       item,
	                                 std::back_insert_iterator< std::vector< FSSpec > >  output )
	{
		if ( N::FSpTestDirectoryExists( item ) )
		{
			ScanDirForProjects( NN::Convert< N::FSDirSpec >( item ), output );
		}
	}
	
	void ScanDirForProjects( const N::FSDirSpec&                                 dir,
	                         std::back_insert_iterator< std::vector< FSSpec > >  output )
	{
		FSSpec conf = dir & "A-line.conf";
		
		if ( N::FSpTestFileExists( conf ) )
		{
			*output++ = conf;
			return;
		}
		
		FSSpec confd = dir & "A-line.confd";
		
		if ( N::FSpTestDirectoryExists( confd ) )
		{
			N::FSDirSpec confdir = NN::Convert< N::FSDirSpec >( confd );
			
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

