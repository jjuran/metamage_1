/*
	IconRefFromIconFile.cc
	----------------------
*/

#include "Pedestal/IconRefFromIconFile.hh"

// mac-icon-utils
#include "mac_icon/create_IconRef_from_icns.hh"


namespace Pedestal
{
	
	namespace n = nucleus;
	
	
	n::owned< IconRef > IconRefFromIconFile( const FSRef& file )
	{
		if ( IconRef icon = mac::icon::create_IconRef_from_icns( file ) )
		{
			return n::owned< IconRef >::seize( icon );
		}
		
		return n::owned< IconRef >();
	}
	
}
