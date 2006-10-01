// ===========
// Builtins.hh
// ===========

#ifndef BUILTINS_HH
#define BUILTINS_HH

// Standard C++
#include <map>
#include <string>

typedef int (*Builtin)( int argc, char const* const argv[] );

int Assign( const char* name, const char* value );

Builtin FindBuiltin( const std::string& name );

typedef std::map< std::string, std::string > StringMap;

#endif

