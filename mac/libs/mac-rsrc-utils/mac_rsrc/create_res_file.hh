/*
	create_res_file.hh
	------------------
*/

#ifndef MACRSRC_CREATERESFILE_HH
#define MACRSRC_CREATERESFILE_HH


struct FSRef;
struct FSSpec;
struct HFSUniStr255;

typedef unsigned char Byte;

typedef short OSErr;

namespace mac  {
namespace rsrc {
	
	OSErr create_res_file( short vRefNum, long parID, const Byte* name );
	
	OSErr create_res_file( const FSSpec& file );
	OSErr create_res_file( const FSRef&  file );
	
	OSErr create_res_file( FSRef&               result,
	                       const FSRef&         parent,
	                       const HFSUniStr255&  file_name,
	                       const HFSUniStr255&  fork_name );
	
}
}

#endif
