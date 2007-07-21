// ==========
// Options.hh
// ==========

#ifndef OPTIONS_HH
#define OPTIONS_HH

enum ShellOption
{
	kOptionBraceExpansion,
	kOptionExitOnError,
	kOptionInteractive,
	kOptionInteractiveComments,
	kOptionMonitor,
	kOptionNonClobberingRedirection,
	
	kOptionCount
};

class NotAnOption {};

ShellOption LookupShellOption( const char* name );

bool GetOption( ShellOption option );

void SetOption( ShellOption option, bool state );

inline bool GetOptionByName( const char* name )  { return GetOption( LookupShellOption( name ) ); }

inline void SetOptionByName( const char* name, bool state )  { SetOption( LookupShellOption( name ), state ); }

#endif

