/*
	create_IconRef_from_icns.hh
	---------------------------
*/

#ifndef MACICON_CREATEICONREFFROMICNS_HH
#define MACICON_CREATEICONREFFROMICNS_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#else
struct FSRef;
typedef struct OpaqueIconRef* IconRef;
#endif


namespace mac  {
namespace icon {
	
	IconRef create_IconRef_from_icns( const FSRef& file );
	
}
}

#endif
