/*
	truncate.cc
	----------
*/

#include "vfs/filehandle/functions/truncate.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/IO/RegularFile.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void truncate( filehandle* that )
	{
		using namespace Genie;
		
		if ( RegularFileHandle* fh = IOHandle_Cast< RegularFileHandle >( that ) )
		{
			fh->SetEOF( 0 );
		}
	}
	
}

