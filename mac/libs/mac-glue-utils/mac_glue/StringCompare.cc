/*
	StringCompare.cc
	----------------
*/

#include "mac_glue/StringCompare.hh"


#pragma exceptions off


namespace mac  {
namespace glue {

#ifdef __MC68K__

asm
unsigned long CmpString_sans_case( const unsigned char*  a : __A0,
                                   unsigned char         n : __D1,
                                   const unsigned char*  b : __A1,
                                   unsigned char         m : __D2 )
{
	MOVEQ.L  #0,D0
	MOVE.B   D1,D0
	SWAP     D0
	MOVE.B   D2,D0
	
	0xA03C            // _CmpString (case-insensitive, marks-sensitive)
	
	RTS
}

asm
long RelString_sans_case( const unsigned char*  a : __A0,
                          unsigned char         n : __D1,
                          const unsigned char*  b : __A1,
                          unsigned char         m : __D2 )
{
	MOVEQ.L  #0,D0
	MOVE.B   D1,D0
	SWAP     D0
	MOVE.B   D2,D0
	
	0xA050            // _RelString (case-insensitive, marks-sensitive)
	
	RTS
}

#else

int dummy;

#endif  // #ifdef __MC68K__

}
}
