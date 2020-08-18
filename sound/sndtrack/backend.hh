/*
	backend.hh
	----------
*/

#ifndef BACKEND_HH
#define BACKEND_HH


namespace backend
{
	
	bool start();
	bool wait();
	bool stop();
	
	void finished();
	void interrupted();
	
}

#endif
