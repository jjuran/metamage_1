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
	
	class StdInt : public Field
	{
		public:
			StdInt( const type_info&  type,
			        const proc_info&  encode,
			        const proc_info&  decode );
	};
	
	static
	unsigned type_size( const type_info& type )
	{
		const char* name = type.name;
		
		++name;
		
		uint8_t a = *name++;
		uint8_t b = *name;
		
		unsigned size = 1;  // if either i8 or u8
		
		if ( b != 0 )
		{
			a -= '0';
			b -= '0';
			
			size = (a * 10 + b) / 8;
		}
		
		return size;
	}
	
	StdInt::StdInt( const type_info&  type,
	                const proc_info&  encode,
	                const proc_info&  decode )
	:
		Field( type_size( type ), type, encode, decode )
	{
	}
	
	#define DEFINE_STDINT_2( type )  \
	Value type##_type()  \
	{                    \
		return StdInt( type##_vtype,          \
		               proc_encode_##type,    \
		               proc_decode_##type );  \
	}
	
	#define DEFINE_STDINT( signedness, n_bits )  \
		DEFINE_STDINT_2( signedness##n_bits )
	
	DEFINE_STDINT( i, 8  )
	DEFINE_STDINT( i, 16 )
	DEFINE_STDINT( i, 32 )
	DEFINE_STDINT( i, 64 )
	DEFINE_STDINT( u, 8  )
	DEFINE_STDINT( u, 16 )
	DEFINE_STDINT( u, 32 )
	DEFINE_STDINT( u, 64 )
	
}
