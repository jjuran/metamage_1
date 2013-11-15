/*
	dup_fd.hh
	---------
*/

#ifndef RELIX_API_DUPFD_HH
#define RELIX_API_DUPFD_HH


namespace relix
{
	
	int dup_fd( int   old_fd,
	            int   new_fd,
	            bool  close_on_exec = false );
	
}

#endif

