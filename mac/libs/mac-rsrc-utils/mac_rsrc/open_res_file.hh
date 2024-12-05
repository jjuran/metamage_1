/*
	open_res_file.hh
	----------------
*/

#ifndef MACRSRC_OPENRESFILE_HH
#define MACRSRC_OPENRESFILE_HH

// mac-rsrc-utils
#include "mac_rsrc/ResFileRefNum.hh"


struct FSRef;
struct FSSpec;
struct HFSUniStr255;

namespace mac  {
namespace rsrc {
	
	ResFileRefNum open_res_file( const FSSpec& file, signed char perm );
	ResFileRefNum open_res_file( const FSRef&  file, signed char perm );
	
	ResFileRefNum open_res_file( const FSRef&         file,
	                             const HFSUniStr255&  forkName,
	                             signed char          perm );
	
}
}

#endif
