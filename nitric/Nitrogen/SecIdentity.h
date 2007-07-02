// Nitrogen/SecIdentity.h
// ----------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2006 by Marshall Clow.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_SECIDENTITY_H
#define	NITROGEN_SECIDENTITY_H

#if	!TARGET_RT_MAC_MACHO
#error "These routines are only directly callable from MachO"
#endif

#ifndef _SECURITY_SECIDENTITY_H_
#include <Security/SecIdentity.h>
#endif

#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

#ifndef NUCLEUS_ONLYONCE_H
#include "Nucleus/OnlyOnce.h"
#endif



namespace Nitrogen {
	void RegisterSecCertificateErrors ();
	
	using ::SecIdentityRef;
	using ::SecCertificateRef;
	using ::SecKeyRef;
  }

namespace Nucleus
  {
   // Should these go through CFTypeRef's disposer?
   
   template <> struct Disposer< Nitrogen::SecIdentityRef >: public std::unary_function< Nitrogen::SecIdentityRef, void >
     {
      void operator()( Nitrogen::SecIdentityRef kc ) const
        {
         (void) ::CFRelease ( kc );
        }
     };

   template <> struct Disposer< Nitrogen::SecCertificateRef >: public std::unary_function< Nitrogen::SecCertificateRef, void >
     {
      void operator()( Nitrogen::SecCertificateRef kci ) const
        {
         (void) ::CFRelease ( kci );
        }
     };

   template <> struct Disposer< Nitrogen::SecKeyRef >: public std::unary_function< Nitrogen::SecKeyRef, void >
     {
      void operator()( Nitrogen::SecKeyRef kcs ) const
        {
         (void) ::CFRelease ( kcs );
        }
     };
  }

namespace Nitrogen
  {


//	CFTypeID SecIdentityGetTypeID(void);
	using ::SecIdentityGetTypeID;
	
	inline void CFShow ( SecIdentityRef ir )	{ ::CFShow ( ir ); }
	inline void CFShow ( SecCertificateRef cr )	{ ::CFShow ( cr ); }
	inline void CFShow ( SecKeyRef kr )			{ ::CFShow ( kr ); }

	inline Nucleus::Owned<SecCertificateRef> SecIdentityCopyCertificate ( SecIdentityRef identityRef ) {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		SecCertificateRef	result;
		ThrowOSStatus ( ::SecIdentityCopyCertificate ( identityRef, &result ));
  	    return Nucleus::Owned<SecCertificateRef>::Seize( result );
		}

	inline Nucleus::Owned<SecKeyRef> SecIdentityCopyPrivateKey ( SecIdentityRef identityRef ) {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		SecKeyRef	result;
		ThrowOSStatus ( ::SecIdentityCopyPrivateKey ( identityRef, &result ));
  	    return Nucleus::Owned<SecKeyRef>::Seize( result );
		}

	}

#endif
