#ifndef NITROGEN_SECCERTIFICATESUPPORT_H
#define	NITROGEN_SECCERTIFICATESUPPORT_H

#if	!TARGET_RT_MAC_MACHO
#error "These routines are only directly callable from MachO"
#endif

#ifndef __SECCERTIFICATESUPPORT__
#include <SecurityHI/SecCertificateSupport.h>
#endif

//	These 3 files don't exist yet!
#ifndef NITROGEN_SECIDENTITY_H
#include "Nitrogen/SecIdentity.h"
#endif

#ifndef NITROGEN_SECTRUST_H
#include "Nitrogen/SecTrust.h"
#endif

namespace Nitrogen {
	void RegisterSecCertificateErrors ();
	
	inline Nucleus::Owned<SecIdentityRef> SecChooseIdentity ( CFStringRef displayInfo, CFArrayRef identities ) {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		SecIdentityRef	result;
		ThrowOSStatus ( ::SecChooseIdentity ( displayInfo, identities, &result ));
  	    return Nucleus::Owned<SecIdentityRef>::Seize( result );
		}

	inline void SecChooseIdentityAsSheet ( WindowRef parentWindow, EventTargetRef inTarget, CFStringRef displayInfo, CFArrayRef identities ) {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		ThrowOSStatus ( ::SecChooseIdentityAsSheet ( parentWindow, inTarget, displayInfo, identities ));
		}
	
	inline void SecDisplayCertificate ( SecCertificateRef certificate, CFArrayRef keychainList ) {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		ThrowOSStatus ( ::SecDisplayCertificate ( certificate, keychainList ));
		}

	inline void SecDisplayCertificateGroup ( const CSSM_CERTGROUP *certificates, CFArrayRef keychainList ) {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		ThrowOSStatus ( ::SecDisplayCertificateGroup ( certificates, keychainList ));
		}

	inline void SecEditTrust ( CFStringRef displayInfo, SecTrustRef trust ) {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		ThrowOSStatus ( ::SecEditTrust ( displayInfo, trust ));
		}

	inline void SecEditTrustAsSheet ( WindowRef parentWindow, EventTargetRef inTarget, CFStringRef displayInfo, SecTrustRef trust ) {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		ThrowOSStatus ( ::SecEditTrustAsSheet ( parentWindow, inTarget, displayInfo, trust ));
		}

	}

#endif



