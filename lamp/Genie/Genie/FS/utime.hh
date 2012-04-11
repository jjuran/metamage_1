/*
	utime.hh
	--------
*/


// time.h
struct timespec;


namespace Genie
{
	
	class FSTree;
	
	void utime( const FSTree* node, const struct timespec* times );
	
}

