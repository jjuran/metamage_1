// ===========
// Builtins.hh
// ===========

#ifndef BUILTINS_HH
#define BUILTINS_HH

// Standard C++
#include <map>

// Iota
#include "iota/argv.hh"

// plus
#include "plus/string.hh"

// poseven
#include "poseven/types/exit_t.hh"


namespace plus
{
	
	class string;
	
}

namespace tool
{
	
	typedef poseven::exit_t (*Builtin)( int argc, iota::argv_t argv );
	
	void AssignShellVariable( const char* name, const char* value );
	
	const char* QueryShellVariable( const plus::string& name );
	
	Builtin FindBuiltin( const plus::string& name );
	
}

#endif

