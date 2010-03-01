/*	===================
 *	FSTree_IconSuite.hh
 *	===================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_ICONSUITE_HH
#define GENIE_FILESYSTEM_FSTREE_ICONSUITE_HH

// nucleus
#include "nucleus/shared.hh"

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
			              const nucleus::shared< Nitrogen::IconSuiteRef >&  iconSuite );
	
	const nucleus::shared< Nitrogen::IconSuiteRef >& Fetch_IconSuite();
	
}

#endif

