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


namespace vlib
{
	
	void define( const proc_info& proc )
	{
		const symbol_id sym = create_symbol( proc.name, Symbol_const );
		
		assign_symbol( sym, proc );
	}
	
	bool install_basic_functions()
	{
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
