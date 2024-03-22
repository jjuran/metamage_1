/*
	fixed.hh
	--------
*/

#ifndef CALLOUT_FIXED_HH
#define CALLOUT_FIXED_HH

// v68k
#include "v68k/state.hh"


namespace v68k    {
namespace callout {

int32_t fast_fix_mul_callout  ( v68k::processor_state& s );
int32_t fast_fix_ratio_callout( v68k::processor_state& s );
int32_t fast_fix_round_callout( v68k::processor_state& s );
int32_t fast_fix_div_callout  ( v68k::processor_state& s );

}  // namespace callout
}  // namespace v68k


#endif
