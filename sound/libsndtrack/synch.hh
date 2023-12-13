/*
	synch.hh
	--------
*/

#ifndef SYNCH_HH
#define SYNCH_HH


namespace synch
{
	
	bool start();
	bool wait();
	
	void finished();
	void interrupted();
	
}

#endif
