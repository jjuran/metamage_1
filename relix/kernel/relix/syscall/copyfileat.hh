/*
	copyfileat.hh
	-------------
*/

#ifndef RELIX_SYSCALL_COPYFILEAT_HH
#define RELIX_SYSCALL_COPYFILEAT_HH


namespace relix
{
	
	int copyfileat( int olddirfd, const char* oldpath, int newdirfd, const char* newpath, unsigned flags );
	
}

#endif

