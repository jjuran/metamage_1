/*
	fixed.cc
	--------
*/

#include "callout/fixed.hh"

// quickdraw
#include "qd/fixed.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k    {
namespace callout {

using namespace quickdraw;

enum
{
	rts = 0x4E75,
};

int32_t fast_fix_mul_callout( v68k::processor_state& s )
{
	const uint32_t a = s.d(0);
	const uint32_t b = s.d(1);
	
	s.d(0) = fix_mul( a, b );
	
	return rts;
}

int32_t fast_fix_ratio_callout( v68k::processor_state& s )
{
	const uint16_t numer = s.d(0);
	const uint16_t denom = s.d(1);
	
	s.d(0) = fix_ratio( numer, denom );
	
	return rts;
}

int32_t fast_fix_round_callout( v68k::processor_state& s )
{
	const uint32_t x = s.d(0);
	
	s.d(0) = fix_round( x );
	
	return rts;
}

int32_t fast_fix_div_callout( v68k::processor_state& s )
{
	const uint32_t numer = s.d(0);
	const uint32_t denom = s.d(1);
	
	s.d(0) = fix_div( numer, denom );
	
	return rts;
}

}  // namespace callout
}  // namespace v68k
