/*
	getdents.hh
	-----------
*/

#ifndef RELIX_SYSCALL_GETDENTS_HH
#define RELIX_SYSCALL_GETDENTS_HH


struct dirent;


namespace relix
{
	
	int getdents( unsigned fd, struct dirent* dirp, unsigned int count );
	
}

#endif
