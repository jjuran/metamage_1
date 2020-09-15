/*
	unpack.hh
	---------
*/

#ifndef DAMOGRAN_UNPACK_HH
#define DAMOGRAN_UNPACK_HH

namespace damogran
{

typedef unsigned char uint8_t;

const uint8_t* validate( const uint8_t* src, const uint8_t* end, long size );

const uint8_t* unpack( const uint8_t* src, uint8_t* dst, uint8_t* end );

}

#endif
