// Nitrogen/SecCertificateSupport.hh
// ---------------------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004 by Marshall Clow.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_SECCERTIFICATESUPPORT_HH
#define NITROGEN_SECCERTIFICATESUPPORT_HH

#ifndef __MACH__
#error "These routines are only directly callable from MachO"
#endif

#ifndef __SECCERTIFICATESUPPORT__
#include <SecurityHI/SecCertificateSupport.h>
#endif

//	These 3 files don't exist yet!
#ifndef NITROGEN_SECIDENTITY_HH
#include "Nitrogen/SecIdentity.hh"
#endif

#ifndef NITROGEN_SECTRUST_HH
#include "Nitrogen/SecTrust.hh"
#endif

namespace Nitrogen {
	void RegisterSecCertificateErrors ();
	
	inline nucleus::owned<SecIdentityRef> SecChooseIdentity ( CFStringRef displayInfo, CFArrayRef identities ) {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		SecIdentityRef	result;
		ThrowOSStatus ( ::SecChooseIdentity ( displayInfo, identities, &result ));
  	    return nucleus::owned<SecIdentityRef>::seize( result );
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

