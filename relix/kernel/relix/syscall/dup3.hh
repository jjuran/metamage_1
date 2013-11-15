/*
	dup3.hh
	-------
*/

#ifndef RELIX_SYSCALL_DUP3_HH
#define RELIX_SYSCALL_DUP3_HH


namespace relix
{
	
	int dup3( int oldfd, int newfd, int flags );
	
}

#endif

