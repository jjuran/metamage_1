/*
	init.cc
	-------
*/

#include "vlib/init.hh"

// Standard C
#include <stddef.h>

// vlib
#include "vlib/calc.hh"
#include "vlib/functions.hh"
#include "vlib/proc_info.hh"
#include "vlib/symbol_table.hh"
#include "vlib/types.hh"


namespace vlib
{
	
	static
	void define( const char* name, const Value& v )
	{
		const symbol_id sym = create_symbol( name, Symbol_const );
		
		assign_symbol( sym, v );
	}
	
	void define( const proc_info& proc )
	{
		const symbol_id sym = create_symbol( proc.name, Symbol_const );
		
		assign_symbol( sym, proc );
	}
	
	bool install_basic_symbols()
	{
		define( "false", Value( false ) );
		define( "true",  Value( true  ) );
		
		define( "boolean",  boolean_vtype  );
		define( "bool",     boolean_vtype  );
		define( "function", function_vtype );
		define( "integer",  integer_vtype  );
		define( "int",      integer_vtype  );
		define( "string",   string_vtype   );
		define( "str",      string_vtype   );
		define( "c_str",    c_str_vtype    );
		define( "type",     type_vtype     );
		define( "byte",     byte_vtype     );
		define( "i64",      i64_vtype      );
		define( "u64",      u64_vtype      );
		define( "i32",      i32_vtype      );
		define( "u32",      u32_vtype      );
		define( "i16",      i16_vtype      );
		define( "u16",      u16_vtype      );
		define( "i8",       i8_vtype       );
		define( "u8",       u8_vtype       );
		
		if ( sizeof (size_t) == 4 )  define( "size_t", u32_vtype );
		if ( sizeof (size_t) == 8 )  define( "size_t", u64_vtype );
		
		define( proc_abs    );
		define( proc_area   );
		define( proc_half   );
		define( proc_hex    );
		define( proc_join   );
		define( proc_mince  );
		define( proc_rep    );
		define( proc_typeof );
		define( proc_unbin  );
		define( proc_unhex  );
		
		create_symbol( "_", Symbol_var );
		
		return true;
	}
	
}
