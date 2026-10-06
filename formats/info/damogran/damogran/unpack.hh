/*
	unpack.hh
	---------
*/

#ifndef DAMOGRAN_UNPACK_HH
#define DAMOGRAN_UNPACK_HH

namespace damogran
{

typedef unsigned char uint8_t;

long unpack_preflight( const uint8_t* src, const uint8_t* end );

#ifdef __MC68K__
	#define _a0  : __A0
	#define _a1  : __A1
#else
	#define _a0  /**/
	#define _a1  /**/
#endif

const uint8_t* unpack( const uint8_t* src _a0, uint8_t* dst _a1, uint8_t* end );

#undef _a0
#undef _a1

}

#endif
