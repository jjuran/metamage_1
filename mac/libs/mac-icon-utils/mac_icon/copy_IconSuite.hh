/*
	copy_IconSuite.hh
	-----------------
*/

#ifndef MACICON_COPYICONSUITE_HH
#define MACICON_COPYICONSUITE_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#else
typedef char** IconSuiteRef;  // typedef of Handle
#endif


namespace mac  {
namespace icon {
	
	IconSuiteRef copy_IconSuite( IconSuiteRef iconSuite );
	
}
}

#endif
