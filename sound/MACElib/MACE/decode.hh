/*
	decode.hh
	---------
*/

#ifndef MACE_DECODE_HH
#define MACE_DECODE_HH


namespace MACE
{
	
	typedef unsigned char uint8_t;
	
	void decode_n_MACE3_frames( const uint8_t* in, uint8_t* out, int n );
	
}  // namespace MACE

#endif
