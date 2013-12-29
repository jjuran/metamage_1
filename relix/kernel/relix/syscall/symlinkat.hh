/*
	symlinkat.hh
	------------
*/

#ifndef RELIX_SYSCALL_SYMLINKAT_HH
#define RELIX_SYSCALL_SYMLINKAT_HH


namespace relix
{
	
	int symlinkat( const char* target_path, int newdirfd, const char* newpath );
	
}

#endif

