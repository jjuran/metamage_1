// Nitrogen/SecTrust.hh
// --------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2006 by Marshall Clow and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_SECTRUST_HH
#define NITROGEN_SECTRUST_HH

#ifndef _SECURITY_SECTRUST_H_
#include <Security/SecTrust.h>
#endif

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

#ifndef NUCLEUS_ONLYONCE_H
#include "Nucleus/OnlyOnce.h"
#endif


namespace Nitrogen {

	void RegisterSecCertificateErrors ();
	
	using ::SecTrustRef;

#if 0
/*!
	@typedef SecTrustResultType
	@abstract Specifies the trust result type.
	@constant kSecTrustResultInvalid Indicates an invalid setting or result.
	@constant kSecTrustResultProceed Indicates you may proceed.  This value may be returned by the SecTrustEvaluate function or stored as part of the user trust settings. 
	@constant kSecTrustResultConfirm Indicates confirmation with the user is required before proceeding.  This value may be returned by the SecTrustEvaluate function or stored as part of the user trust settings. 
	@constant kSecTrustResultDeny Indicates a user-configured deny; do not proceed. This value may be returned by the SecTrustEvaluate function or stored as part of the user trust settings. 
	@constant kSecTrustResultUnspecified Indicates user intent is unknown. This value may be returned by the SecTrustEvaluate function or stored as part of the user trust settings. 
	@constant kSecTrustResultRecoverableTrustFailure Indicates a trust framework failure; retry after fixing inputs. This value may be returned by the SecTrustEvaluate function but not stored as part of the user trust settings. 
	@constant kSecTrustResultFatalTrustFailure Indicates a trust framework failure; no "easy" fix. This value may be returned by the SecTrustEvaluate function but not stored as part of the user trust settings.
	@constant kSecTrustResultOtherError Indicates a failure other than that of trust evaluation. This value may be returned by the SecTrustEvaluate function but not stored as part of the user trust settings.
 */
typedef enum {
    kSecTrustResultInvalid,
    kSecTrustResultProceed,
    kSecTrustResultConfirm,
    kSecTrustResultDeny,
    kSecTrustResultUnspecified,
    kSecTrustResultRecoverableTrustFailure,
    kSecTrustResultFatalTrustFailure,
    kSecTrustResultOtherError
} SecTrustResultType;

/*!
	@typedef SecTrustUserSetting
	@abstract Specifies user-specified trust settings.
*/
typedef SecTrustResultType SecTrustUserSetting;
#endif

	using ::SecTrustRef;

  }

namespace nucleus
  {
   template <> struct disposer< SecTrustRef >
     {
      typedef SecTrustRef  argument_type;
      typedef void         result_type;
      
      void operator()( SecTrustRef kc ) const
        {
         (void) ::CFRelease ( kc );
        }
     };
  }

namespace Nitrogen
  {
//	CFTypeID SecTrustGetTypeID(void);
	using ::SecTrustGetTypeID;
	
	inline void CFShow ( SecTrustRef tr ) { ::CFShow ( tr ); }
	
	inline nucleus::owned<SecTrustRef> SecTrustCreateWithCertificates ( CFArrayRef certificates, CFTypeRef policies ) {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		SecTrustRef	result;
		ThrowOSStatus ( ::SecTrustCreateWithCertificates ( certificates, policies, &result ));
  	    return nucleus::owned<SecTrustRef>::seize( result );
		}

	inline void SecTrustSetParameters ( SecTrustRef trustRef, CSSM_TP_ACTION action, CFDataRef actionData ) {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		ThrowOSStatus ( ::SecTrustSetParameters ( trustRef, action, actionData ));
		}

	inline void SecTrustSetAnchorCertificates ( SecTrustRef trust, CFArrayRef anchorCertificates ) {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		ThrowOSStatus ( ::SecTrustSetAnchorCertificates ( trust, anchorCertificates ));
		}
	
	inline void SecTrustSetKeychains ( SecTrustRef trust, CFTypeRef keychainOrArray ) {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		ThrowOSStatus ( ::SecTrustSetKeychains ( trust, keychainOrArray ));
		}

	inline void SecTrustSetVerifyDate ( SecTrustRef trust, CFDateRef verifyDate ) {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		ThrowOSStatus ( ::SecTrustSetVerifyDate ( trust, verifyDate ));
		}

	inline SecTrustResultType SecTrustEvaluate ( SecTrustRef trust ) {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		::SecTrustResultType result;
		ThrowOSStatus ( ::SecTrustEvaluate ( trust, &result ));
		return result;
		}

	struct SecTrustGetResult_Result {
		SecTrustResultType			resultType;
		nucleus::owned<CFArrayRef>			certChain;
		CSSM_TP_APPLE_EVIDENCE_INFO	*statusChain;
		};
	
	inline SecTrustGetResult_Result SecTrustGetResult ( SecTrustRef trustRef ) {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		SecTrustGetResult_Result result;
		CFArrayRef arrRef;
		ThrowOSStatus ( ::SecTrustGetResult ( trustRef, &result.resultType, &arrRef, &result.statusChain ));
		result.certChain = nucleus::owned<CFArrayRef>::seize ( arrRef );
		return result;
		}
	
	inline CSSM_TP_VERIFY_CONTEXT_RESULT_PTR SecTrustGetCssmResult ( SecTrustRef trust ) {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		::CSSM_TP_VERIFY_CONTEXT_RESULT_PTR result;
		ThrowOSStatus ( ::SecTrustGetCssmResult ( trust, &result ));
		return result;
		}

	inline CSSM_TP_HANDLE SecTrustGetTPHandle ( SecTrustRef trust ) {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		::CSSM_TP_HANDLE result;
		ThrowOSStatus ( ::SecTrustGetTPHandle ( trust, &result ));
		return result;
		}

	inline nucleus::owned<CFArrayRef> SecTrustCopyAnchorCertificates () {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		CFArrayRef	result;
		ThrowOSStatus ( ::SecTrustCopyAnchorCertificates ( &result ));
  	    return nucleus::owned<CFArrayRef>::seize( result );
		}

	struct SecTrustGetCSSMAnchorCertificates_Result {
		uint32 cssmAnchorCount;
	//	This is a 'get' - we're not getting a copy
		const CSSM_DATA *cssmAnchors;
		};
	
	inline SecTrustGetCSSMAnchorCertificates_Result SecTrustGetCSSMAnchorCertificates () {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		SecTrustGetCSSMAnchorCertificates_Result result;
		ThrowOSStatus ( ::SecTrustGetCSSMAnchorCertificates ( &result.cssmAnchors, &result.cssmAnchorCount ));
		return result;
		}
		
	inline SecTrustUserSetting SecTrustGetUserTrust ( SecCertificateRef certificate, SecPolicyRef policy ) {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		::SecTrustUserSetting result;
		ThrowOSStatus ( ::SecTrustGetUserTrust ( certificate, policy, &result ));
		return result;
		}

	inline void SecTrustSetUserTrust ( SecCertificateRef certificate, SecPolicyRef policy, SecTrustUserSetting trustSetting ) {
		Nucleus::OnlyOnce<RegisterSecCertificateErrors>();
		ThrowOSStatus ( ::SecTrustSetUserTrust ( certificate, policy, trustSetting ));
		}
	}

#endif
