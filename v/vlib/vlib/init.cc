/*
	init.cc
	-------
*/

#include "vlib/init.hh"

// Standard C
#include <stddef.h>

// vlib
#include "vlib/functions.hh"
#include "vlib/proc_info.hh"
#include "vlib/symbol_table.hh"
#include "vlib/types.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/byte.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/mb32.hh"
#include "vlib/types/null.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/stdint.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"


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
	
	bool install_keywords()
	{
		define_keyword( "false", False );
		define_keyword( "true",  True  );
		
		define_keyword( "boolean",  boolean_vtype  );
		define_keyword( "bool",     boolean_vtype  );
		define_keyword( "function", function_vtype );
		define_keyword( "integer",  integer_vtype  );
		define_keyword( "int",      integer_vtype  );
		define_keyword( "vector",   packed_vtype   );
		define_keyword( "vec",      packed_vtype   );
		define_keyword( "string",   string_vtype   );
		define_keyword( "str",      string_vtype   );
		define_keyword( "c_str",    c_str_vtype    );
		define_keyword( "type",     type_vtype     );
		define_keyword( "null",     null_vtype     );
		define_keyword( "mb32",     mb32_vtype     );
		define_keyword( "byte",     byte_vtype     );
		define_keyword( "i64",      i64_vtype      );
		define_keyword( "u64",      u64_vtype      );
		define_keyword( "i32",      i32_vtype      );
		define_keyword( "u32",      u32_vtype      );
		define_keyword( "i16",      i16_vtype      );
		define_keyword( "u16",      u16_vtype      );
		define_keyword( "i8",       i8_vtype       );
		define_keyword( "u8",       u8_vtype       );
		
		define_keyword( "array", generic_array_type );
		
		if ( sizeof (size_t) == 4 )  define_keyword( "size_t", u32_vtype );
		if ( sizeof (size_t) == 8 )  define_keyword( "size_t", u64_vtype );
		
		define_keyword( proc_abs    );
		define_keyword( proc_areaof );
		define_keyword( proc_half   );
		define_keyword( proc_hex    );
		define_keyword( proc_join   );
		define_keyword( proc_mince  );
		define_keyword( proc_rep    );
		define_keyword( proc_sha256 );
		define_keyword( proc_substr );
		define_keyword( proc_trans  );
		define_keyword( proc_typeof );
		define_keyword( proc_unbin  );
		define_keyword( proc_unhex  );
		
		define_keyword( proc_mkpub  );
		define_keyword( proc_sign   );
		define_keyword( proc_verify );
		
		return true;
	}
	
}
