// Nitrogen/LSOpen.hh
// ------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2006 by Marshall Clow and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_LSOPEN_HH
#define NITROGEN_LSOPEN_HH

#ifndef __MACH__
#error "These routines are only directly callable from MachO"
#endif

#ifndef __LSOPEN__
#include <ApplicationServices/ApplicationServices.h>
#endif

#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
#endif

#ifndef NITROGEN_CFURL_HH
#include "Nitrogen/CFURL.hh"
#endif


namespace Nitrogen {
	
	class LaunchServicesErrorsRegistrationDependency
	{
		public:
			LaunchServicesErrorsRegistrationDependency();
	};
	
/* ================================================================== */
/* LaunchServices API                                                                                       */
/* ================================================================== */

	inline FSRef LSOpenFSRef ( const FSRef &inRef ) {
		(void) LaunchServicesErrorsRegistrationDependency();
		FSRef	result;
		ThrowOSStatus ( ::LSOpenFSRef ( &inRef, &result ));
		return result;
		}

	inline nucleus::owned<CFURLRef> LSOpenCFURLRef ( CFURLRef inURL ) {
		(void) LaunchServicesErrorsRegistrationDependency();
		CFURLRef	result;
		ThrowOSStatus ( ::LSOpenCFURLRef ( inURL, &result ));
  	    return nucleus::owned<CFURLRef>::seize( result );
		}

	inline FSRef LSOpenFromRefSpec ( const LSLaunchFSRefSpec &inLaunchSpec ) {
		(void) LaunchServicesErrorsRegistrationDependency();
		FSRef	result;
		ThrowOSStatus ( ::LSOpenFromRefSpec ( &inLaunchSpec, &result ));
		return result;
		}

	inline nucleus::owned<CFURLRef> LSOpenFromURLSpec ( const LSLaunchURLSpec &inLaunchSpec ) {
		(void) LaunchServicesErrorsRegistrationDependency();
		CFURLRef	result;
		ThrowOSStatus ( ::LSOpenFromURLSpec ( &inLaunchSpec, &result ));
  	    return nucleus::owned<CFURLRef>::seize( result );
		}
	}
	
#endif /* NITROGEN_LSOPEN_HH */

