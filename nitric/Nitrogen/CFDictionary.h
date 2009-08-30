// Nitrogen/CFDictionary.h
// -----------------------

// Part of the Nitrogen project.
//
// Written 2003-2004 by Lisa Lippincott and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_CFDICTIONARY_H
#define NITROGEN_CFDICTIONARY_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __CFDICTIONARY__
#include FRAMEWORK_HEADER(CoreFoundation,CFDictionary.h)
#endif
#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
#endif

namespace Nitrogen
  {
   using ::CFDictionaryRef;
   using ::CFDictionaryGetTypeID;
   template <> struct CFType_Traits< CFDictionaryRef >: Basic_CFType_Traits< CFDictionaryRef, ::CFDictionaryGetTypeID > {};

   using ::CFMutableDictionaryRef;
   template <> struct CFType_Traits< CFMutableDictionaryRef >: Basic_CFType_Traits< CFMutableDictionaryRef, ::CFDictionaryGetTypeID > {};
   }

namespace Nucleus {
   template <> struct OwnedDefaults< Nitrogen::CFDictionaryRef >       : OwnedDefaults< Nitrogen::CFTypeRef >  {};
   template <> struct OwnedDefaults< Nitrogen::CFMutableDictionaryRef >: OwnedDefaults< Nitrogen::CFTypeRef >  {};
   }

namespace Nitrogen {
   class CFDictionaryCreate_Failed {};
   Nucleus::Owned<CFDictionaryRef> CFDictionaryCreate ( CFAllocatorRef allocator, const void **keys, const void **values, CFIndex numValues, 
   				const CFDictionaryKeyCallBacks *keyCallBacks = NULL, const CFDictionaryValueCallBacks *valueCallBacks = NULL);

   inline void CFShow( const CFMutableDictionaryRef d ) { ::CFShow( d ); }
   inline void CFShow( const CFDictionaryRef d )        { ::CFShow( d ); }

   class CFDictionaryCreateMutable_Failed {};
   Nucleus::Owned<CFMutableDictionaryRef> CFDictionaryCreateMutable ( CFAllocatorRef allocator, CFIndex capacity, 
   				const CFDictionaryKeyCallBacks *keyCallBacks = NULL, const CFDictionaryValueCallBacks *valueCallBacks = NULL );
   
   
   class CFDictionaryCreateCopy_Failed {};
   Nucleus::Owned<CFDictionaryRef> CFDictionaryCreateCopy ( CFAllocatorRef allocator, CFDictionaryRef theDict );

   class CFDictionaryCreateMutableCopy_Failed {};
   Nucleus::Owned<CFMutableDictionaryRef> CFDictionaryCreateMutableCopy ( CFAllocatorRef allocator, CFIndex capacity, CFDictionaryRef theDict );

//	CFIndex CFDictionaryGetCount		( CFDictionaryRef theDict );
//	CFIndex CFDictionaryGetCountOfKey	( CFDictionaryRef theDict, const void *key );
//	CFIndex CFDictionaryGetCountOfValue ( CFDictionaryRef theDict, const void *value );
	using ::CFDictionaryGetCount;
	using ::CFDictionaryGetCountOfKey;
	using ::CFDictionaryGetCountOfValue;

//	Boolean CFDictionaryContainsKey	  ( CFDictionaryRef theDict, const void *key );
//	Boolean CFDictionaryContainsValue ( CFDictionaryRef theDict, const void *value );
	using ::CFDictionaryContainsKey;
	using ::CFDictionaryContainsValue;
	
//	const void *CFDictionaryGetValue      ( CFDictionaryRef theDict, const void *key );
//	Boolean CFDictionaryGetValueIfPresent ( CFDictionaryRef theDict, const void *key, const void **value );
	using ::CFDictionaryGetValue;
	using ::CFDictionaryGetValueIfPresent;
	
#if 0
void CFDictionaryApplyFunction(CFDictionaryRef theDict, CFDictionaryApplierFunction applier, void *context);
void CFDictionaryAddValue(CFMutableDictionaryRef theDict, const void *key, const void *value);
void CFDictionarySetValue(CFMutableDictionaryRef theDict, const void *key, const void *value);
void CFDictionaryReplaceValue(CFMutableDictionaryRef theDict, const void *key, const void *value);
void CFDictionaryRemoveValue(CFMutableDictionaryRef theDict, const void *key);
void CFDictionaryRemoveAllValues(CFMutableDictionaryRef theDict);
#endif

  }

#endif
