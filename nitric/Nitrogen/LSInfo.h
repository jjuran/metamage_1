// LSInfo.h

#ifndef	NITROGEN_LSINFO_H
#define	NITROGEN_LSINFO_H

#if	!TARGET_RT_MAC_MACHO
#error "These routines are only directly callable from MachO"
#endif


#ifndef __LSINFO__
#include "LaunchServices/LSInfo.h"
#endif


// #ifndef __CORESERVICES__
// #include <CoreServices/CoreServices.h>
// #endif


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
	
	inline Nucleus::Owned<CFStringRef> LSCopyDisplayNameForRef ( const FSRef &inRef ) {
		CFStringRef result;
		ThrowOSStatus ( ::LSCopyDisplayNameForRef ( &inRef, &result ));
		return Nucleus::Owned<CFStringRef>::Seize ( result );
		}
	
	inline Nucleus::Owned<CFStringRef> LSCopyDisplayNameForURL ( CFURLRef inURL ) {
		CFStringRef result;
		ThrowOSStatus ( ::LSCopyDisplayNameForURL ( inURL, &result ));
		return Nucleus::Owned<CFStringRef>::Seize ( result );
		}

	inline void LSSetExtensionHiddenForRef ( const FSRef &inRef, Boolean inHide ) {
		ThrowOSStatus ( ::LSSetExtensionHiddenForRef ( &inRef, inHide ));
		}
	
	inline void LSSetExtensionHiddenForURL ( CFURLRef inURL, Boolean inHide ) {
		ThrowOSStatus ( ::LSSetExtensionHiddenForURL ( inURL, inHide ));
		}
	
	inline Nucleus::Owned<CFStringRef> LSCopyKindStringForRef ( const FSRef &inRef ) {
		CFStringRef result;
		ThrowOSStatus ( ::LSCopyKindStringForRef ( &inRef, &result ));
		return Nucleus::Owned<CFStringRef>::Seize ( result );
		}
	
	inline Nucleus::Owned<CFStringRef> LSCopyKindStringForURL ( CFURLRef inURL ) {
		CFStringRef result;
		ThrowOSStatus ( ::LSCopyKindStringForURL ( inURL, &result ));
		return Nucleus::Owned<CFStringRef>::Seize ( result );
		}

	inline Nucleus::Owned<CFStringRef> LSCopyKindStringForTypeInfo ( OSType inType, OSType inCreator, CFStringRef inExtension ) {
		CFStringRef result;
		ThrowOSStatus ( ::LSCopyKindStringForTypeInfo ( inType, inCreator, inExtension, &result ));
		return Nucleus::Owned<CFStringRef>::Seize ( result );
		}
	
//	Helpful special cases
	inline Nucleus::Owned<CFStringRef> LSCopyKindStringForTypeInfo ( OSType inType, OSType inCreator ) {
		CFStringRef result;
		ThrowOSStatus ( ::LSCopyKindStringForTypeInfo ( inType, inCreator, NULL, &result ));
		return Nucleus::Owned<CFStringRef>::Seize ( result );
		}

	inline Nucleus::Owned<CFStringRef> LSCopyKindStringForTypeInfo ( CFStringRef inExtension ) {
		CFStringRef result;
		ThrowOSStatus ( ::LSCopyKindStringForTypeInfo ( kLSUnknownType, kLSUnknownCreator, inExtension, &result ));
		return Nucleus::Owned<CFStringRef>::Seize ( result );
		}

	inline Nucleus::Owned<CFStringRef> LSCopyKindStringForMIMEType ( CFStringRef inMIMEType ) {
		CFStringRef result;
		ThrowOSStatus ( ::LSCopyKindStringForMIMEType ( inMIMEType, &result ));
		return Nucleus::Owned<CFStringRef>::Seize ( result );
		}


	template <typename T>
	T LSGetApplicationForItem ( const FSRef &inItemRef, LSRolesMask inRoleMask );
	
//	template <> FSRef			LSGetApplicationForItem < FSRef > 			( const FSRef &inItemRef, LSRolesMask inRoleMask );
//	template <> Nucleus::Owned<CFURLRef> LSGetApplicationForItem < Nucleus::Owned<CFURLRef> > ( const FSRef &inItemRef, LSRolesMask inRoleMask );

	template <> inline FSRef LSGetApplicationForItem < FSRef > ( const FSRef &inItemRef, LSRolesMask inRoleMask ) {
		FSRef retVal;
		ThrowOSStatus ( ::LSGetApplicationForItem ( &inItemRef, inRoleMask, &retVal, NULL ));
		return retVal;		
		}
	
	template <> inline Nucleus::Owned<CFURLRef> LSGetApplicationForItem ( const FSRef &inItemRef, LSRolesMask inRoleMask ) {
		CFURLRef result;
		ThrowOSStatus ( ::LSGetApplicationForItem ( &inItemRef, inRoleMask, NULL, &result ));
		return Nucleus::Owned<CFURLRef>::Seize ( result );
		}


	template <typename T>
	T LSGetApplicationForInfo ( OSType inType, OSType inCreator, LSRolesMask inRoleMask );
	
//	FSRef			LSGetApplicationForInfo < FSRef > 			( OSType inType, OSType inCreator, LSRolesMask inRoleMask );
//	Nucleus::Owned<CFURLRef> LSGetApplicationForInfo < Nucleus::Owned<CFURLRef> > ( OSType inType, OSType inCreator, LSRolesMask inRoleMask );

	template <> inline FSRef LSGetApplicationForInfo < FSRef > ( OSType inType, OSType inCreator, LSRolesMask inRoleMask ) {
		FSRef retVal;
		ThrowOSStatus ( ::LSGetApplicationForInfo ( inType, inCreator, NULL, inRoleMask, &retVal, NULL ));
		return retVal;
		}

	template <> inline Nucleus::Owned<CFURLRef> LSGetApplicationForInfo < Nucleus::Owned<CFURLRef> > ( OSType inType, OSType inCreator, LSRolesMask inRoleMask ) {
		CFURLRef result;
		ThrowOSStatus ( ::LSGetApplicationForInfo ( inType, inCreator, NULL, inRoleMask, NULL, &result ));
		return Nucleus::Owned<CFURLRef>::Seize ( result );
		}


	template <typename T>
	T LSGetApplicationForInfo ( CFStringRef inExtension, LSRolesMask inRoleMask );
	
//	FSRef			LSGetApplicationForInfo < FSRef > 			( CFStringRef inExtension, LSRolesMask inRoleMask );
//	Nucleus::Owned<CFURLRef> LSGetApplicationForInfo < Nucleus::Owned<CFURLRef> > ( CFStringRef inExtension, LSRolesMask inRoleMask );

	template <> inline FSRef LSGetApplicationForInfo < FSRef > ( CFStringRef inExtension, LSRolesMask inRoleMask ) {
		FSRef retVal;
		ThrowOSStatus ( ::LSGetApplicationForInfo ( kLSUnknownType, kLSUnknownCreator, inExtension, inRoleMask, &retVal, NULL ));
		return retVal;
		}

	template <> inline Nucleus::Owned<CFURLRef> LSGetApplicationForInfo < Nucleus::Owned<CFURLRef> > ( CFStringRef inExtension, LSRolesMask inRoleMask ) {
		CFURLRef result;
		ThrowOSStatus ( ::LSGetApplicationForInfo ( kLSUnknownType, kLSUnknownCreator, inExtension, inRoleMask, NULL, &result ));
		return Nucleus::Owned<CFURLRef>::Seize ( result );
		}

	inline Nucleus::Owned<CFURLRef> LSCopyApplicationForMIMEType ( CFStringRef inMIMEType, LSRolesMask inRoleMask ) {
		CFURLRef result;
		ThrowOSStatus ( ::LSCopyApplicationForMIMEType ( inMIMEType, inRoleMask, &result ));
		return Nucleus::Owned<CFURLRef>::Seize ( result );
		}


	template <typename T>
	T LSGetApplicationForURL ( CFURLRef inURL, LSRolesMask inRoleMask );
	
//	FSRef			LSGetApplicationForURL < FSRef > 		   ( CFURLRef inURL, LSRolesMask inRoleMask );
//	Nucleus::Owned<CFURLRef> LSGetApplicationForURL < Nucleus::Owned<CFURLRef> > ( CFURLRef inURL, LSRolesMask inRoleMask );

	template <> inline FSRef LSGetApplicationForURL < FSRef > ( CFURLRef inURL, LSRolesMask inRoleMask ) {
		FSRef retVal;
		ThrowOSStatus ( ::LSGetApplicationForURL ( inURL, inRoleMask, &retVal, NULL ));
		return retVal;		
		}
	
	template <> inline Nucleus::Owned<CFURLRef> LSGetApplicationForURL < Nucleus::Owned<CFURLRef> > ( CFURLRef inURL, LSRolesMask inRoleMask ) {
		CFURLRef result;
		ThrowOSStatus ( ::LSGetApplicationForURL ( inURL, inRoleMask, NULL, &result ));
		return Nucleus::Owned<CFURLRef>::Seize ( result );
		}
	

#if 0
//	We don't have the general case here - returns two flavors of the same data
//	Special cases of the two return values
	inline FSRef LSGetApplicationForURL ( CFURLRef inURL, LSRolesMask inRoleMask ) {
		FSRef retVal;
		ThrowOSStatus ( ::LSGetApplicationForURL ( inURL, inRoleMask, &retVal, NULL ));
		return retVal;		
		}
	
	inline Nucleus::Owned<CFURLRef> LSGetApplicationForURL ( CFURLRef inURL, LSRolesMask inRoleMask ) {
		CFURLRef result;
		ThrowOSStatus ( ::LSGetApplicationForURL ( inURL, inRoleMask, NULL, &result ));
		return Nucleus::Owned<CFURLRef>::Seize ( result );
		}
#endif


	template <typename T>
	T LSFindApplicationForInfo ( OSType inCreator, CFStringRef inBundleID, CFStringRef inName );
	
//	FSRef			LSFindApplicationForInfo < FSRef > 			 ( OSType inCreator, CFStringRef inBundleID, CFStringRef inName );
//	Nucleus::Owned<CFURLRef> LSFindApplicationForInfo < Nucleus::Owned<CFURLRef> > ( OSType inCreator, CFStringRef inBundleID, CFStringRef inName );

	template <> inline FSRef LSFindApplicationForInfo < FSRef > ( OSType inCreator, CFStringRef inBundleID, CFStringRef inName ) {
		FSRef retVal;
		ThrowOSStatus ( ::LSFindApplicationForInfo ( inCreator, inBundleID, inName, &retVal, NULL ));
		return retVal;		
		}
	
	template <> inline Nucleus::Owned<CFURLRef> LSFindApplicationForInfo < Nucleus::Owned<CFURLRef> > ( OSType inCreator, CFStringRef inBundleID, CFStringRef inName ) {
		CFURLRef result;
		ThrowOSStatus ( ::LSFindApplicationForInfo ( inCreator, inBundleID, inName, NULL, &result ));
		return Nucleus::Owned<CFURLRef>::Seize ( result );
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

	inline Nucleus::Owned<CFArrayRef> LSCopyApplicationURLsForURL ( CFURLRef inURL, LSRolesMask inRoleMask ) {
		return Nucleus::Owned<CFArrayRef>::Seize ( ::LSCopyApplicationURLsForURL ( inURL, inRoleMask ));
		}
	}	


#endif
