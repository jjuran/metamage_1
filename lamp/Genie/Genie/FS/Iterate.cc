/*
	Iterate.cc
	----------
*/

#include "Genie/FS/Iterate.hh"

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSIteratorPtr Iterate( const FSTreePtr& dir )
	{
		return dir->Iterate();
	}
	
}

