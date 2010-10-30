// Nitrogen/UTType.hh
// ------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2007 by Marshall Clow and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_UTTYPE_HH
#define NITROGEN_UTTYPE_HH

#ifndef __MACH__
#error "These routines are only directly callable from MachO"
#endif

#ifndef __UTTYPE__
#include "LaunchServices/UTType.h"
#endif

// #ifndef __CORESERVICES__
// #include <CoreServices/CoreServices.h>
// #endif

// Nitrogen
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

	struct UTTypeCreatePreferredIdentifierForTag_Failed {};

	inline nucleus::owned<CFStringRef> UTTypeCreatePreferredIdentifierForTag ( CFStringRef inTagClass, 
				CFStringRef inTag, CFStringRef inConformingToTypeIdentifier = NULL ) {
		CFStringRef result = ::UTTypeCreatePreferredIdentifierForTag ( inTagClass, inTag, inConformingToTypeIdentifier );
//  Result:
//    a new CFStringRef containing the type identifier, or NULL if
//    inTagClass is not a known tag class
		if ( result == NULL )
			throw UTTypeCreatePreferredIdentifierForTag_Failed ();
		return nucleus::owned<CFStringRef>::seize ( result );
		}
	
	struct UTTypeCreateAllIdentifiersForTag_Failed {};
	
	inline nucleus::owned<CFArrayRef> UTTypeCreateAllIdentifiersForTag ( CFStringRef inTagClass, 
				CFStringRef inTag, CFStringRef inConformingToTypeIdentifier = NULL ) {
		CFArrayRef result = ::UTTypeCreateAllIdentifiersForTag ( inTagClass, inTag, inConformingToTypeIdentifier );
//  Result:
//    An array of uniform type identifiers, or NULL if inTagClass is
//    not a known tag class
		if ( result == NULL )
			throw UTTypeCreateAllIdentifiersForTag_Failed ();
		return nucleus::owned<CFArrayRef>::seize ( result );
		}

	struct UTTypeCopyPreferredTagWithClass_Failed {};
	
	inline nucleus::owned<CFStringRef> UTTypeCopyPreferredTagWithClass ( CFStringRef inTypeIdentifier, CFStringRef inTagClass ) {
		CFStringRef result = ::UTTypeCopyPreferredTagWithClass ( inTypeIdentifier, inTagClass );
//  Result:
//    An array of uniform type identifiers, or NULL if inTagClass is
//    not a known tag class
		if ( result == NULL )
			throw UTTypeCopyPreferredTagWithClass_Failed ();
		return nucleus::owned<CFStringRef>::seize ( result );
		}

//	extern Boolean UTTypeEqual ( CFStringRef inTypeIdentifier1, CFStringRef inTypeIdentifier2 );
	using ::UTTypeEqual;

//	extern Boolean UTTypeConformsTo ( CFStringRef inTypeIdentifier, CFStringRef inConformsToTypeIdentifier );
	using ::UTTypeConformsTo;


	struct UTTypeCopyDescription_Failed {};
	
	inline nucleus::owned<CFStringRef> UTTypeCopyDescription ( CFStringRef inTypeIdentifier ) {
		CFStringRef result = ::UTTypeCopyDescription ( inTypeIdentifier );
//  Result:
//    a localized string, or NULL of no type description is available
		if ( result == NULL )
			throw UTTypeCopyDescription_Failed ();
		return nucleus::owned<CFStringRef>::seize ( result );
		}

	struct UTTypeCopyDeclaration_Failed {};
	
	inline nucleus::owned<CFDictionaryRef> UTTypeCopyDeclaration ( CFStringRef inTypeIdentifier ) {
		CFDictionaryRef result = ::UTTypeCopyDeclaration ( inTypeIdentifier );
//  Result:
//    a tag declaration dictionary, or NULL if the type is not declared
		if ( result == NULL )
			throw UTTypeCopyDeclaration_Failed ();
		return nucleus::owned<CFDictionaryRef>::seize ( result );
		}

	struct UTTypeCopyDeclaringBundleURL_Failed {};
	
	inline nucleus::owned<CFURLRef> UTTypeCopyDeclaringBundleURL ( CFStringRef inTypeIdentifier ) {
		CFURLRef result = ::UTTypeCopyDeclaringBundleURL ( inTypeIdentifier );
//  Result:
//    a URL, or NULL if the bundle cannot be located.
		if ( result == NULL )
			throw UTTypeCopyDeclaringBundleURL_Failed ();
		return nucleus::owned<CFURLRef>::seize ( result );
		}


	struct UTCreateStringForOSType_Failed {};
	
	inline nucleus::owned<CFStringRef> UTCreateStringForOSType ( Mac::FSType inOSType ) {
		CFStringRef result = ::UTCreateStringForOSType ( inOSType );
//  Result:
//    a new CFString representing the OSType, or NULL if the argument
//    is 0 or '????'
		if ( result == NULL )
			throw UTCreateStringForOSType_Failed ();
		return nucleus::owned<CFStringRef>::seize ( result );
		}

	struct UTGetOSTypeFromString_Failed {};

	inline Mac::FSType UTGetOSTypeFromString ( CFStringRef inString ) {
		Mac::FSType retVal = Mac::FSType( ::UTGetOSTypeFromString ( inString ) );
//  Result:
//    the OSType value encoded in the string, or 0 if the string is not
//    a valid encoding of an OSType
		if ( retVal == Mac::FSType() )
			throw UTGetOSTypeFromString_Failed ();
		return retVal;
		}
	
	}
	
#endif

