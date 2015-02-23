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

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	vfs::node_ptr
	//
	New_FSTree_IconSuite( const vfs::node*                          parent,
			              const plus::string&                       name,
			              nucleus::owned< Nitrogen::IconSuiteRef >  iconSuite );
	
	const Nitrogen::IconSuiteRef Fetch_IconSuite();
	
}

#endif
