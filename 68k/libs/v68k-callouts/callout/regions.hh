/*
	regions.hh
	----------
*/

#ifndef CALLOUT_REGIONS_HH
#define CALLOUT_REGIONS_HH

// v68k
#include "v68k/state.hh"


namespace v68k    {
namespace callout {

int32_t sect_rect_region_callout( v68k::processor_state& s );
int32_t sect_regions_callout    ( v68k::processor_state& s );
int32_t xor_regions_callout     ( v68k::processor_state& s );

}  // namespace callout
}  // namespace v68k


#endif
