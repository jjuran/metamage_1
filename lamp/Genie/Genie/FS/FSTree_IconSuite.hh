/*	===================
 *	FSTree_IconSuite.hh
 *	===================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_ICONSUITE_HH
#define GENIE_FILESYSTEM_FSTREE_ICONSUITE_HH

// Nucleus
#include "Nucleus/Shared.h"

// Nitrogen
#include "Nitrogen/Icons.hh"

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr
	//
	New_FSTree_IconSuite( const FSTreePtr&                                  parent,
			              const std::string&                                name,
			              const Nucleus::Shared< Nitrogen::IconSuiteRef >&  iconSuite );
	
	const Nucleus::Shared< Nitrogen::IconSuiteRef >& Fetch_IconSuite();
	
}

#endif

