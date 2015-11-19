/*
	init.cc
	-------
*/

#include "vlib/init.hh"

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
		define( "function", function_vtype );
		define( "integer",  integer_vtype  );
		define( "int",      integer_vtype  );
		define( "string",   string_vtype   );
		define( "type",     type_vtype     );
		
		define( proc_abs    );
		define( proc_area   );
		define( proc_bool   );
		define( proc_half   );
		define( proc_hex    );
		define( proc_join   );
		define( proc_rep    );
		define( proc_str    );
		define( proc_typeof );
		define( proc_unbin  );
		define( proc_unhex  );
		
		return true;
	}
	
}
