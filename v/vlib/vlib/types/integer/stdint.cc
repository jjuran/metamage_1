/*
	stdint.cc
	---------
*/

#include "vlib/types/integer/stdint.hh"

// vlib
#include "vlib/types/field.hh"
#include "vlib/types/stdint.hh"
#include "vlib/types/endec/stdint.hh"


namespace vlib
{
	
	#define DEFINE_STDINT_2( n_bits, type )  \
	Value type##_type()  \
	{                    \
		return Field( n_bits / 8,            \
		              type##_vtype,          \
		              proc_encode_##type,    \
		              proc_decode_##type );  \
	}
	
	#define DEFINE_STDINT( signedness, n_bits )  \
		DEFINE_STDINT_2( n_bits, signedness##n_bits )
	
	DEFINE_STDINT( i, 8  )
	DEFINE_STDINT( i, 16 )
	DEFINE_STDINT( i, 32 )
	DEFINE_STDINT( i, 64 )
	DEFINE_STDINT( u, 8  )
	DEFINE_STDINT( u, 16 )
	DEFINE_STDINT( u, 32 )
	DEFINE_STDINT( u, 64 )
	
}
