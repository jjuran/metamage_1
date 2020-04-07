/*
	coprocess.hh
	------------
*/

#ifndef COPROCESS_HH
#define COPROCESS_HH

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
		coprocess_launch( const char* raster_path );
		~coprocess_launch();
		
		int socket() const  { return its_socket; }
};

#endif
