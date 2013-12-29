/*
	renameat.hh
	-----------
*/

#ifndef RELIX_SYSCALL_RENAMEAT_HH
#define RELIX_SYSCALL_RENAMEAT_HH


namespace relix
{
	
	int renameat( int olddirfd, const char* oldpath, int newdirfd, const char* newpath );
	
}

#endif

