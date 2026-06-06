/*
	coprocess.hh
	------------
*/

#ifndef FREND_COPROCESS_HH
#define FREND_COPROCESS_HH

// iota
#include "iota/class.hh"


namespace frend
{

struct coprocess_failed {};

struct coprocess_state
{
	int child_pid;
	int socket_fd;
};

coprocess_state launch_coprocess( int bindir_fd, const char* works_path );

int wait_for_coprocess( int pid );

class coprocess_launch
{
	NON_COPYABLE( coprocess_launch )
	
	private:
		coprocess_state its_state;
	
	public:
		coprocess_launch( int bindir_fd, const char* works_path )
		{
			its_state = launch_coprocess( bindir_fd, works_path );
		}
		
		~coprocess_launch()
		{
			wait_for_coprocess( its_state.child_pid );
		}
		
		int socket() const  { return its_state.socket_fd; }
};

}

#endif
