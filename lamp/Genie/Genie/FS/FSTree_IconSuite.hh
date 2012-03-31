/*	===================
 *	FSTree_IconSuite.hh
 *	===================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_ICONSUITE_HH
#define GENIE_FILESYSTEM_FSTREE_ICONSUITE_HH

// plus
#include "plus/string.hh"

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef NITROGEN_ICONS_HH
#include "Nitrogen/Icons.hh"
#endif

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr
	//
	New_FSTree_IconSuite( const FSTreePtr&                          parent,
			              const plus::string&                       name,
			              nucleus::owned< Nitrogen::IconSuiteRef >  iconSuite );
	
	const Nitrogen::IconSuiteRef Fetch_IconSuite();
	
}

#endif

