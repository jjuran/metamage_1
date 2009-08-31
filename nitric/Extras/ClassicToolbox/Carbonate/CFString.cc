// Carbonate/CFString.cc


#ifndef __CFSTRING__
#include <CFString.h>
#endif

#if !TARGET_API_MAC_CARBON

#include <string>

#include "Carbonate/CFBase.hh"



//#define CFSTR( x )  ( x )

typedef std::basic_string< UniChar > UniString;

struct __CFString : CFObject
{
	std::string  chars8;
	UniString    chars16;
	
	__CFString()  {}
	
	__CFString( const char* cStr ) : chars8( cStr )  {}
	
	__CFString( ConstStr255Param pStr ) : chars8( pStr + 1, pStr + 1 + pStr[ 0 ] )  {}
	
	__CFString( const char   * chars, std::size_t length ) : chars8 ( chars, length )  {}
	__CFString( const UniChar* chars, std::size_t length ) : chars16( chars, length )  {}
};

CFStringRef CFStringCreateWithPascalString( CFAllocatorRef    alloc,
                                            ConstStr255Param  pStr,
                                            CFStringEncoding  encoding )
{
	return new __CFString( pStr );
}

CFStringRef CFStringCreateWithCharacters( CFAllocatorRef  alloc,
                                          const UniChar*  chars,
                                          CFIndex         numChars )
{
	return new __CFString( chars, numChars );
}

#endif

