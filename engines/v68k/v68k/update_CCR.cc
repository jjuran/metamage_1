/*
	update_CCR.cc
	-------------
*/

#include "v68k/update_CCR.hh"

// v68k
#include "v68k/macros.hh"
#include "v68k/op_params.hh"
#include "v68k/state.hh"


namespace v68k
{
	
	#define N( x )  (!!(x) << 3)
	#define Z( x )  (!!(x) << 2)
	#define V( x )  (!!(x) << 1)
	#define C( x )  (!!(x) << 0)
	
	
	static inline uint8_t common_NZ( int32_t data )
	{
		return + N( data <  0 )
		       | Z( data == 0 );
	}
	
	static inline uint8_t additive_VC( int32_t a, int32_t b, int32_t c )
	{
		const bool S = a < 0;
		const bool D = b < 0;
		const bool R = c < 0;
		
		return + V( (S == D) & (S != R) )
		       | C( S & D | !R & D | S & !R );
	}
	
	static void update_CCR_ADD( processor_state& s, const op_params& pb )
	{
		const int32_t a = sign_extend( pb.first,  pb.size );
		const int32_t b = sign_extend( pb.second, pb.size );
		const int32_t c = sign_extend( pb.result, pb.size );
		
		s.regs.nzvc = common_NZ( c )
		            | additive_VC( a, b, c );
	}
	
	static void update_CCR_SUB( processor_state& s, const op_params& pb )
	{
		const int32_t a = pb.first;
		const int32_t b = pb.second;
		
		const int32_t d = b - a;
		
		s.regs.nzvc = common_NZ( d )
		            | additive_VC( a, d, b );  // b is the sum
	}
	
	static void update_CCR_TST( processor_state& s, const op_params& pb )
	{
		const int32_t data = sign_extend( pb.result, pb.size );
		
		s.regs.nzvc = common_NZ( data );
	}
	
	static void update_CCR_BTST( processor_state& s, const op_params& pb )
	{
		const uint32_t bit = pb.first;
		
		s.regs.nzvc &= ~0x4;
		s.regs.nzvc |= (~pb.second >> bit & 0x1) << 2;
	}
	
	
	CCR_updater the_CCR_updaters[] =
	{
		&update_CCR_ADD,
		&update_CCR_SUB,
		&update_CCR_TST,
		&update_CCR_BTST
	};
	
}

