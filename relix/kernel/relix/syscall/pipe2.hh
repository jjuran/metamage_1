/*
	pipe2.hh
	--------
*/

#ifndef RELIX_SYSCALL_PIPE2_HH
#define RELIX_SYSCALL_PIPE2_HH


namespace relix
{
	
	int pipe2( int pipefd[ 2 ], int flags );
	
}

#endif

