/*
	OSErr.hh
	--------
*/

#ifndef VARYX_MAC_OSERR_HH
#define VARYX_MAC_OSERR_HH


namespace varyx {
namespace mac   {
	
	void throw_MacOS_error( int err, const char* routine );
	
}
}

#endif
