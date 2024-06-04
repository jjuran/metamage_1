/*
	create_res_file.hh
	------------------
*/

#ifndef MACRSRC_CREATERESFILE_HH
#define MACRSRC_CREATERESFILE_HH

struct FSRef;
struct FSSpec;

namespace mac  {
namespace rsrc {
	
	short create_res_file( const FSSpec& file );
	short create_res_file( const FSRef&  file );
	
}
}

#endif
