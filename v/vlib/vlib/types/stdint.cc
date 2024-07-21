/*
	stdint.cc
	---------
*/

#include "vlib/types/stdint.hh"

// Standard C
#include <stdint.h>

// bignum
#include "bignum/integer.hh"

// vlib
#include "vlib/type_info.hh"
#include "vlib/types/integer.hh"


namespace vlib
{
	
	enum signedness
	{
		u,
		i,
	};
	
	template < bool S, int N >  struct stdint;
	
	template <>  struct stdint< i,  8 > { typedef int8_t   type; };
	template <>  struct stdint< u,  8 > { typedef uint8_t  type; };
	template <>  struct stdint< i, 16 > { typedef int16_t  type; };
	template <>  struct stdint< u, 16 > { typedef uint16_t type; };
	template <>  struct stdint< i, 32 > { typedef int32_t  type; };
	template <>  struct stdint< u, 32 > { typedef uint32_t type; };
	template <>  struct stdint< i, 64 > { typedef int64_t  type; };
	template <>  struct stdint< u, 64 > { typedef uint64_t type; };
	
	static
	uint64_t coerced_int( const Value& v )
	{
		return Integer::coerce( v ).number().clipped_to< uint64_t >();
	}
	
	#define DEFINE_ADAPT_TO_INT( s, n )       \
	static                                    \
	Value assign_to_##s##n( const Value& v )  \
	{                                         \
		if ( v.type() == Value_number )  \
		{                                \
			if ( v.number().demotes_to< stdint< s, n >::type >() )  return v;  \
		}                                \
		return Value_NIL;                \
	}                                         \
	static                                    \
	Value coerce_to_##s##n( const Value& v )  \
	{ return Integer( (stdint< s, n >::type) coerced_int( v ) ); }
	
	DEFINE_ADAPT_TO_INT( i, 64 )
	DEFINE_ADAPT_TO_INT( u, 64 )
	DEFINE_ADAPT_TO_INT( i, 32 )
	DEFINE_ADAPT_TO_INT( u, 32 )
	DEFINE_ADAPT_TO_INT( i, 16 )
	DEFINE_ADAPT_TO_INT( u, 16 )
	DEFINE_ADAPT_TO_INT( i, 8  )
	DEFINE_ADAPT_TO_INT( u, 8  )
	
	#define DEFINE_TYPE_INFO_A_C( T )  \
	const type_info T##_vtype = { #T, &assign_to_##T, &coerce_to_##T, 0, 0, 0 }
	
	DEFINE_TYPE_INFO_A_C( i64 );
	DEFINE_TYPE_INFO_A_C( u64 );
	DEFINE_TYPE_INFO_A_C( i32 );
	DEFINE_TYPE_INFO_A_C( u32 );
	DEFINE_TYPE_INFO_A_C( i16 );
	DEFINE_TYPE_INFO_A_C( u16 );
	DEFINE_TYPE_INFO_A_C( i8  );
	DEFINE_TYPE_INFO_A_C( u8  );
	
}
