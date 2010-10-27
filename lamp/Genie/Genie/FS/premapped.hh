/*
	premapped.hh
	------------
*/

#ifndef GENIE_FS_PREMAPPED_HH
#define GENIE_FS_PREMAPPED_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr premapped_factory( const FSTreePtr&     parent,
	                             const plus::string&  name,
	                             const void*          args );
	
}

#endif

