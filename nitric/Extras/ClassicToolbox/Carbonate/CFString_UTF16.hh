/*
	CFString_UTF16.hh
	-----------------
*/

#ifndef CARBONATE_CFSTRINGUTF16_HH
#define CARBONATE_CFSTRINGUTF16_HH

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif


UInt32 count_UTF16_chars( const UTF16Char* begin, const UTF16Char* end );

unsigned int get_next_code_point_from_UTF16( const char*& p, const char* end );

unsigned put_code_point_into_UTF16( unsigned int uc, char* p, const char* end );


#endif

