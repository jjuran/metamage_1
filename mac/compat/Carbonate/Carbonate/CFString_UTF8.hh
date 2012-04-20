/*
	CFString_UTF8.hh
	----------------
*/

#ifndef CARBONATE_CFSTRINGUTF8_HH
#define CARBONATE_CFSTRINGUTF8_HH

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif


UInt32 count_UTF8_chars( const char* begin, const char* end );

unsigned put_code_point_into_UTF8( unsigned int uc, char* p, const char* end );


#endif

