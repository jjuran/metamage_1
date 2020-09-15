/*
	linkat.hh
	---------
*/

#ifndef RELIX_SYSCALL_LINKAT_HH
#define RELIX_SYSCALL_LINKAT_HH


namespace relix
{
	
	int linkat( int olddirfd, const char* oldpath, int newdirfd, const char* newpath, int flags );
	
}

#endif
