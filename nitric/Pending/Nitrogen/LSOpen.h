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

#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif

#ifndef NITROGEN_CFURL_H
#include "Nitrogen/CFURL.h"
#endif

#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif

namespace Nitrogen {
	void RegisterLSOpenErrors ();
	
/* ================================================================== */
/* LaunchServices API                                                                                       */
/* ================================================================== */

	inline FSRef LSOpenFSRef ( const FSRef &inRef ) {
		OnlyOnce<RegisterLSOpenErrors>();
		FSRef	result;
		ThrowOSStatus ( ::LSOpenFSRef ( &inRef, &result ));
		return result;
		}

	inline Owned<CFURLRef> LSOpenCFURLRef ( CFURLRef inURL ) {
		OnlyOnce<RegisterLSOpenErrors>();
		CFURLRef	result;
		ThrowOSStatus ( ::LSOpenCFURLRef ( inURL, &result ));
  	    return Owned<CFURLRef>::Seize( result );
		}

	inline FSRef LSOpenFromRefSpec ( const LSLaunchFSRefSpec &inLaunchSpec ) {
		OnlyOnce<RegisterLSOpenErrors>();
		FSRef	result;
		ThrowOSStatus ( ::LSOpenFromRefSpec ( &inLaunchSpec, &result ));
		return result;
		}

	inline Owned<CFURLRef> LSOpenFromURLSpec ( const LSLaunchURLSpec &inLaunchSpec ) {
		OnlyOnce<RegisterLSOpenErrors>();
		CFURLRef	result;
		ThrowOSStatus ( ::LSOpenFromURLSpec ( &inLaunchSpec, &result ));
  	    return Owned<CFURLRef>::Seize( result );
		}
	}
	
#endif /* NITROGEN_LSOPEN_H */

