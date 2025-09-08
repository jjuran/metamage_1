/*
	create_res_file.hh
	------------------
*/

#ifndef MACRSRC_CREATERESFILE_HH
#define MACRSRC_CREATERESFILE_HH

struct FSRef;
struct FSSpec;

typedef unsigned char Byte;

typedef short OSErr;

namespace mac  {
namespace rsrc {
	
	OSErr create_res_file( short vRefNum, long parID, const Byte* name );
	
	OSErr create_res_file( const FSSpec& file );
	OSErr create_res_file( const FSRef&  file );
	
}
}

#endif
