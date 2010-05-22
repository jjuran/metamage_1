// Nitrogen/SecIdentity.hh
// -----------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2006 by Marshall Clow.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_SECIDENTITY_HH
#define NITROGEN_SECIDENTITY_HH

#ifndef __MACH__
#error "These routines are only directly callable from MachO"
#endif

#ifndef _SECURITY_SECIDENTITY_H_
#include <Security/SecIdentity.h>
#endif

// nucleus
#include "nucleus/owned.hh"

#ifndef NUCLEUS_ONLYONCE_H
#include "Nucleus/OnlyOnce.h"
#endif



namespace Nitrogen {
	void RegisterSecCertificateErrors ();
	
	using ::SecIdentityRef;
	using ::SecCertificateRef;
	using ::SecKeyRef;
  }

namespace nucleus
  {
   // Should these go through CFTypeRef's disposer?
   
   template <> struct disposer< SecIdentityRef >
     {
      typedef SecIdentityRef  argument_type;
      typedef void            result_type;
      
      void operator()( SecIdentityRef kc ) const
        {
         (void) ::CFRelease ( kc );
        }
     };

   template <> struct disposer< SecCertificateRef >
     {
      typedef SecCertificateRef  argument_type;
      typedef void               result_type;
      
      void operator()( SecCertificateRef kci ) const
        {
         (void) ::CFRelease ( kci );
        }
     };

   template <> struct disposer< SecKeyRef >
     {
      typedef SecKeyRef  argument_type;
      typedef void       result_type;
      
      void operator()( SecKeyRef kcs ) const
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

	inline nucleus::owned<SecCertificateRef> SecIdentityCopyCertificate ( SecIdentityRef identityRef ) {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		SecCertificateRef	result;
		ThrowOSStatus ( ::SecIdentityCopyCertificate ( identityRef, &result ));
  	    return nucleus::owned<SecCertificateRef>::seize( result );
		}

	inline nucleus::owned<SecKeyRef> SecIdentityCopyPrivateKey ( SecIdentityRef identityRef ) {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		SecKeyRef	result;
		ThrowOSStatus ( ::SecIdentityCopyPrivateKey ( identityRef, &result ));
  	    return nucleus::owned<SecKeyRef>::seize( result );
		}

	}

#endif
