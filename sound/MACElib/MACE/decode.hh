/*
	decode.hh
	---------
*/

#ifndef MACE_DECODE_HH
#define MACE_DECODE_HH


namespace MACE
{
	
	typedef unsigned char uint8_t;
	
	typedef uint8_t semiframe_t;
	
	void decode_n_MACE3_to_u8( uint8_t* out, const semiframe_t* in, int n );
	
}  // namespace MACE

#endif
