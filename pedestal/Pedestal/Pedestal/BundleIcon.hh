/*
	BundleIcon.hh
	-------------
*/

#ifndef PEDESTAL_BUNDLEICON_HH
#define PEDESTAL_BUNDLEICON_HH

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_ICONS_TYPES_ICONREF_HH
#include "Mac/Icons/Types/IconRef.hh"
#endif


namespace Pedestal
{
	
	nucleus::owned< IconRef > BundleIcon();
	
}

#endif
