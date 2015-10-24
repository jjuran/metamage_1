/*
	calc.hh
	-------
*/

#ifndef VC_CALC_HH
#define VC_CALC_HH

// vc
#include "vc/op_type.hh"
#include "vc/value.hh"


namespace vc
{
	
	Value v_join( const Value& args );
	
	Value calc( const Value&  left,
	            op_type       op,
	            const Value&  right );
	
}

#endif
