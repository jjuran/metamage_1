/*
	execvpe.hh
	----------
*/

#ifndef EXECVPE_HH
#define EXECVPE_HH

#ifdef __APPLE__

int execvpe( const char* file, char* const argv[], char* const envp[] );

#endif

#endif

