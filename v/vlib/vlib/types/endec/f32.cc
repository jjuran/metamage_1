/*
	f32.cc
	------
*/

#include "vlib/types/endec/f32.hh"

// vlib
#include "vlib/proc_info.hh"
#include "vlib/throw.hh"
#include "vlib/types/field.hh"
#include "vlib/types/float.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/type.hh"


namespace vlib
{

typedef float endec_float_type;

const int endec_float_size = sizeof (endec_float_type);

static
Value v_encode( const Value& v )
{
	endec_float_type data = v.as< Float >().get();
	
	return Packed( plus::string( (char*) &data, endec_float_size ) );
}

static
endec_float_type get_pack_data( const Value& v )
{
	const plus::string& pack = v.string();
	
	if ( pack.size() != endec_float_size )
	{
		THROW( "decode arg has wrong size" );
	}
	
	return *(const endec_float_type*) pack.data();
}

static
Value v_decode( const Value& v )
{
	return Float( get_pack_data( v ) );
}

static const Type float_ = float_vtype;
static const Type packed = packed_vtype;

enum
{
	pure = Proc_pure,
};

static const proc_info encode_f32 = { "encode", &v_encode, &float_, pure };
static const proc_info decode_f32 = { "decode", &v_decode, &packed, pure };

Value f32_type()
{
	return Field( endec_float_size, float_vtype, encode_f32, decode_f32 );
}

}
