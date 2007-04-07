// ==========
// Options.hh
// ==========

#ifndef OPTIONS_HH
#define OPTIONS_HH

enum ShellOption
{
	
};

class NotAnOption {};

ShellOption LookupShellOption( const char* name );

bool GetOption( ShellOption option );

void SetOption( ShellOption option, bool state );

inline bool GetOption( const char* name )  { return GetOption( LookupShellOption( name ) ); }

inline void SetOption( const char* name, bool state )  { SetOption( LookupShellOption( name ), state ); }

#endif

