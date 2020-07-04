/*
	Delay.cc
	--------
*/

#include "varyx/mac/Delay.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif

// vlib
#include "vlib/proc_info.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/stdint.hh"
#include "vlib/types/type.hh"


namespace varyx
{
namespace mac
{

using namespace vlib;

static
Value v_Delay( const Value& v )
{
	unsigned long delay_ticks = integer_cast< unsigned long >( v );
	unsigned long final_ticks;
	
	Delay( delay_ticks, &final_ticks );
	
	return Integer( final_ticks );
}

static const Type ulong = sizeof (long) == 4 ? u32_vtype : u64_vtype;

const proc_info proc_Delay = { "Delay", &v_Delay, &ulong };

}
}
