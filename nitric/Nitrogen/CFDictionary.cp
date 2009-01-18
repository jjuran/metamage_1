// Nitrogen/CFDictionary.cp
// ------------------------

// Part of the Nitrogen project.
//
// Written 2003-2004 by Lisa Lippincott and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#include "Nitrogen/CFDictionary.h"


namespace Nitrogen {
	Nucleus::Owned<CFDictionaryRef>
	CFDictionaryCreate ( CFAllocatorRef allocator, const void **keys, const void **values, CFIndex numValues, const CFDictionaryKeyCallBacks *keyCallBacks, const CFDictionaryValueCallBacks *valueCallBacks ) {
		CFDictionaryRef result = ::CFDictionaryCreate ( allocator, keys, values, numValues, keyCallBacks, valueCallBacks );
		if ( result == NULL )
			throw CFDictionaryCreate_Failed ();
		return Nucleus::Owned<CFDictionaryRef>::Seize( result );
		}

	Nucleus::Owned<CFMutableDictionaryRef>
	CFDictionaryCreateMutable ( CFAllocatorRef allocator, CFIndex capacity, const CFDictionaryKeyCallBacks *keyCallBacks, const CFDictionaryValueCallBacks *valueCallBacks ) {
		CFMutableDictionaryRef result = ::CFDictionaryCreateMutable ( allocator, capacity, keyCallBacks, valueCallBacks );
		if ( result == NULL )
			throw CFDictionaryCreateMutable_Failed ();
		return Nucleus::Owned<CFMutableDictionaryRef>::Seize( result );
		}
	
	Nucleus::Owned<CFDictionaryRef>
	CFDictionaryCreateCopy ( CFAllocatorRef allocator, CFDictionaryRef theDict ) {
		CFDictionaryRef result = ::CFDictionaryCreateCopy ( allocator, theDict );
		if ( result == NULL )
			throw CFDictionaryCreateCopy_Failed ();
		return Nucleus::Owned<CFDictionaryRef>::Seize( result );
		}

	Nucleus::Owned<CFMutableDictionaryRef>
	CFDictionaryCreateMutableCopy ( CFAllocatorRef allocator, CFIndex capacity, CFDictionaryRef theDict ) {
		CFMutableDictionaryRef result = ::CFDictionaryCreateMutableCopy ( allocator, capacity, theDict );
		if ( result == NULL )
			throw CFDictionaryCreateMutableCopy_Failed ();
		return Nucleus::Owned<CFMutableDictionaryRef>::Seize( result );
		}
	}