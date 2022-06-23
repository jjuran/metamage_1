/*
	coprocess.hh
	------------
*/

#ifndef AMICUS_COPROCESS_HH
#define AMICUS_COPROCESS_HH

namespace amicus
{

struct coprocess_failed {};

class coprocess_launch
{
	private:
		int its_pid;
		int its_socket;
		
		// non-copyable
		coprocess_launch           ( const coprocess_launch& );
		coprocess_launch& operator=( const coprocess_launch& );
	
	public:
		coprocess_launch( int bindir_fd, const char* raster_path );
		~coprocess_launch();
		
		int socket() const  { return its_socket; }
};

}

#endif
