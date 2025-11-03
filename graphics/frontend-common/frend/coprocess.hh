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

class coprocess_launch
{
	NON_COPYABLE( coprocess_launch )
	
	private:
		int its_pid;
		int its_socket;
	
	public:
		coprocess_launch( int bindir_fd, const char* works_path );
		~coprocess_launch();
		
		int socket() const  { return its_socket; }
};

}

#endif
