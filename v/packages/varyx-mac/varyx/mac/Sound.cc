/*
	Sound.cc
	--------
*/

#include "varyx/mac/Sound.hh"

#if ! __LP64__

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

// <Sound.h>
extern "C" pascal void SysBeep( short ticks )  ONEWORDINLINE( 0xA9C8 );

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
Value v_SysBeep( const Value& v )
{
	short ticks = integer_cast< short >( v );
	
	SysBeep( ticks );
	
	return Value_nothing;
}

static const Type u16 = u16_vtype;

static const Value u16_beep( u16,
                             Op_duplicate,
                             Integer( 6 ) );

const proc_info proc_SysBeep  = { "SysBeep", &v_SysBeep, &u16_beep };

}
}

#else  // #if ! __LP64__

int dummy;

#endif  // #if ! __LP64__
