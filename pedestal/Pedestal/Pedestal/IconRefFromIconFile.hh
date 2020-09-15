/*
	IconRefFromIconFile.hh
	----------------------
*/

#ifndef PEDESTAL_ICONREFFROMICONFILE_HH
#define PEDESTAL_ICONREFFROMICONFILE_HH

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
	
	nucleus::owned< IconRef > IconRefFromIconFile( const FSRef& file );
	
}

#endif
