// Nitrogen/LSOpen.h
// -----------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2006 by Marshall Clow and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_LSOPEN_H
#define NITROGEN_LSOPEN_H

#ifndef __MACH__
#error "These routines are only directly callable from MachO"
#endif

#ifndef __LSOPEN__
#include <ApplicationServices/ApplicationServices.h>
#endif

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

#ifndef NITROGEN_CFURL_H
#include "Nitrogen/CFURL.h"
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

	inline Nucleus::Owned<CFURLRef> LSOpenCFURLRef ( CFURLRef inURL ) {
		(void) LaunchServicesErrorsRegistrationDependency();
		CFURLRef	result;
		ThrowOSStatus ( ::LSOpenCFURLRef ( inURL, &result ));
  	    return Nucleus::Owned<CFURLRef>::Seize( result );
		}

	inline FSRef LSOpenFromRefSpec ( const LSLaunchFSRefSpec &inLaunchSpec ) {
		(void) LaunchServicesErrorsRegistrationDependency();
		FSRef	result;
		ThrowOSStatus ( ::LSOpenFromRefSpec ( &inLaunchSpec, &result ));
		return result;
		}

	inline Nucleus::Owned<CFURLRef> LSOpenFromURLSpec ( const LSLaunchURLSpec &inLaunchSpec ) {
		(void) LaunchServicesErrorsRegistrationDependency();
		CFURLRef	result;
		ThrowOSStatus ( ::LSOpenFromURLSpec ( &inLaunchSpec, &result ));
  	    return Nucleus::Owned<CFURLRef>::Seize( result );
		}
	}
	
#endif /* NITROGEN_LSOPEN_H */

