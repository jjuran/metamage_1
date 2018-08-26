/*
	startup.cc
	----------
*/

#include "vlib/startup.hh"

// vlib
#include "vlib/analyze.hh"
#include "vlib/execute.hh"
#include "vlib/parse.hh"
#include "vlib/symbol.hh"
#include "vlib/symbol_table.hh"
#include "vlib/iterators/list_iterator.hh"


namespace vlib
{
	
	static const char* startup =
		"def abs (x) {[ x, -x ][ +(x < 0) ]};\n"  // if-then doesn't fold yet
		;
	
	void inject_startup_header( lexical_scope* globals )
	{
		const Value parsed = parse( startup, "<startup>" );
		const Value analyzed = analyze( parsed, globals );
		
		execute( analyzed );
		
		const Value& symbols = analyzed.expr()->left;
		
		list_iterator it( symbols );
		
		++it;  // skip `_`
		
		while ( it )
		{
			const Symbol* sym = it.use().sym();
			
			create_keyword( sym->name() ).sym()->assign( sym->get() );
		}
	}
	
}
