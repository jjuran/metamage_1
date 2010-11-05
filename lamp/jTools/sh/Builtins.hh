// ===========
// Builtins.hh
// ===========

#ifndef BUILTINS_HH
#define BUILTINS_HH

// plus
#include "plus/string.hh"

// poseven
#ifndef POSEVEN_TYPES_EXIT_T_HH
#include "poseven/types/exit_t.hh"
#endif


namespace plus
{
	
	class string;
	
}

namespace tool
{
	
	typedef poseven::exit_t (*Builtin)( int argc, char** argv );
	
	void AssignShellVariable( const char* name, const char* value );
	
	const char* QueryShellVariable( const plus::string& name );
	
	Builtin FindBuiltin( const plus::string& name );
	
}

#endif

