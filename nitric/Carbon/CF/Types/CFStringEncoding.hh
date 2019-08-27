/*
	Carbon/CF/Types/CFStringEncoding.hh
	-----------------------------------
*/

#ifndef CARBON_CF_TYPES_CFSTRINGENCODING_HH
#define CARBON_CF_TYPES_CFSTRINGENCODING_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

// Mac OS
#ifndef __COREFOUNDATION_CFSTRING__
#ifndef __CFSTRING__
#include <CFString.h>
#endif
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


#ifdef kCFStringEncodingInvalidId
#undef kCFStringEncodingInvalidId
const UInt32 kCFStringEncodingInvalidId = 0xffffffffU;
#endif


namespace Carbon
{
	
	enum CFStringEncoding
	{
		// CFStringBuiltInEncodings
		kCFStringEncodingInvalidId     = ::kCFStringEncodingInvalidId,
		kCFStringEncodingMacRoman      = ::kCFStringEncodingMacRoman,
		kCFStringEncodingWindowsLatin1 = ::kCFStringEncodingWindowsLatin1,
		kCFStringEncodingISOLatin1     = ::kCFStringEncodingISOLatin1,
		kCFStringEncodingNextStepLatin = ::kCFStringEncodingNextStepLatin,
		kCFStringEncodingASCII         = ::kCFStringEncodingASCII,
		kCFStringEncodingUnicode       = ::kCFStringEncodingUnicode,
		kCFStringEncodingUTF8          = ::kCFStringEncodingUTF8,
		kCFStringEncodingNonLossyASCII = ::kCFStringEncodingNonLossyASCII,
		
	#ifdef MAC_OS_X_VERSION_10_4
		
		kCFStringEncodingUTF16   = ::kCFStringEncodingUTF16,
		kCFStringEncodingUTF16BE = ::kCFStringEncodingUTF16BE,
		kCFStringEncodingUTF16LE = ::kCFStringEncodingUTF16LE,
		
		kCFStringEncodingUTF32   = ::kCFStringEncodingUTF32,
		kCFStringEncodingUTF32BE = ::kCFStringEncodingUTF32BE,
		kCFStringEncodingUTF32LE = ::kCFStringEncodingUTF32LE,
		
	#endif
		
		kCFStringEncoding_Max = nucleus::enumeration_traits< ::CFStringEncoding >::max
	};
	
}

#endif
