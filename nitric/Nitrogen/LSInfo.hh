// Nitrogen/LSInfo.hh
// ------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004 by Marshall Clow.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_LSINFO_HH
#define NITROGEN_LSINFO_HH

#ifndef __MACH__
#error "These routines are only directly callable from MachO"
#endif


#ifndef __LSINFO__
#include "LaunchServices/LSInfo.h"
#endif


// #ifndef __CORESERVICES__
// #include <CoreServices/CoreServices.h>
// #endif


// Nitrogen
#ifndef MAC_FILES_TYPES_FSCREATOR_HH
#include "Mac/Files/Types/FSCreator.hh"
#endif
#ifndef MAC_FILES_TYPES_FSTYPE_HH
#include "Mac/Files/Types/FSType.hh"
#endif

#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
#endif

#ifndef NITROGEN_CFURL_HH
#include "Nitrogen/CFURL.hh"
#endif

namespace Nitrogen {

//	LSInit and LSTerm are deprecated, so we don't include it here

//	There are ownership issues here - if we return a extension, the user has to release it
	inline LSItemInfoRecord LSCopyItemInfoForRef ( const FSRef &inRef, LSRequestedInfo inWhichInfo ) {
		LSItemInfoRecord retVal;
		ThrowOSStatus ( ::LSCopyItemInfoForRef ( &inRef, inWhichInfo, &retVal ));
		return retVal;
		}

//	There are ownership issues here - if we return a extension, the user has to release it
	inline LSItemInfoRecord LSCopyItemInfoForURL ( CFURLRef inURL, LSRequestedInfo inWhichInfo ) {
		LSItemInfoRecord retVal;
		ThrowOSStatus ( ::LSCopyItemInfoForURL ( inURL, inWhichInfo, &retVal ));
		return retVal;
		}

/*    outExtStartIndex:
 *      On success, the starting index of the extension if there is one
 *      (not including the period). Set to kLSInvalidExtensionIndex if
 *      inNameBuffer does not contain a valid extension.
 */
 //	should we throw if retVal == kLSInvalidExtensionIndex ??
	inline UniCharCount LSGetExtensionInfo ( UniCharCount inNameLen, const UniChar inNameBuffer[] ) {
		UniCharCount retVal;
		ThrowOSStatus ( ::LSGetExtensionInfo ( inNameLen, inNameBuffer, &retVal ));
		return retVal;
		}
	
	inline nucleus::owned<CFStringRef> LSCopyDisplayNameForRef ( const FSRef &inRef ) {
		CFStringRef result;
		ThrowOSStatus ( ::LSCopyDisplayNameForRef ( &inRef, &result ));
		return nucleus::owned<CFStringRef>::seize ( result );
		}
	
	inline nucleus::owned<CFStringRef> LSCopyDisplayNameForURL ( CFURLRef inURL ) {
		CFStringRef result;
		ThrowOSStatus ( ::LSCopyDisplayNameForURL ( inURL, &result ));
		return nucleus::owned<CFStringRef>::seize ( result );
		}

	inline void LSSetExtensionHiddenForRef ( const FSRef &inRef, Boolean inHide ) {
		ThrowOSStatus ( ::LSSetExtensionHiddenForRef ( &inRef, inHide ));
		}
	
	inline void LSSetExtensionHiddenForURL ( CFURLRef inURL, Boolean inHide ) {
		ThrowOSStatus ( ::LSSetExtensionHiddenForURL ( inURL, inHide ));
		}
	
	inline nucleus::owned<CFStringRef> LSCopyKindStringForRef ( const FSRef &inRef ) {
		CFStringRef result;
		ThrowOSStatus ( ::LSCopyKindStringForRef ( &inRef, &result ));
		return nucleus::owned<CFStringRef>::seize ( result );
		}
	
	inline nucleus::owned<CFStringRef> LSCopyKindStringForURL ( CFURLRef inURL ) {
		CFStringRef result;
		ThrowOSStatus ( ::LSCopyKindStringForURL ( inURL, &result ));
		return nucleus::owned<CFStringRef>::seize ( result );
		}

	inline nucleus::owned<CFStringRef> LSCopyKindStringForTypeInfo ( Mac::FSType inType, Mac::FSCreator inCreator, CFStringRef inExtension ) {
		CFStringRef result;
		ThrowOSStatus ( ::LSCopyKindStringForTypeInfo ( inType, inCreator, inExtension, &result ));
		return nucleus::owned<CFStringRef>::seize ( result );
		}
	
//	Helpful special cases
	inline nucleus::owned<CFStringRef> LSCopyKindStringForTypeInfo ( Mac::FSType inType, Mac::FSCreator inCreator ) {
		CFStringRef result;
		ThrowOSStatus ( ::LSCopyKindStringForTypeInfo ( inType, inCreator, NULL, &result ));
		return nucleus::owned<CFStringRef>::seize ( result );
		}

	inline nucleus::owned<CFStringRef> LSCopyKindStringForTypeInfo ( CFStringRef inExtension ) {
		CFStringRef result;
		ThrowOSStatus ( ::LSCopyKindStringForTypeInfo ( kLSUnknownType, kLSUnknownCreator, inExtension, &result ));
		return nucleus::owned<CFStringRef>::seize ( result );
		}

	inline nucleus::owned<CFStringRef> LSCopyKindStringForMIMEType ( CFStringRef inMIMEType ) {
		CFStringRef result;
		ThrowOSStatus ( ::LSCopyKindStringForMIMEType ( inMIMEType, &result ));
		return nucleus::owned<CFStringRef>::seize ( result );
		}


	template <typename T>
	T LSGetApplicationForItem ( const FSRef &inItemRef, LSRolesMask inRoleMask );
	
//	template <> FSRef			LSGetApplicationForItem < FSRef > 			( const FSRef &inItemRef, LSRolesMask inRoleMask );
//	template <> nucleus::owned<CFURLRef> LSGetApplicationForItem < nucleus::owned<CFURLRef> > ( const FSRef &inItemRef, LSRolesMask inRoleMask );

	template <> inline FSRef LSGetApplicationForItem < FSRef > ( const FSRef &inItemRef, LSRolesMask inRoleMask ) {
		FSRef retVal;
		ThrowOSStatus ( ::LSGetApplicationForItem ( &inItemRef, inRoleMask, &retVal, NULL ));
		return retVal;		
		}
	
	template <> inline nucleus::owned<CFURLRef> LSGetApplicationForItem ( const FSRef &inItemRef, LSRolesMask inRoleMask ) {
		CFURLRef result;
		ThrowOSStatus ( ::LSGetApplicationForItem ( &inItemRef, inRoleMask, NULL, &result ));
		return nucleus::owned<CFURLRef>::seize ( result );
		}


	template <typename T>
	T LSGetApplicationForInfo ( Mac::FSType inType, Mac::FSCreator inCreator, LSRolesMask inRoleMask );
	
//	FSRef			LSGetApplicationForInfo < FSRef > 			( Mac::FSType inType, Mac::FSCreator inCreator, LSRolesMask inRoleMask );
//	nucleus::owned<CFURLRef> LSGetApplicationForInfo < nucleus::owned<CFURLRef> > ( Mac::FSType inType, Mac::FSCreator inCreator, LSRolesMask inRoleMask );

	template <> inline FSRef LSGetApplicationForInfo < FSRef > ( Mac::FSType inType, Mac::FSCreator inCreator, LSRolesMask inRoleMask ) {
		FSRef retVal;
		ThrowOSStatus ( ::LSGetApplicationForInfo ( inType, inCreator, NULL, inRoleMask, &retVal, NULL ));
		return retVal;
		}

	template <> inline nucleus::owned<CFURLRef> LSGetApplicationForInfo < nucleus::owned<CFURLRef> > ( Mac::FSType inType, Mac::FSCreator inCreator, LSRolesMask inRoleMask ) {
		CFURLRef result;
		ThrowOSStatus ( ::LSGetApplicationForInfo ( inType, inCreator, NULL, inRoleMask, NULL, &result ));
		return nucleus::owned<CFURLRef>::seize ( result );
		}


	template <typename T>
	T LSGetApplicationForInfo ( CFStringRef inExtension, LSRolesMask inRoleMask );
	
//	FSRef			LSGetApplicationForInfo < FSRef > 			( CFStringRef inExtension, LSRolesMask inRoleMask );
//	nucleus::owned<CFURLRef> LSGetApplicationForInfo < nucleus::owned<CFURLRef> > ( CFStringRef inExtension, LSRolesMask inRoleMask );

	template <> inline FSRef LSGetApplicationForInfo < FSRef > ( CFStringRef inExtension, LSRolesMask inRoleMask ) {
		FSRef retVal;
		ThrowOSStatus ( ::LSGetApplicationForInfo ( kLSUnknownType, kLSUnknownCreator, inExtension, inRoleMask, &retVal, NULL ));
		return retVal;
		}

	template <> inline nucleus::owned<CFURLRef> LSGetApplicationForInfo < nucleus::owned<CFURLRef> > ( CFStringRef inExtension, LSRolesMask inRoleMask ) {
		CFURLRef result;
		ThrowOSStatus ( ::LSGetApplicationForInfo ( kLSUnknownType, kLSUnknownCreator, inExtension, inRoleMask, NULL, &result ));
		return nucleus::owned<CFURLRef>::seize ( result );
		}

	inline nucleus::owned<CFURLRef> LSCopyApplicationForMIMEType ( CFStringRef inMIMEType, LSRolesMask inRoleMask ) {
		CFURLRef result;
		ThrowOSStatus ( ::LSCopyApplicationForMIMEType ( inMIMEType, inRoleMask, &result ));
		return nucleus::owned<CFURLRef>::seize ( result );
		}


	template <typename T>
	T LSGetApplicationForURL ( CFURLRef inURL, LSRolesMask inRoleMask );
	
//	FSRef			LSGetApplicationForURL < FSRef > 		   ( CFURLRef inURL, LSRolesMask inRoleMask );
//	nucleus::owned<CFURLRef> LSGetApplicationForURL < nucleus::owned<CFURLRef> > ( CFURLRef inURL, LSRolesMask inRoleMask );

	template <> inline FSRef LSGetApplicationForURL < FSRef > ( CFURLRef inURL, LSRolesMask inRoleMask ) {
		FSRef retVal;
		ThrowOSStatus ( ::LSGetApplicationForURL ( inURL, inRoleMask, &retVal, NULL ));
		return retVal;		
		}
	
	template <> inline nucleus::owned<CFURLRef> LSGetApplicationForURL < nucleus::owned<CFURLRef> > ( CFURLRef inURL, LSRolesMask inRoleMask ) {
		CFURLRef result;
		ThrowOSStatus ( ::LSGetApplicationForURL ( inURL, inRoleMask, NULL, &result ));
		return nucleus::owned<CFURLRef>::seize ( result );
		}
	

#if 0
//	We don't have the general case here - returns two flavors of the same data
//	Special cases of the two return values
	inline FSRef LSGetApplicationForURL ( CFURLRef inURL, LSRolesMask inRoleMask ) {
		FSRef retVal;
		ThrowOSStatus ( ::LSGetApplicationForURL ( inURL, inRoleMask, &retVal, NULL ));
		return retVal;		
		}
	
	inline nucleus::owned<CFURLRef> LSGetApplicationForURL ( CFURLRef inURL, LSRolesMask inRoleMask ) {
		CFURLRef result;
		ThrowOSStatus ( ::LSGetApplicationForURL ( inURL, inRoleMask, NULL, &result ));
		return nucleus::owned<CFURLRef>::seize ( result );
		}
#endif


	template <typename T>
	T LSFindApplicationForInfo ( Mac::FSCreator inCreator, CFStringRef inBundleID, CFStringRef inName );
	
//	FSRef			LSFindApplicationForInfo < FSRef > 			 ( Mac::FSCreator inCreator, CFStringRef inBundleID, CFStringRef inName );
//	nucleus::owned<CFURLRef> LSFindApplicationForInfo < nucleus::owned<CFURLRef> > ( Mac::FSCreator inCreator, CFStringRef inBundleID, CFStringRef inName );

	template <> inline FSRef LSFindApplicationForInfo < FSRef > ( Mac::FSCreator inCreator, CFStringRef inBundleID, CFStringRef inName ) {
		FSRef retVal;
		ThrowOSStatus ( ::LSFindApplicationForInfo ( inCreator, inBundleID, inName, &retVal, NULL ));
		return retVal;		
		}
	
	template <> inline nucleus::owned<CFURLRef> LSFindApplicationForInfo < nucleus::owned<CFURLRef> > ( Mac::FSCreator inCreator, CFStringRef inBundleID, CFStringRef inName ) {
		CFURLRef result;
		ThrowOSStatus ( ::LSFindApplicationForInfo ( inCreator, inBundleID, inName, NULL, &result ));
		return nucleus::owned<CFURLRef>::seize ( result );
		}


	inline Boolean LSCanRefAcceptItem ( const FSRef &inItem, const FSRef &inTarget, LSRolesMask inRoleMask, LSAcceptanceFlags inFlags = kLSAcceptDefault ) {
		::Boolean retVal;
		ThrowOSStatus ( ::LSCanRefAcceptItem ( &inItem, &inTarget, inRoleMask, inFlags, &retVal ));
		return retVal;
		}
			
	inline Boolean LSCanURLAcceptURL ( CFURLRef inItem, CFURLRef inTarget, LSRolesMask inRoleMask, LSAcceptanceFlags inFlags = kLSAcceptDefault ) {
		::Boolean retVal;
		ThrowOSStatus ( ::LSCanURLAcceptURL ( inItem, inTarget, inRoleMask, inFlags, &retVal ));
		return retVal;
		}
	
	inline void LSRegisterFSRef ( const FSRef &inRef, Boolean inUpdate ) {
		ThrowOSStatus ( ::LSRegisterFSRef ( &inRef, inUpdate ));
		}

	inline void LSRegisterURL ( CFURLRef inURL, Boolean inUpdate ) {
		ThrowOSStatus ( ::LSRegisterURL ( inURL, inUpdate ));
		}

	inline nucleus::owned<CFArrayRef> LSCopyApplicationURLsForURL ( CFURLRef inURL, LSRolesMask inRoleMask ) {
		return nucleus::owned<CFArrayRef>::seize ( ::LSCopyApplicationURLsForURL ( inURL, inRoleMask ));
		}
	}	


#endif
