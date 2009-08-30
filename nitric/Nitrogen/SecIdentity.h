#ifndef NITROGEN_SECIDENTITY_H
#define	NITROGEN_SECIDENTITY_H

#if	!TARGET_RT_MAC_MACHO
#error "These routines are only directly callable from MachO"
#endif

#ifndef _SECURITY_SECIDENTITY_H_
#include <Security/SecIdentity.h>
#endif

#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif

#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif



namespace Nitrogen {
	void RegisterSecurityErrors ();
	
   template <> struct Disposer< SecIdentityRef >: public std::unary_function< SecIdentityRef, void >
     {
      void operator()( SecIdentityRef kc ) const
        {
         (void) ::CFRelease ( kc );
        }
     };

   template <> struct Disposer< SecCertificateRef >: public std::unary_function< SecCertificateRef, void >
     {
      void operator()( SecCertificateRef kci ) const
        {
         (void) ::CFRelease ( kci );
        }
     };

   template <> struct Disposer< SecKeyRef >: public std::unary_function< SecKeyRef, void >
     {
      void operator()( SecKeyRef kcs ) const
        {
         (void) ::CFRelease ( kcs );
        }
     };


//	CFTypeID SecIdentityGetTypeID(void);
	using ::SecIdentityGetTypeID;
	
	inline void CFShow ( SecIdentityRef ir )	{ ::CFShow ( ir ); }
	inline void CFShow ( SecCertificateRef cr )	{ ::CFShow ( cr ); }
	inline void CFShow ( SecKeyRef kr )			{ ::CFShow ( kr ); }

	inline Owned<SecCertificateRef> SecIdentityCopyCertificate ( SecIdentityRef identityRef ) {
		OnlyOnce<RegisterSecCertificateErrors>();
		SecCertificateRef	result;
		ThrowOSStatus ( ::SecIdentityCopyCertificate ( identityRef, &result ));
  	    return Owned<SecCertificateRef>::Seize( result );
		}

	inline Owned<SecKeyRef> SecIdentityCopyPrivateKey ( SecIdentityRef identityRef ) {
		OnlyOnce<RegisterSecCertificateErrors>();
		SecKeyRef	result;
		ThrowOSStatus ( ::SecIdentityCopyPrivateKey ( identityRef, &result ));
  	    return Owned<SecKeyRef>::Seize( result );
		}

	}

#endif
