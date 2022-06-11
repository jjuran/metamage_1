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
	
	Value i8_type()
	{
		return StdInt( i8_vtype, proc_encode_i8, proc_decode_i8 );
	}
	
	Value u8_type()
	{
		return StdInt( u8_vtype, proc_encode_u8, proc_decode_u8 );
	}
	
	#define DEFINE_MULTIBYTE_STDINT( type )  \
	Value type##_type()  \
	{                    \
		return StdInt( type##_vtype,          \
		               proc_encode_##type,    \
		               proc_decode_##type );  \
	}
	
	DEFINE_MULTIBYTE_STDINT( i16 )
	DEFINE_MULTIBYTE_STDINT( i32 )
	DEFINE_MULTIBYTE_STDINT( i64 )
	DEFINE_MULTIBYTE_STDINT( u16 )
	DEFINE_MULTIBYTE_STDINT( u32 )
	DEFINE_MULTIBYTE_STDINT( u64 )
	
}
