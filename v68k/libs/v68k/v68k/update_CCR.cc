/*
	update_CCR.cc
	-------------
*/

#include "v68k/update_CCR.hh"

// v68k
#include "v68k/op_params.hh"
#include "v68k/state.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k
{
	
	#define N( x )  (!!(x) << 3)
	#define Z( x )  (!!(x) << 2)
	#define V( x )  (!!(x) << 1)
	#define C( x )  (!!(x) << 0)
	
	
	static inline
	uint32_t additive_overflow( uint32_t a, uint32_t b, uint32_t c )
	{
		return ~(a ^ b) & (a ^ c);
	}
	
	static inline
	uint32_t additive_carry( uint32_t a, uint32_t b, uint32_t c )
	{
		return (a & b) | (~c & b) | (a & ~c);
	}
	
	static inline
	uint8_t common_NZ( int32_t data, int pb_size )
	{
		int n_bits = 1 << (pb_size + 2);  // 1, 2, 3 -> 3, 4, 5 -> 8, 16, 32
		
		data = (uint32_t) data << (32 - n_bits);  // 8, 16, 32 -> 24, 16, 0
		
		return + N( data <  0 )
		       | Z( data == 0 );
	}
	
	static inline
	uint8_t ADDX_NZ( int32_t data, int pb_size, uint8_t nzvc )
	{
		int n_bits = 1 << (pb_size + 2);  // 1, 2, 3 -> 3, 4, 5 -> 8, 16, 32
		
		data = (uint32_t) data << (32 - n_bits);  // 8, 16, 32 -> 24, 16, 0
		
		return +   N( data <  0 )
		       | ( Z( data == 0 ) & nzvc & 0x4 );
	}
	
	static inline
	uint8_t additive_VC( uint32_t a, uint32_t b, uint32_t c, int pb_size )
	{
		uint32_t V = additive_overflow( a, b, c );
		uint32_t C = additive_carry   ( a, b, c );
		
		int n_bits = 1 << (pb_size + 2);  // 1, 2, 3 -> 3, 4, 5 -> 8, 16, 32
		
		int v_shift = n_bits - 2;
		int c_shift = n_bits - 1;
		
		int v_mask = 0x02;
		int c_mask = 0x01;
		
		return + (V >> v_shift & v_mask)
		       | (C >> c_shift & c_mask);
	}
	
	static void update_CCR_ADD( processor_state& s, const op_params& pb )
	{
		const int32_t a = pb.first;
		const int32_t b = pb.second;
		const int32_t c = pb.result;
		
		s.sr.nzvc = common_NZ( c, pb.size )
		          | additive_VC( a, b, c, pb.size );
	}
	
	static void update_CCR_SUB( processor_state& s, const op_params& pb )
	{
		const int32_t a = pb.first;
		const int32_t b = pb.second;
		const int32_t d = b - a;
		
		s.sr.nzvc = common_NZ( d, pb.size )
		          | additive_VC( a, d, b, pb.size );  // b is the sum
	}
	
	static void update_CCR_ADDX( processor_state& s, const op_params& pb )
	{
		const int32_t a = pb.first - s.sr.x;
		const int32_t b = pb.second;
		const int32_t c = pb.result;
		
		s.sr.nzvc = ADDX_NZ( c, pb.size, s.sr.nzvc )
		          | additive_VC( a, b, c, pb.size );
	}
	
	static void update_CCR_SUBX( processor_state& s, const op_params& pb )
	{
		const int32_t a = pb.first - s.sr.x;
		const int32_t b = pb.second;
		const int32_t d = pb.result;
		
		s.sr.nzvc = ADDX_NZ( d, pb.size, s.sr.nzvc )
		          | additive_VC( a, d, b, pb.size );  // b is the sum
	}
	
	static void update_CCR_TST( processor_state& s, const op_params& pb )
	{
		s.sr.nzvc = common_NZ( pb.result, pb.size );
	}
	
	static void update_CCR_BTST( processor_state& s, const op_params& pb )
	{
		const uint32_t bit = pb.first;
		
		s.sr.nzvc &= ~0x4;
		s.sr.nzvc |= (~pb.second >> bit & 0x1) << 2;
	}
	
	static void update_CCR_DIV( processor_state& s, const op_params& pb )
	{
		if ( pb.size == unsized )
		{
			/*
				Either overflow or division by zero has occurred.
				V is set in the former case and undefined in the latter.
				N and Z are undefined either way; C is always cleared.
				So set V and clear N, Z, and C.
			*/
			
			s.sr.nzvc = 0x2;
		}
		else
		{
			/*
				DIVS.W and DIVU.W are long-sized operations, in the sense
				that they read and write a 32-bit operand/result.  (The
				divisor is only 16 bits, but is always in a register, so
				memory access isn't an issue.)  However, the CCR update
				considers only the 16-bit quotient.
			*/
			
			s.sr.nzvc = common_NZ( pb.result, word_sized );
		}
	}
	
	
	CCR_updater the_CCR_updaters[] =
	{
		&update_CCR_ADD,
		&update_CCR_SUB,
		&update_CCR_ADDX,
		&update_CCR_SUBX,
		&update_CCR_TST,
		&update_CCR_BTST,
		&update_CCR_DIV
	};
	
}
