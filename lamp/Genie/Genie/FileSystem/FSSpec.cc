/*	=========
 *	FSSpec.cc
 *	=========
 */

#include "Genie/FileSystem/FSSpec.hh"

// Genie
#include "Genie/Utilities/AsyncIO.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	FSTreePtr FSTreeFromFSDirSpec( const N::FSDirSpec& dir )
	{
		return FSTreeFromFSSpec( FSMakeFSSpec< FNF_Throws >( dir, NULL ) );
	}
	
}

