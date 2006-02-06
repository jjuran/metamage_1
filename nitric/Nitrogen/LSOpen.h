#ifndef NITROGEN_LSOPEN_H
#define NITROGEN_LSOPEN_H

#if	!TARGET_RT_MAC_MACHO
#error "These routines are only directly callable from MachO"
#endif

#ifndef __LSOPEN__
#include <LaunchServices/LSOpen.h>
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

#ifndef NUCLEUS_ONLYONCE_H
#include "Nucleus/OnlyOnce.h"
#endif

namespace Nitrogen {
	void RegisterLSOpenErrors ();
	
/* ================================================================== */
/* LaunchServices API                                                                                       */
/* ================================================================== */

	inline FSRef LSOpenFSRef ( const FSRef &inRef ) {
		Nucleus::OnlyOnce<RegisterLSOpenErrors>();
		FSRef	result;
		ThrowOSStatus ( ::LSOpenFSRef ( &inRef, &result ));
		return result;
		}

	inline Nucleus::Owned<CFURLRef> LSOpenCFURLRef ( CFURLRef inURL ) {
		Nucleus::OnlyOnce<RegisterLSOpenErrors>();
		CFURLRef	result;
		ThrowOSStatus ( ::LSOpenCFURLRef ( inURL, &result ));
  	    return Nucleus::Owned<CFURLRef>::Seize( result );
		}

	inline FSRef LSOpenFromRefSpec ( const LSLaunchFSRefSpec &inLaunchSpec ) {
		Nucleus::OnlyOnce<RegisterLSOpenErrors>();
		FSRef	result;
		ThrowOSStatus ( ::LSOpenFromRefSpec ( &inLaunchSpec, &result ));
		return result;
		}

	inline Nucleus::Owned<CFURLRef> LSOpenFromURLSpec ( const LSLaunchURLSpec &inLaunchSpec ) {
		Nucleus::OnlyOnce<RegisterLSOpenErrors>();
		CFURLRef	result;
		ThrowOSStatus ( ::LSOpenFromURLSpec ( &inLaunchSpec, &result ));
  	    return Nucleus::Owned<CFURLRef>::Seize( result );
		}
	}
	
#endif /* NITROGEN_LSOPEN_H */

