/*
	math.cc
	-------
*/

#include "vlib/lib/math.hh"

// Standard C
#include <math.h>

#ifndef M_PI
	#ifndef __FP__
	#include <fp.h>
	#endif
	#define M_PI  pi
#endif

// vlib
#include "vlib/proc_info.hh"
#include "vlib/types/float.hh"
#include "vlib/types/type.hh"


namespace vlib
{

static
Value v_sincospi( const Value& v )
{
	double semis = static_cast< const Float& >( v );  // # of half circles
	double theta = M_PI * semis;
	
	double sin_th = sin( theta );
	double cos_th = cos( theta );
	
	return Value( Float( sin_th ), Float( cos_th ) );
}

static const Type float_ = float_vtype;

enum
{
	pure = Proc_pure,
};

const proc_info proc_sincospi = { "sincospi", &v_sincospi, &float_, pure };

}
