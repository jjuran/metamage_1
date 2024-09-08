/*
	init.cc
	-------
*/

#include "vlib/init.hh"

// Standard C
#include <stddef.h>

// vlib
#include "vlib/functions.hh"
#include "vlib/namespace_info.hh"
#include "vlib/proc_info.hh"
#include "vlib/pure.hh"
#include "vlib/symbol_table.hh"
#include "vlib/types.hh"
#include "vlib/namespaces/Iter.hh"
#include "vlib/namespaces/Math.hh"
#include "vlib/namespaces/V.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/byte.hh"
#include "vlib/types/byteclass.hh"
#include "vlib/types/byterange.hh"
#include "vlib/types/endianness.hh"
#include "vlib/types/float.hh"
#include "vlib/types/pointer.hh"
#include "vlib/types/fraction.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/iterator.hh"
#include "vlib/types/namespace.hh"
#include "vlib/types/null.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/receiver.hh"
#include "vlib/types/record.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"
#include "vlib/types/endec/f32.hh"
#include "vlib/types/endec/f64.hh"
#include "vlib/types/endec/mb32.hh"
#include "vlib/types/integer/stdint.hh"
#include "vlib/types/struct/metatype.hh"
#include "vlib/types/vector/metatype.hh"


namespace vlib
{
	
	static
	void define_keyword( const char* name, const Value& v )
	{
		create_keyword( name ).sym()->assign( v );
	}
	
	static
	void define_keyword( const char* name, const type_info& type )
	{
		define_keyword( name, Type( type ) );
	}
	
	static
	void define_keyword( const proc_info& proc )
	{
		create_keyword( proc.name ).sym()->assign( Proc( proc ) );
	}
	
	static
	void define_keyword( const namespace_info& space )
	{
		create_keyword( space.name ).sym()->assign( Namespace( space ) );
	}
	
	bool install_keywords()
	{
		define_keyword( namespace_Iter );
		define_keyword( namespace_Math );
		define_keyword( namespace_V    );
		
		define_keyword( "false", False );
		define_keyword( "true",  True  );
		
		define_keyword( "boolean",  boolean_vtype  );
		define_keyword( "bool",     boolean_vtype  );
		define_keyword( "byteclass", byteclass_vtype );
		define_keyword( "byterange", byterange_vtype );
		define_keyword( "function", function_vtype );
		define_keyword( "fraction", fraction_vtype );
		define_keyword( "receiver", receiver_vtype );
		define_keyword( "iterator", iterator_vtype );
		define_keyword( "pointer",  pointer_vtype  );
		define_keyword( "integer",  integer_vtype  );
		define_keyword( "int",      integer_vtype  );
		define_keyword( "record",   record_vtype   );
		define_keyword( "packed",   packed_vtype   );
		define_keyword( "string",   string_vtype   );
		define_keyword( "str",      string_vtype   );
		define_keyword( "c_str",    c_str_vtype    );
		define_keyword( "type",     type_vtype     );
		define_keyword( "null",     null_vtype     );
		define_keyword( "mb32",     mb32_type()    );
		define_keyword( "byte",     byte_type()    );
		define_keyword( "float",    float_vtype    );
		define_keyword( "f64",      f64_type()     );
		define_keyword( "f32",      f32_type()     );
		define_keyword( "i64",      i64_type()     );
		define_keyword( "u64",      u64_type()     );
		define_keyword( "i32",      i32_type()     );
		define_keyword( "u32",      u32_type()     );
		define_keyword( "i16",      i16_type()     );
		define_keyword( "u16",      u16_type()     );
		define_keyword( "i8",       i8_type()      );
		define_keyword( "u8",       u8_type()      );
		
		define_keyword( "array", generic_array_type );
		
		define_keyword( "struct", Struct_Metatype() );
		define_keyword( "vector", Vector_Metatype() );
		
		define_keyword( "big-endian",    Endianness( big_endian    ) );
		define_keyword( "native-endian", Endianness( native_endian ) );
		define_keyword( "little-endian", Endianness( little_endian ) );
		
		if ( sizeof (size_t) == 4 )  define_keyword( "size_t", u32_type() );
		if ( sizeof (size_t) == 8 )  define_keyword( "size_t", u64_type() );
		
		define_keyword( proc_areaof );
		define_keyword( proc_half   );
		define_keyword( proc_head   );
		define_keyword( proc_hex    );
		define_keyword( proc_is_pure );
		define_keyword( proc_join   );
		define_keyword( proc_md5    );
		define_keyword( proc_mince  );
		define_keyword( proc_rep    );
		define_keyword( proc_sha1   );
		define_keyword( proc_sha256 );
		define_keyword( proc_substr );
		define_keyword( proc_tail   );
		define_keyword( proc_trans  );
		define_keyword( proc_transd );
		define_keyword( proc_typeof );
		define_keyword( proc_uchr   );
		define_keyword( proc_unbin  );
		define_keyword( proc_unhex  );
		
		return true;
	}
	
}
