// Carbonate/CFString.cc

// Mac OS
#ifndef __CFSTRING__
#include <CFString.h>
#endif

#if !TARGET_API_MAC_CARBON

#include "Carbonate/CFBase.hh"


struct __CFString : CFObject
{
	__CFString( const UTF16Char* u, UInt32 n_chars );
};

CFStringRef CFStringCreateWithPascalString( CFAllocatorRef    alloc,
                                            ConstStr255Param  pStr,
                                            CFStringEncoding  encoding )
{
	return CFStringCreateWithBytes( alloc,
	                                pStr + 1,
	                                pStr[ 0 ],
	                                encoding,
	                                false );
}

CFStringRef CFStringCreateWithCharacters( CFAllocatorRef  alloc,
                                          const UniChar*  chars,
                                          CFIndex         numChars )
{
	return new __CFString( chars, numChars );
}

#endif

