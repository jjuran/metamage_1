/*
	volume_params.hh
	----------------
*/

#ifndef MACSYS_VOLUMEPARAMS_HH
#define MACSYS_VOLUMEPARAMS_HH

// Mac OS
//#include <Files.h>
struct GetVolParmsInfoBuffer;


namespace mac {
namespace sys {
	
	short get_volume_params( ::GetVolParmsInfoBuffer& buffer, short vRefNum );
	
}
}

#endif
