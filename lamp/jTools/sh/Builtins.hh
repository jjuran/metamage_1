// ===========
// Builtins.hh
// ===========

#ifndef BUILTINS_HH
#define BUILTINS_HH

// Standard C++
#include <map>
#include <string>

// Iota
#include "iota/argv.hh"

// poseven
#include "poseven/types/exit_t.hh"


namespace tool
{
	
	typedef poseven::exit_t (*Builtin)( int argc, iota::argv_t argv );
	
	void AssignShellVariable( const char* name, const char* value );
	
	const char* QueryShellVariable( const std::string& name );
	
	Builtin FindBuiltin( const std::string& name );
	
}

#endif

