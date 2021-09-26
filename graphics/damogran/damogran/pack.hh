/*
	pack.hh
	-------
*/

#ifndef DAMOGRAN_PACK_HH
#define DAMOGRAN_PACK_HH

namespace damogran
{

typedef unsigned char uint8_t;

unsigned long pack_preflight( const uint8_t* src, const uint8_t* end );

uint8_t* pack( const uint8_t* src, const uint8_t* end, uint8_t* dst );

}

#endif
