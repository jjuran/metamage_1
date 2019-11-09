/*
	decode.cc
	---------
*/

#include "MACE/decode.hh"

// MACElib
#include "MACE/tables.hh"


namespace MACE
{

struct M3State
{
	 int16_t index;
	 int16_t level;
};

static inline
int16_t MACE_clipped_add( int16_t a, int16_t b )
{
	int sum = a + b;
	
	return + sum >  32767 ?  32767
	       : sum < -32768 ? -32767
	       :                 sum;
}

static
int16_t read_table( M3State& state, short phase, uint8_t codon )
{
    const metatable& meta = metatables[ phase ];
    
    const int16_t* one = meta.one;
    const int16_t* two = meta.two;
    
	const int stride = meta.stride;
	
	short base_index = ((state.index & 0x7f0) >> 4) * stride;
	
	if ( (state.index += one[ codon ] - (state.index >> 5)) < 0 )
	{
		state.index = 0;
	}
	
	return codon < stride ?  two[ base_index + codon                  ]
	                      : ~two[ base_index - codon + 2 * stride - 1 ];
}

static
uint8_t decode_MACE3_sample( M3State& state, short phase, uint8_t codon )
{
	int16_t readout = read_table( state, phase, codon );
	
	int16_t sample = MACE_clipped_add( readout, state.level );
	
	state.level = sample - (sample >> 3);
	
	return (sample >> 8) ^ 0x80;
}

void decode_n_MACE3_frames( const uint8_t* in, uint8_t* out, int n )
{
	M3State state = {};
	
	n <<= 1;
	
	while ( n-- > 0 )
	{
		uint8_t byte = *in++;
		
		*out++ = decode_MACE3_sample( state, 0,  byte        & 7 );
		*out++ = decode_MACE3_sample( state, 1, (byte >>= 3) & 3 );
		*out++ = decode_MACE3_sample( state, 2,  byte >>  2      );
	}
}

}  // namespace MACE
