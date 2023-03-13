/*
	StringCompare.hh
	----------------
*/

#ifndef MACGLUE_STRINGCOMPARE_HH
#define MACGLUE_STRINGCOMPARE_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MC68K__
#ifndef __STRINGCOMPARE__
#include <StringCompare.h>
#endif
#endif


namespace mac  {
namespace glue {

#ifndef __LP64__

#ifdef __MC68K__

unsigned long CmpString_sans_case( const unsigned char*  a : __A0,
                                   unsigned char         n : __D1,
                                   const unsigned char*  b : __A1,
                                   unsigned char         m : __D2 );

inline
unsigned long EqualString_sans_case( const unsigned char*  a : __A0,
                                     unsigned char         n : __D1,
                                     const unsigned char*  b : __A1,
                                     unsigned char         m : __D2 )
{
	return ! CmpString_sans_case( a, n, b, m );
}

inline
unsigned long EqualString_sans_case( const unsigned char*  a : __A0,
                                     const unsigned char*  b : __A1 )
{
	unsigned char n = *a++;
	unsigned char m = *b++;
	
	return EqualString_sans_case( a, n, b, m );
}

long RelString_sans_case( const unsigned char*  a : __A0,
                          unsigned char         n : __D1,
                          const unsigned char*  b : __A1,
                          unsigned char         m : __D2 );

inline
long RelString_sans_case( const unsigned char*  a : __A0,
                          const unsigned char*  b : __A1 )
{
	unsigned char n = *a++;
	unsigned char m = *b++;
	
	return RelString_sans_case( a, n, b, m );
}

#else  // #ifdef __MC68K__

inline
unsigned long EqualString_sans_case( const unsigned char*  a,
                                     const unsigned char*  b )
{
	return EqualString( a, b, false, true );
}

inline
long RelString_sans_case( const unsigned char*  a,
                          const unsigned char*  b )
{
	return RelString( a, b, false, true );
}

#endif  // #else  // #ifdef __MC68K__

#endif  // #ifndef __LP64__

}
}

#endif
