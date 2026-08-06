/*
	open_res_file.cc
	----------------
*/

#include "Genie/Utilities/open_res_file.hh"

// mac-sys-utils
#include "mac_sys/res_error.hh"

// mac-rsrc-utils
#include "mac_rsrc/open_res_file.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


namespace Genie
{

ResFileRefNum open_res_file( const FSSpec& file, signed char perm )
{
	ResFileRefNum opened = mac::rsrc::open_res_file( file, perm );
	
	Mac::ThrowOSStatus( mac::sys::res_error() );
	
	return opened;
}

}
